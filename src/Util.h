#pragma once
#include <cassert>
#include <cstring>
#include <cstdio>
#include <SdFat.h>
#include <sdios.h>

#ifndef PLOG_ENABLE_WCHAR_INPUT
#       define PLOG_ENABLE_WCHAR_INPUT 0
#endif

#   include <OPEnS_RTC.h>
inline ostream &operator<< (ostream &os, const String &arg) {
  os << arg.c_str();
  return os;
}

#   define PLOG_NSTR(x)    x

namespace plog
{
    namespace util
    {
        typedef String nstring;
        typedef obufstream nostringstream;
        typedef ibufstream nistringstream;
        typedef char nchar;
        extern DateTime dwoke; // RTC time reference
        extern long twoke;     // millis() reference
        extern short tz;       // local timezone (-12 to +12). Half-hour zones not supported

        inline void localtime_s(struct tm* t, const time_t* time)
        {
            
        }

        inline void gmtime_s(struct tm* t, const time_t* time)
        {
            
        }

        struct Time
        {
            DateTime time;
            unsigned short millitm;
        };
        
        inline void ftime(Time* t, long timezone)
        {
            long tv = millis() - twoke;
            t->time = dwoke + TimeSpan(timezone*3600 + tv/1000);
            t->millitm = tv%1000;
        }

        inline void ftime(Time* t)
        {
            return ftime(t, tz);
        }

        inline unsigned int gettid()
        {
            return 0;
        }


#if PLOG_ENABLE_WCHAR_INPUT && !defined(_WIN32)
        inline nstring toNarrow(const wchar_t* wstr)
        {
            size_t wlen = ::wcslen(wstr);
            nstring str(wlen * sizeof(wchar_t), 0);

            if (!str.empty())
            {
                const char* in = reinterpret_cast<const char*>(&wstr[0]);
                char* out = &str[0];
                size_t inBytes = wlen * sizeof(wchar_t);
                size_t outBytes = str.size();

                iconv_t cd = ::iconv_open("UTF-8", "WCHAR_T");
                ::iconv(cd, const_cast<char**>(&in), &inBytes, &out, &outBytes);
                ::iconv_close(cd);

                str.resize(str.size() - outBytes);
            }

            return str;
        }
#endif


        inline nstring processFuncName(const char* func)
        {
            return nstring(func);
        }

        inline const nchar* findExtensionDot(const nchar* fileName)
        {
            return std::strrchr(fileName, '.');
        }

        inline void splitFileName(const nchar* fileName, nstring& fileNameNoExt, nstring& fileExt)
        {
            const nchar* dot = findExtensionDot(fileName);
            if (dot)
            {
                fileNameNoExt = String(fileName);
                fileNameNoExt.remove(dot-fileName);
                fileExt = String(dot + 1);
            }
            else
            {
                fileNameNoExt = String(fileName);
                fileExt = "";
            }
        }

        class NonCopyable
        {
        protected:
            NonCopyable()
            {
            }

        private:
            NonCopyable(const NonCopyable&);
            NonCopyable& operator=(const NonCopyable&);
        };
        
        class File : NonCopyable
        {
        public:
            File()
            {
                FatFile::dateTimeCallback(FatTime);
            }
            
            bool is_open() {
                return f.isOpen() && (f.getError() == 0);
            }

            bool open(const nchar* fileName)
            {
                fname = util::nstring(fileName);
                if (is_open())
                    f.sync();
                f.close();
                f.clearError();
                return f.open(fileName, O_CREAT | O_WRONLY | O_APPEND);
            }
            
            bool open(const util::nstring& fileName)
            {
                return open(fileName.c_str());
            }

            int write(const void* buf, size_t count)
            {
                if(!f.isOpen())
                    return 0;
                return f.write(buf, count);
            }

            int write(const util::nstring& str)
            {
                return write(str.c_str(), str.length());
            }

            bool seek(off_t offset, int whence)
            {
                if (f.isOpen()) {
                    if (whence == SEEK_END)
                        return f.seekEnd(offset);
                    if (whence == SEEK_SET)
                        return f.seekSet(offset);
                    if (whence == SEEK_CUR)
                        return f.seekCur(offset);
                }
                return false;
            }

            bool sync() {
                if (is_open())
                    return f.sync();
                return false;
            }

            bool close()
            {
                if (is_open()) 
                    f.sync();
                return f.close();
            }

            uint32_t cur_position() {
                if (f.isOpen())
                    return f.curPosition();
                return 0;
            }

            uint8_t get_error() {
                return f.getError();
            }

            static bool unlink(const nchar* fileName)
            {
                FatFile rm(fileName, O_RDWR);
                if (rm.isOpen())
                    return rm.remove();
                return true;
            }
            
            static bool unlink(const util::nstring& fileName)
            {
                return unlink(fileName.c_str());
            }

            static bool rename(const nchar* oldFilename, const nchar* newFilename)
            {
                FatFile old(oldFilename, O_RDWR);
                if (old.isOpen())
                    return old.rename(newFilename);
                return false;
            }
            
            static bool rename(const util::nstring& oldName, util::nstring& newName)
            {
                return rename(oldName.c_str(), newName.c_str());
            }

        private:
            util::nstring fname;
            FatFile f;
            static void FatTime(uint16_t* date, uint16_t* time) {
              // User gets date and time from GPS or real-time clock here
              Time t;
              ftime(&t, 0); // UTC timezone
              // return date using FAT_DATE macro to format fields
              *date = FAT_DATE(t.time.year(), t.time.month(), t.time.day());
              // return time using FAT_TIME macro to format fields
              *time = FAT_TIME(t.time.hour(), t.time.minute(), t.time.second());
            }
        };

        template<class T>
        class Singleton : NonCopyable
        {
        public:
            Singleton()
            {
                assert(!m_instance);
                m_instance = static_cast<T*>(this);
            }

            ~Singleton()
            {
                assert(m_instance);
                m_instance = 0;
            }

            static T* getInstance()
            {
                return m_instance;
            }

        private:
            static T* m_instance;
        };

        template<class T>
        T* Singleton<T>::m_instance = NULL;
    }
    // We set the system time in UTC, even if our time source is localtime
    inline void TimeSync( DateTime now, long timezone = 0 ){
        util::twoke = millis();
        util::dwoke = now - TimeSpan(timezone * 3600);
        util::tz = timezone;
    }
}

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
        DateTime dwoke; // RTC time reference
        long twoke;     // millis() reference

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

        inline void ftime(Time* t)
        {
            long tv = millis() - twoke;
            t->time = dwoke + TimeSpan(tv/1000);
            t->millitm = tv%1000;
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
            File() : m_file(-1)
            {
                f.dateTimeCallback(FatTime);
            }

            File(const nchar* fileName) : m_file(-1)
            {
                open(fileName);
            }

            ~File()
            {
                close();
            }

            off_t open(const nchar* fileName)
            {
                f.open(fileName, O_CREAT | O_WRONLY | O_AT_END);
                seek(0, SEEK_END);
            }
            
            off_t open(util::nstring fileName)
            {
                return open(fileName.c_str());
            }

            int write(const void* buf, size_t count)
            {
                int len = f.write(buf, count);
                f.sync();
                return len;
            }
            int write(const String str)
            {
                return write(str.c_str(), str.length());
            }

            off_t seek(off_t offset, int whence)
            {
                if (whence == SEEK_END)
                    f.seekEnd(offset);
                else if (whence == SEEK_SET)
                    f.seekSet(offset);
                else if (whence == SEEK_CUR)
                    f.seekCur(offset);
                return f.curPosition();
            }

            void close()
            {
                if (m_file != -1)
                {
                    f.close();
                    m_file = -1;
                }
            }

            static int unlink(const nchar* fileName)
            {
                FatFile rm(fileName, O_RDWR);
                return rm.remove();
            }
            
            static int unlink(util::nstring fileName)
            {
                return unlink(fileName.c_str());
            }

            static int rename(const nchar* oldFilename, const nchar* newFilename)
            {
                FatFile old(oldFilename, O_RDWR);
                return old.rename(newFilename);
            }
            
            static int rename(util::nstring oldName, util::nstring newName)
            {
                return rename(oldName.c_str(), newName.c_str());
            }

        private:
            int m_file;
            SdBaseFile f;
            static void FatTime(uint16_t* date, uint16_t* time) {
              uint16_t year;
              uint8_t month, day, hour, minute, second;
              // User gets date and time from GPS or real-time clock here
              Time t;
              ftime(&t);
              // return date using FAT_DATE macro to format fields
              *date = FAT_DATE(t.time.year(), t.time.month(), t.time.day());
              // return time using FAT_TIME macro to format fields
              *time = FAT_TIME(t.time.hour(), t.time.minute(), t.time.second());
            }
        };
        class Mutex : NonCopyable
        {
        public:
            Mutex()
            {
            }

            ~Mutex()
            {
            }

            friend class MutexLock;

        private:
            void lock()
            {
            }

            void unlock()
            {
            }

        private:
        };

        class MutexLock : NonCopyable
        {
        public:
            MutexLock(Mutex& mutex) : m_mutex(mutex)
            {
                m_mutex.lock();
            }

            ~MutexLock()
            {
                m_mutex.unlock();
            }

        private:
            Mutex& m_mutex;
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
    void TimeSync( DateTime now ){
        util::twoke = millis();
        util::dwoke = now;
    }
}

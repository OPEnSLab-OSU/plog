#pragma once
#include "../Appenders/IAppender.h"
#include "../Converters/UTF8Converter.h"
#include "../Converters/NativeEOLConverter.h"
#include "../Util.h"
#include "../CircularBuffer.h"
#include "../PlogShortFile.h"

namespace plog
{
    template<class Formatter, class Converter = NativeEOLConverter<UTF8Converter> >
    class RollingFileAppender : public IAppender
    {
    public:
        RollingFileAppender(const util::nchar* fileName, uint32_t time_split_seconds = 360)
            : m_split_seconds(time_split_seconds)
            , m_fileExt()
            , m_fileNameNoExt()
            , m_startfiletime(0)
            , m_curfilename{ 0 }
            , m_backup{ 0 }
            , m_backup_count(0)
            , m_drop_count(0)
        {
            util::splitFileName(fileName, m_fileNameNoExt, m_fileExt);
        }


        virtual void write(const Record& record)
        {
            util::Time cur_time;
            util::ftime(&cur_time);
            long cur_seconds = cur_time.time.secondstime();
            if (m_startfiletime == 0 
                || (cur_seconds > m_startfiletime && cur_seconds - m_startfiletime > (long)m_split_seconds)) {
                m_file.close();
                buildFileName(cur_time);
            }
            
            // reopen the file if needed!
            if (!m_file.is_open())
            {
                if (!openLogFile()) {
                    Serial.println("PLOG file opening failed! Using record cache...");
                    pushToBackup(record);
                    return;
                }
            }

            // write all the backed up records in m_backup, if any
            while (m_backup_count > 0) {
                int res = m_file.write(m_backup, m_backup_count);
                if (res > 0)
                    m_backup_count -= res;
                if (res <= 0)
                    return;
            }

            // file is open! if there's a drop count, log how many entries we dropped
            if (m_drop_count > 0) {
                Record droperror(Severity::warning, __FUNCTION__, __LINE__, __PLOG_SHORT_FILE__, this, 0);
                droperror << "Dropped " << m_drop_count << " records due to SD error";
                if (!writeRecord(droperror)) {
                    // queue the record and return
                    pushToBackup(record);
                    return;
                }
                m_drop_count = 0;
            }

            if (!writeRecord(record))
                pushToBackup(record);
        }

        void sync() {
            m_file.sync();
        }

    private:
        void pushToBackup(const Record& record) {
            util::nstring str = Formatter::format(record);
            if (sizeof(m_backup) - m_backup_count <= str.length()) {
                Serial.println("PLOG Warning: discarding records due to buffer overflow.");
                m_drop_count++;
            }
            // add the formatted string to the backup buffer
            for (size_t i = 0; i < str.length(); i++) {
                m_backup[m_backup_count++] = str.charAt(i);
            }
        }

        bool writeRecord(const Record& record) {
            util::nstring str = Formatter::format(record);
            str = Converter::convert(str);
            return m_file.write(str) > 0;
        }
        
        bool openLogFile()
        {
            if (!m_curfilename[0]) {
                util::Time time;
                ftime(&time);
                buildFileName(time);
            }

            // I was logging a failure here, but it generates too many records to be useful
            if (!m_file.open(m_curfilename))
                return false;

            if (0 == m_file.cur_position()) {
                if(!m_file.write(Converter::header(Formatter::header())))
                    return false;
            }

            return true;
        }

        void buildFileName(const util::Time& time)
        {
            char time_buf[32];
            snprintf(time_buf, sizeof(time_buf), ".%04u_%02u_%02u_%02u_%02u_%02u",
                time.time.year(), time.time.month(), time.time.day(), time.time.hour(), time.time.minute(), time.time.second());
            
            util::nostringstream ss(m_curfilename, 128);
            ss << m_fileNameNoExt << time_buf;
            if (m_fileExt.length())
                ss << '.' << m_fileExt;
            // round starttime down to nearest "time unit"
            const long seconds = time.time.secondstime();
            m_startfiletime = seconds - (seconds % m_split_seconds);
        }

    private:
        const uint32_t  m_split_seconds;
        util::File      m_file;
        util::nstring   m_fileExt;
        util::nstring   m_fileNameNoExt;
        long      m_startfiletime;
        char      m_curfilename[128];
        char      m_backup[2048];
        size_t    m_backup_count;
        uint32_t  m_drop_count;
    };
}

#pragma once
#include "../Appenders/IAppender.h"
#include "../Util.h"

namespace plog
{
    template<class Formatter>
    class SimpleFileAppender : public IAppender
    {
    public:
        SimpleFileAppender(const util::nchar* fileName)
            : m_fileSize()
            , m_firstWrite(true)
            , m_fileName(fileName)
        {}


        virtual void write(const Record& record)
        {

            if (m_firstWrite)
            {
                openLogFile();
                m_firstWrite = false;
            }
            
            util::nstring str = Formatter::format(record);
            
            int bytesWritten = m_file.write(str);
            
            if (bytesWritten > 0)
            {
                m_fileSize += bytesWritten;
            }
            
        }

    private:
        void openLogFile()
        {
            m_fileSize = m_file.open(m_fileName);

            if (0 == m_fileSize)
            {
                int bytesWritten = m_file.write(Formatter::header());

                if (bytesWritten > 0)
                {
                    m_fileSize += bytesWritten;
                }
            }
        }

    private:
        util::File      m_file;
        off_t           m_fileSize;
        util::nstring   m_fileName;
        bool            m_firstWrite;
    };
}

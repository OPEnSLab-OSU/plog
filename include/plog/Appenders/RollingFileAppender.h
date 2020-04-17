#pragma once
#include "../Appenders/IAppender.h"
#include "../Converters/UTF8Converter.h"
#include "../Converters/NativeEOLConverter.h"
#include "../Util.h"

namespace plog
{
    template<class Formatter, class Converter = NativeEOLConverter<UTF8Converter> >
    class RollingFileAppender : public IAppender
    {
    public:
        RollingFileAppender(const util::nchar* fileName, size_t maxFileSize = 0, int maxFiles = 0)
            : m_fileSize()
            , m_maxFileSize(maxFileSize)
            , m_maxFiles(maxFiles)
            , m_firstWrite(true)
        {
            util::splitFileName(fileName, m_fileNameNoExt, m_fileExt);
        }


        virtual void write(const Record& record)
        {
            if (m_firstWrite)
            {
                openLogFile();
            }
            else if (m_maxFiles > 0 && m_fileSize > m_maxFileSize && -1 != m_fileSize)
            {
                rollLogFiles();
            }
            
            util::nstring str = Formatter::format(record);
            
            str = Converter::convert(str);
            
            int bytesWritten = m_file.write(str);
            
            if (bytesWritten > 0)
            {
                m_fileSize += bytesWritten;
            }
            
        }

        void rollLogFiles()
        {
            m_file.close();

            util::nstring lastFileName = buildFileName(m_maxFiles - 1);
            util::File::unlink(lastFileName);

            for (int fileNumber = m_maxFiles - 2; fileNumber >= 0; --fileNumber)
            {
                util::nstring currentFileName = buildFileName(fileNumber);
                util::nstring nextFileName = buildFileName(fileNumber + 1);

                util::File::rename(currentFileName, nextFileName);
            }

            openLogFile();
        }

    private:
        void openLogFile()
        {
            util::nstring fileName = buildFileName();
            m_fileSize = m_file.open(fileName);
            if (0 == m_fileSize)
            {
                m_fileSize = m_file.write(Converter::header(Formatter::header()));
            }
            m_firstWrite = false;
        }

        util::nstring buildFileName(int fileNumber = 0)
        {
            char buf[64];
            util::nostringstream ss(buf, 64);
            ss << m_fileNameNoExt;

            if (fileNumber > 0)
            {
                ss << '.' << fileNumber;
            }
            if (m_fileExt.length())
            {
                ss << '.' << m_fileExt;
            }
            return util::nstring(buf);
        }

    private:
        util::File      m_file;
        off_t           m_fileSize;
        const off_t     m_maxFileSize;
        const int       m_maxFiles;
        util::nstring   m_fileExt;
        util::nstring   m_fileNameNoExt;
        bool            m_firstWrite;
    };
}

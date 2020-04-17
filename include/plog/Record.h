#pragma once
#include <cstdarg>
#include "Severity.h"
#include "Util.h"

#define MAX_MSG_LEN 128

namespace plog
{
    class Record
    {
    public:
        Record(Severity severity, const char* func, size_t line, const char* file, const void* object, int instanceId)
            : m_severity(severity), m_tid(util::gettid()), m_object(object), m_line(line), m_func(func), m_file(file), m_instanceId(instanceId), m_message(new char[MAX_MSG_LEN], MAX_MSG_LEN)
        {
            util::ftime(&m_time);
        }

        Record& ref()
        {
            return *this;
        }

        //////////////////////////////////////////////////////////////////////////
        // Stream output operators

        Record& operator<<(char data)
        {
            char str[] = { data, 0 };
            return *this << str;
        }

#if PLOG_ENABLE_WCHAR_INPUT
        Record& operator<<(wchar_t data)
        {
            wchar_t str[] = { data, 0 };
            return *this << str;
        }
#endif

        Record& operator<<(ostream& (*data)(ostream&))
        {
            m_message << data;
            return *this;
        }

        Record& operator<<(char * data)
        {
            m_message << data;
            return *this;
        }

        template<typename T>
        Record& operator<<(const T& data)
        {
            m_message << data;
            return *this;
        }

        Record& printf(const char* format, ...)
        {
            using namespace util;

            char* str = NULL;
            va_list ap;

            va_start(ap, format);
            size_t len = 0;
            str = vasnprintf(str, &len, format, ap);
            static_cast<void>(len);
            va_end(ap);

            *this << str;
            free(str);

            return *this;
        }


        //////////////////////////////////////////////////////////////////////////
        // Getters

        virtual const util::Time& getTime() const
        {
            return m_time;
        }

        virtual Severity getSeverity() const
        {
            return m_severity;
        }

        virtual unsigned int getTid() const
        {
            return m_tid;
        }

        virtual const void* getObject() const
        {
            return m_object;
        }

        virtual size_t getLine() const
        {
            return m_line;
        }

        virtual const util::nchar* getMessage() const
        {
            return static_cast<const util::nchar*>(
                static_cast<util::nostringstream>(m_message).buf()
            );
        }

        virtual const char* getFunc() const
        {
            m_funcStr = util::processFuncName(m_func);
            return m_funcStr.c_str();
        }

        virtual const char* getFile() const
        {
            return m_file;
        }

        virtual ~Record() // virtual destructor to satisfy -Wnon-virtual-dtor warning
        {
            delete(m_message.buf());
        }

        virtual int getInstanceId() const
        {
            return m_instanceId;
        }

    private:
        util::Time              m_time;
        const Severity          m_severity;
        const unsigned int      m_tid;
        const void* const       m_object;
        const size_t            m_line;
        util::nostringstream    m_message;
        const char* const       m_func;
        const char* const       m_file;
        const int               m_instanceId;
        mutable util::nstring   m_funcStr;
    };
}

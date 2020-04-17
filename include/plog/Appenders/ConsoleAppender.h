#pragma once
#include "../Appenders/IAppender.h"
#include "../Util.h"
#include "../WinApi.h"
#include <iostream>

namespace plog
{
    template<class Formatter>
    class ConsoleAppender : public IAppender
    {
    public:
        ConsoleAppender() : m_isatty(!!isatty(fileno(stdout))) {}

        virtual void write(const Record& record)
        {
            util::nstring str = Formatter::format(record);

            writestr(str);
        }

    protected:
        void writestr(const util::nstring& str)
        {
            std::cout << str << std::flush;
        }

    private:

    protected:
        const bool  m_isatty;
    };
}

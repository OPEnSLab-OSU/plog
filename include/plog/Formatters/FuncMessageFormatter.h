#pragma once
#include "../Record.h"
#include "../Util.h"

namespace plog
{
    class FuncMessageFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            char buf[MAX_MSG_LEN+32];
            util::nostringstream ss(buf, MAX_MSG_LEN+32);
            ss << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR(": ") << record.getMessage() << PLOG_NSTR("\n");
            util::nstring str(ss.buf());
            return str;
        }
    };
}

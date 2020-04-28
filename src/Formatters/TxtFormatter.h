#pragma once
#include "../Record.h"
#include "../Util.h"

namespace plog
{
    template<bool useUtcTime>
    class TxtFormatterImpl
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        {
            char buf[MAX_MSG_LEN+128];
            util::nostringstream ss(buf, MAX_MSG_LEN+128);
            DateTime t = record.getTime().time;
            ss << int(t.year()) << "-" << setfill(PLOG_NSTR('0')) << setw(2) << short(t.month()) << PLOG_NSTR("-") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.day()) << PLOG_NSTR(" ");
            ss << setfill(PLOG_NSTR('0')) << setw(2) << short(t.hour()) << PLOG_NSTR(":") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.minute()) << PLOG_NSTR(":") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.second()) << PLOG_NSTR(".") << setfill(PLOG_NSTR('0')) << setw(3) << record.getTime().millitm << PLOG_NSTR(" ");
            ss << setfill(PLOG_NSTR(' ')) << setw(5) << left << severityToString(record.getSeverity()) << PLOG_NSTR(" ");
            ss << PLOG_NSTR("[") << record.getTid() << PLOG_NSTR("] ");
            ss << PLOG_NSTR("[") << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR("] ");
            ss << record.getMessage() << PLOG_NSTR("\n");
            util::nstring str(ss.buf());
            return str;
        }
    };

    class TxtFormatter : public TxtFormatterImpl<false> {};
    class TxtFormatterUtcTime : public TxtFormatterImpl<true> {};
}

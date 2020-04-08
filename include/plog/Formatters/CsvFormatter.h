#pragma once
#include "../Record.h"
#include "../Util.h"

namespace plog
{
    template<bool useUtcTime>
    class CsvFormatterImpl
    {
    public:
        static util::nstring header()
        {
            return PLOG_NSTR("Date;Time;Severity;TID;This;Function;Message\n");
        }

        static util::nstring format(const Record& record)
        {
            /* I added extra space to this buffer. As long as your function
             * name is less than 60 characters, you should have room for the full
             * message (as limited by MAX_MSG_LEN) */
            char buf[MAX_MSG_LEN+128];
            util::nostringstream ss(buf, MAX_MSG_LEN+128);
            DateTime t = record.getTime().time;
            ss << t.year() << PLOG_NSTR("/") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.month()) << PLOG_NSTR("/") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.day()) << PLOG_NSTR(";");
            ss << setfill(PLOG_NSTR('0')) << setw(2) << short(t.hour()) << PLOG_NSTR(":") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.minute()) << PLOG_NSTR(":") << setfill(PLOG_NSTR('0')) << setw(2) << short(t.second()) << PLOG_NSTR(".") << setfill(PLOG_NSTR('0')) << setw(3) << record.getTime().millitm << PLOG_NSTR(";");
            ss << severityToString(record.getSeverity()) << PLOG_NSTR(";");
            ss << record.getTid() << PLOG_NSTR(";");
            ss << record.getObject() << PLOG_NSTR(";");
            ss << record.getFunc() << PLOG_NSTR("@") << record.getLine() << PLOG_NSTR(";");

            ss << '"'<< record.getMessage() << "\"\n";
            //FIXME: Sanitize the message string by escaping quotes

            util::nstring str(ss.buf());
            return str; 
        }
    };

    class CsvFormatter : public CsvFormatterImpl<false> {};
    class CsvFormatterUtcTime : public CsvFormatterImpl<true> {};
}

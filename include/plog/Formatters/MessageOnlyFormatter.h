#pragma once
#include "../Record.h"
#include "../Util.h"

namespace plog
{
    class MessageOnlyFormatter
    {
    public:
        static util::nstring header()
        {
            return util::nstring();
        }

        static util::nstring format(const Record& record)
        { 
            util::nstring str = record.getMessage();
            
            str += PLOG_NSTR("\n");
            
            return str;
        }
    };
}

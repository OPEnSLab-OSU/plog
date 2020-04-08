#pragma once
#include "../Appenders/IAppender.h"
#include "../Util.h"

namespace plog
{
    template<class Formatter>
    class SerialAppender : public IAppender
    {
    public:
        SerialAppender(Print& where) : ser(where)
        {
        }
        
        SerialAppender()
        {
            SerialAppender(Serial);
        }

        virtual void write(const Record& record)
        {
            util::nstring str = Formatter::format(record);
        
            writestr(str);
            
        }

    protected:
        void writestr(const util::nstring& str)
        {
            ser.print( str );
        }

    private:

    protected:
        Print& ser;
    };
}

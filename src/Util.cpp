#include "Util.h"

namespace plog
{
    namespace util
    {
        DateTime dwoke; // RTC time reference
        long twoke;     // millis() reference
        short tz;       // local timezone (-12 to +12). Half-hour zones not supported
    }
}

#pragma once
#include "../Converters/UTF8Converter.h"
#include "../Util.h"

namespace plog
{
    template<class NextConverter = UTF8Converter>
    class NativeEOLConverter : public NextConverter
    {
    };
}

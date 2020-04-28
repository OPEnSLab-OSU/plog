#pragma once
#include "../Util.h"

namespace plog
{
    class UTF8Converter
    {
    public:
        static util::nstring& header(const util::nstring& str)
        {
            const char kBOM[] = "\xEF\xBB\xBF";

            return util::nstring(kBOM) + convert(str);
        }

        static const util::nstring& convert(const util::nstring& str)
        {
            return str;
        }
    };
}

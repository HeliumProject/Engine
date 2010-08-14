#pragma once

#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Controls/Label.h"
#include "Application/Inspect/Controls/Canvas.h"

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API Script
        {
        public:
            static void Initialize();
            static void Cleanup();

        private:
            static bool PreProcess(tstring& script);
            static void ParseAttributes(tstring& attributes, Control* control);

        public:
            // parses script data and binds controls to data
            static bool Parse(const tstring& script, Interpreter* interpreter, Canvas* canvas, Container* output, u32 fieldFlags = 0);
        };
    }
}
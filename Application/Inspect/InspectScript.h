#pragma once

#include "Application/Inspect/InspectData.h"
#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectCanvas.h"

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
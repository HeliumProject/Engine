#pragma once

#include "Foundation/Inspect/InspectData.h"
#include "Foundation/Inspect/InspectInterpreter.h"
#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Controls/CanvasControl.h"

namespace Helium
{
    namespace Inspect
    {
        class FOUNDATION_API Script
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
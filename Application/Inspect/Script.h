#pragma once

#include "Widgets/Canvas.h"
#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Widgets/Other Controls/Label.h"

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
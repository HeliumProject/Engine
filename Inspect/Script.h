#pragma once

#include "Canvas.h"
#include "Data.h"
#include "Label.h"

namespace Inspect
{
  class INSPECT_API Script
  {
  public:
    static void Initialize();
    static void Cleanup();

  private:
    static bool PreProcess(std::string& script);
    static void ParseAttributes(std::string& attributes, Control* control);

  public:
    // parses script data and binds controls to data
    static bool Parse(const std::string& script, Interpreter* interpreter, Canvas* canvas, Container* output, u32 fieldFlags = 0);
  };
}
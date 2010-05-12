#pragma once

#include "API.h"
#include "Console.h"

namespace Console
{
  class CONSOLE_API Listener
  {
  public:
    Listener( u32 throttle = Console::Streams::All, u32* errorCount = NULL, u32* warningCount = NULL, Console::V_Statement* consoleOutput = NULL );
    ~Listener();

    void Start();
    void Stop();
    void Dump(bool stop = true);

    u32 GetWarningCount();
    u32 GetErrorCount();

  private:
    void Print( Console::PrintingArgs& args );

  private:
    u32                   m_Thread;
    u32                   m_Throttle;
    u32*                  m_ErrorCount;
    u32*                  m_WarningCount;
    Console::V_Statement* m_ConsoleOutput;
  };
}
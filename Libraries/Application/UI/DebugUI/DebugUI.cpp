#include <wx/wx.h>
#include <wx/choicdlg.h>
#include <wx/msw/private.h>

#if ( wxUSE_ON_FATAL_EXCEPTION == 1 || wxUSE_STACKWALKER == 1 || wxUSE_DEBUGREPORT == 1 || wxUSE_EXCEPTIONS == 1 || wxUSE_CRASHREPORT == 1 )
#pragma message( "WARNING: wxWidgets exception handling is enabled!" )
#endif

#include "DebugUI.h"

#include "Debug/Exception.h"
#include "Platform/Windows/Console.h"

static int ShowBreakpointDialog(const Debug::BreakpointArgs& args )
{
  static std::set<DWORD> disabled;
  static bool skipAll = false;
  bool skip = skipAll;

  // are we NOT skipping everything?
  if (!skipAll)
  {
    // have we disabled this break point?
    if (disabled.find(args.m_Info->ContextRecord->IPREG) != disabled.end())
    {
      skip = true;
    }
    // we have NOT disabled this break point yet
    else
    {
      Debug::ExceptionArgs exArgs ( Debug::ExceptionTypes::SEH, args.m_Fatal ); 
      Debug::GetExceptionDetails( args.m_Info, exArgs ); 

      // dump args.m_Info to console
      Platform::Print(Platform::ConsoleColors::Red, stderr, "%s", Debug::GetExceptionInfo(args.m_Info).c_str());

      // display result
      std::string message ("A break point was triggered in the application:\n\n");
      message += Debug::GetSymbolInfo( args.m_Info->ContextRecord->IPREG );
      message += std::string ("\n\nWhat do you wish to do?");

      const char* nothing = "Let the OS handle this as an exception";
      const char* thisOnce = "Skip this break point once";
      const char* thisDisable = "Skip this break point and disable it";
      const char* allDisable = "Skip all break points";

      wxArrayString choices;
      choices.Add(nothing);
      choices.Add(thisOnce);
      choices.Add(thisDisable);
      choices.Add(allDisable);
      wxString choice = ::wxGetSingleChoice( message.c_str(), "Break Point Triggered", choices );

      if (choice == nothing)
      {
        // we are not continuable, so unhook the top level filter
        SetUnhandledExceptionFilter( NULL );

        // this should let the OS prompt for the debugger
        return EXCEPTION_CONTINUE_SEARCH;
      }
      else if (choice == thisOnce)
      {
        skip = true;
      }
      else if (choice == thisDisable)
      {
        skip = true;
        disabled.insert(args.m_Info->ContextRecord->IPREG);
      }
      else if (choice == allDisable)
      {
        skip = true;
        skipAll = true;
      }
    }
  }

  if (skipAll || skip)
  {
    // skip break instruction (move the ip ahead one byte)
    args.m_Info->ContextRecord->IPREG += 1;

    // continue execution past the break instruction
    return EXCEPTION_CONTINUE_EXECUTION;
  }
  else
  {
    // fall through and let window's crash API run
    return EXCEPTION_CONTINUE_SEARCH;
  }
}

static u32 g_InitCount = 0;

void DebugUI::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    Debug::g_BreakpointOccurred.Set( &ShowBreakpointDialog );
  }
}

void DebugUI::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    Debug::g_BreakpointOccurred.Clear();
  }
}
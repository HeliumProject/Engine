#pragma once

#include "Common/Compiler.h"
#include "Common/InitializerStack.h"

namespace Luna
{
  class Application : public wxApp
  {
  private:
    Nocturnal::InitializerStack m_InitializerStack;

  public:
    virtual void  OnInitCmdLine( wxCmdLineParser& parser ) NOC_OVERRIDE;
    virtual bool  OnCmdLineParsed( wxCmdLineParser& parser ) NOC_OVERRIDE;
    virtual int   OnRun() NOC_OVERRIDE;
    virtual int   OnExit() NOC_OVERRIDE;

    std::string ShowFileBrowser();
  };

  DECLARE_APP( Application );
}
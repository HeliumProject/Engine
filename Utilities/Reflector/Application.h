#pragma once

class ReflectorApp : public wxApp
{
public:
  int  OnRun();
  int  OnExit();
};

DECLARE_APP(ReflectorApp);
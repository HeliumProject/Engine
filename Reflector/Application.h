#pragma once

class Application : public wxApp
{
public:
  int  OnRun();
  int  OnExit();
};

DECLARE_APP(Application);
#pragma once

#include "API.h"
#include "Task.h"

#include <string>
#include <vector>

#include "wx/wx.h"
#include "wx/textctrl.h"
#include "wx/button.h"

#include "Foundation/Log.h"

namespace Luna
{
  class TaskOutputWindow;

  class UpdateTimer : public wxTimer
  {
  protected:
    TaskOutputWindow* m_Window;

  public:
    UpdateTimer( TaskOutputWindow* window );

    void Notify();
  };

  typedef Nocturnal::Signature<void, Nocturnal::Void> SignalSignature;

  class TaskOutputWindow : public wxFrame
  {
  protected:
    wxTextCtrl* m_TextControl;
    wxButton* m_CloseButton;

    CRITICAL_SECTION m_CriticalSection;

    Log::V_Statement m_Statements;
    UpdateTimer* m_MessageTimer;

  public:
    TaskOutputWindow( wxWindow* parent, const std::string& title, int xpos, int ypos, int width, int height );
    ~TaskOutputWindow();

    wxTextCtrl* GetTextCtrl()
    {
      return m_TextControl;
    }

    Log::V_Statement& GetPendingStatements()
    {
      return m_Statements;
    }

    // for protecting message queue
    void EnterMessageSection();
    void LeaveMessageSection();

    // translate color definition
    const wxColour& TranslateColor( Log::Color color );

    // insomniac event handlers
    void TaskStarted( const TaskStartedArgs& args );
    void TaskFinished( const TaskFinishedArgs& args );
    void PrintListener( const Log::PrintedArgs& args );

    // wx event handlers
    void OnClose( wxCommandEvent& event );

  private:
    SignalSignature::Event m_Signal;
  public:
    void AddSignalListener(const SignalSignature::Delegate& listener)
    {
      m_Signal.Add( listener );
    }
    void RemoveSignalListener(const SignalSignature::Delegate& listener)
    {
      m_Signal.Remove( listener );
    }

    DECLARE_EVENT_TABLE();
  };
}
#pragma once

#include "Platform/Compiler.h"
#include "Editor/PreviewWindow.h"

namespace Reflect
{
  struct ElementChangeArgs;
}

namespace Luna
{
  class BrowserFrame;

  class BrowserPreviewWindow : public PreviewWindow
  {
  public:
    BrowserPreviewWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = wxT( "Luna::BrowserPreviewWindow" ) );
    virtual ~BrowserPreviewWindow();

    void SetBrowserFrame( BrowserFrame* browserFrame );

    virtual void DisplayReferenceAxis( bool display ) NOC_OVERRIDE;

  private:
    void OnPreferencesChanged( const Reflect::ElementChangeArgs& args );

  private:
    BrowserFrame* m_BrowserFrame;

  private:
    DECLARE_EVENT_TABLE();
  };
}

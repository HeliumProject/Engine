#pragma once

#include "Platform/Compiler.h"
#include "PreviewWindow.h"

namespace Reflect
{
  struct ElementChangeArgs;
}

namespace Luna
{
  class VaultFrame;

  class VaultPreviewWindow : public PreviewWindow
  {
  public:
    VaultPreviewWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = wxT( "Luna::VaultPreviewWindow" ) );
    virtual ~VaultPreviewWindow();

    void SetVaultFrame( VaultFrame* browserFrame );

    virtual void DisplayReferenceAxis( bool display ) NOC_OVERRIDE;

  private:
    void OnPreferencesChanged( const Reflect::ElementChangeArgs& args );

  private:
    VaultFrame* m_VaultFrame;

  private:
    DECLARE_EVENT_TABLE();
  };
}

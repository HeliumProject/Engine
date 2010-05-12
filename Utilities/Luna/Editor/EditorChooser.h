#pragma once

#include "Common/Types.h"
#include "Common/Automation/Event.h"
#include "Windows/Atomic.h"

class EditorChooserInnerPanel;

namespace Luna
{
  class RunGameFrame;

  typedef Nocturnal::Signature<void, Nocturnal::Void, ::Windows::AtomicRefCountBase> AssetsUpdatedSignature;

  class EditorChooser : public wxPanel
  {
  private:
    EditorChooserInnerPanel* m_Panel;
    RunGameFrame* m_RunGame;

  public:
    EditorChooser( wxWindow* parent );
    virtual ~EditorChooser();

  private:
    void OnButton( wxCommandEvent& event );
    void OnCloseRunGame( wxCloseEvent& event );

  private:
    static AssetsUpdatedSignature::Event s_AssetsUpdated;
  public:
    static void AddAssetsUpdatedListener(AssetsUpdatedSignature::Delegate& delegate)
    {
      s_AssetsUpdated.Add( delegate );
    }
    static void RemoveAssetsUpdatedListener(AssetsUpdatedSignature::Delegate& delegate)
    {
      s_AssetsUpdated.Remove( delegate );
    }
  };
}

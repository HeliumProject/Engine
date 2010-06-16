#include "ModalFrame.h"

#include <wx/app.h>
#include <wx/evtloop.h>
#include <wx/ptr_scpd.h>
#include <wx/settings.h>
#include <wx/msw/private.h>

namespace Nocturnal
{

  /////////////////////////////////////////////////////////////////////////////
  // ModalFrameData
  // this is simply a container for any data we need to implement modality which
  // allows us to avoid changing wxDialog each time the implementation changes
  class ModalFrameData
  {
  public:
    ModalFrameData( wxWindow* winModal )
      : m_evtLoop( winModal )
    {
    }

    void RunLoop()
    {
      m_evtLoop.Run();
    }

    void ExitLoop()
    {
      m_evtLoop.Exit();
    }

  private:
    wxModalEventLoop m_evtLoop;
  };

  // Defines ModalFrameDataTiedPtrc
  wxDEFINE_TIED_SCOPED_PTR_TYPE( ModalFrameData );



  /////////////////////////////////////////////////////////////////////////////
  //
  // ModalFrame class
  //
  /////////////////////////////////////////////////////////////////////////////

  // Required so that OnCompareItems will be called
  IMPLEMENT_DYNAMIC_CLASS( ModalFrame, wxFrame )

  ModalFrame::ModalFrame()
  {
    Init();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Initialize the dialog
  ModalFrame::ModalFrame
    (
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name
    )
  {
    Init();

    Create( parent, id, title, pos, size, style, name );
  }


  /////////////////////////////////////////////////////////////////////////////
  // Initialize the dialog
  //
  void ModalFrame::Init()
  {
    m_oldFocus              = (wxWindow *)NULL;
    m_isShown               = false;
    m_modalData             = NULL;
    m_endModalCalled        = false;
    m_returnCode            = wxID_CANCEL;
  }


  /////////////////////////////////////////////////////////////////////////////
  // 
  //
  bool ModalFrame::Create
    (
    wxWindow *parent,
    wxWindowID id,
    const wxString& title,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name
    )
  {
    if ( !__super::Create( parent, id, title, pos, size, style, name ) )
      return false;

    SetExtraStyle( GetExtraStyle() | wxTOPLEVEL_EX_DIALOG );

    // save focus before doing anything which can potentially change it
    m_oldFocus = FindFocus();

    // All dialogs should really have this style
    style |= wxTAB_TRAVERSAL;

    if ( !m_hasFont )
      SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

    return true;
  }


  /////////////////////////////////////////////////////////////////////////////
  // Dtor, deletes the wxWidget objects
  // and this will also reenable all the other windows for a modal dialog
  // 
  ModalFrame::~ModalFrame()
  {    
    m_isBeingDeleted = true;

    Hide();
  }


  /////////////////////////////////////////////////////////////////////////////
  // Used to disable parent windows for modal state
  // Copied from: X:\sdk_depot\wxWidgets\2.8.4\src\msw\dialog.cpp
  //
  wxWindow* ModalFrame::FindSuitableParent() const
  {
    wxWindow *parent = wxTheApp->GetTopWindow();

    // finally, check if the parent we found is really suitable
    if ( !parent || parent == (wxWindow *)this || !parent->IsShown() )
    {
      // don't use this one
      parent = NULL;
    }

    return parent;
  }


  /////////////////////////////////////////////////////////////////////////////
  // is the dialog in modal state right now? 
  //
  bool ModalFrame::IsModal() const 
  { 
    return m_modalData != NULL;
  }

  /////////////////////////////////////////////////////////////////////////////
  // show the dialog modally and return the value passed to EndModal()
  //
  bool ModalFrame::Show( bool show )
  {
    if ( show == IsShown() )
      return false;

    if ( !show && m_modalData )
    {
      // we need to do this before calling wxDialogBase version because if we
      // had disabled other app windows, they must be reenabled right now as
      // if they stay disabled Windows will activate another window (one
      // which is enabled, anyhow) when we're hidden in the base class Show()
      // and we will lose activation
      m_modalData->ExitLoop();
    }

    if ( show )
    {
      // this usually will result in TransferDataToWindow() being called
      // which will change the controls values so do it before showing as
      // otherwise we could have some flicker
      InitDialog();
    }

    __super::Show(show);

    if ( show )
    {
      // dialogs don't get WM_SIZE message after creation unlike most (all?)
      // other windows and so could start their life non laid out correctly
      // if we didn't call Layout() from here
      //
      // NB: normally we should call it just the first time but doing it
      //     every time is simpler than keeping a flag
      Layout();
    }

    return true;
  }


  /////////////////////////////////////////////////////////////////////////////
  // show the dialog modally and return the value passed to EndModal()
  //
  void ModalFrame::Raise()
  {
      ShowModal();
//    ::SetForegroundWindow(GetHwnd());
  }


  /////////////////////////////////////////////////////////////////////////////
  // For now, same as Show(true) but returns return code
  // show the dialog modally and return the value passed to EndModal()
  //
  int ModalFrame::ShowModal()
  {
    wxASSERT_MSG( !IsModal(), _T("wxDialog::ShowModal() reentered?") );

    m_endModalCalled = false;

    Show( true );

    // EndModal may have been called from InitDialog handler (called from
    // inside Show()), which would cause an infinite loop if we didn't take it
    // into account
    if ( !m_endModalCalled )
    {
      // modal dialog needs a parent window, so try to find one
      wxWindow *parent = GetParent();
      if ( !parent )
      {
        parent = FindSuitableParent();
      }

      // remember where the focus was
      wxWindow *oldFocus = m_oldFocus;
      if ( !oldFocus )
      {
        // VZ: do we really want to do this?
        oldFocus = parent;
      }

      // We have to remember the HWND because we need to check
      // the HWND still exists (oldFocus can be garbage when the dialog
      // exits, if it has been destroyed)
      HWND hwndOldFocus = oldFocus ? GetHwndOf(oldFocus) : NULL;


      // enter and run the modal loop
      {
        ModalFrameDataTiedPtr modalData(&m_modalData,
          new ModalFrameData(this));
        modalData->RunLoop();
      }


      // and restore focus
      // Note that this code MUST NOT access the dialog object's data
      // in case the object has been deleted (which will be the case
      // for a modal dialog that has been destroyed before calling EndModal).
      if ( oldFocus && ( oldFocus != this) && ::IsWindow( hwndOldFocus ) )
      {
        // This is likely to prove that the object still exists
        if ( wxFindWinFromHandle((WXHWND) hwndOldFocus) == oldFocus )
          oldFocus->SetFocus();
      }
    }

    return GetReturnCode();
  }


  /////////////////////////////////////////////////////////////////////////////
  // may be called to terminate the dialog with the given return code
  //
  void ModalFrame::EndModal( int returnCode )
  {
    wxASSERT_MSG( IsModal(), _T( "EndModal() called for non modal dialog" ) );

    m_endModalCalled = true;

    SetReturnCode( returnCode );

    Hide();
  }

  /////////////////////////////////////////////////////////////////////////////
  // Set the return code
  //
  void ModalFrame::SetReturnCode( int returnCode )
  { 
    m_returnCode = returnCode; 
  }

  /////////////////////////////////////////////////////////////////////////////
  // Get the return code
  //
  int ModalFrame::GetReturnCode() const
  {
    return m_returnCode; 
  }

} // namespace Nocturnal
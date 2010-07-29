#pragma once

#include "Application/API.h"

#include <wx/frame.h>

namespace Helium
{
  // Forwards
  class ModalFrameData;

  /////////////////////////////////////////////////////////////////////////////
  // 
  // 
  class APPLICATION_API ModalFrame : public wxFrame 
  {
  public:

    //
    // Ctor/Dtor
    //
    ModalFrame();

    ModalFrame(
      wxWindow *parent,
      wxWindowID id = wxID_ANY,
      const wxString& title = wxEmptyString,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL,
      const wxString& name = wxFrameNameStr );

    virtual ~ModalFrame();

    //
    // wxDialog API
    //

    virtual int ShowModal();
    virtual void EndModal( int returnCode = wxID_CANCEL );
    
    virtual bool IsModal() const;

    virtual void SetReturnCode( int returnCode );
    virtual int GetReturnCode() const;

    // HELIUM_OVERRIDE some base class virtuals
    virtual bool Show( bool show = true );
    virtual void Raise();

    // Windows callbacks
    //WXLRESULT MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  protected:

    // common part of all ctors
    void Init();

    bool Create(
      wxWindow *parent,
      wxWindowID id = wxID_ANY,
      const wxString& title = wxEmptyString,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL,
      const wxString& name = wxFrameNameStr );    

  private:

    DECLARE_DYNAMIC_CLASS( ModalFrame )
    DECLARE_NO_COPY_CLASS( ModalFrame )
    
    //
    // Helpers
    //

    wxWindow* FindSuitableParent() const;


    //
    // wxDialog Members
    //

    wxWindow*                 m_oldFocus;
    bool                      m_endModalCalled;   // allow for closing within InitDialog
    ModalFrameData*           m_modalData;        // this pointer is non-NULL only while the modal event loop is running
    int                       m_returnCode;       // The return code from modal dialog


  };

} // namespace Helium
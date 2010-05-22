#pragma once

#include "API.h"


namespace File
{
  /////////////////////////////////////////////////////////////////////////////
  // Editor for property grid items of type PropertyGridFilePicker.  Provides
  // two different buttons for browsing for a file.
  // 
  class FILEBROWSER_API FilePickerEditor : public wxPGTextCtrlEditor
  {
    WX_PG_DECLARE_EDITOR_CLASS( FilePickerEditor );

  private:
    mutable int m_Button1ID;
    mutable int m_Button2ID;

  public:
    FilePickerEditor();
    virtual ~FilePickerEditor();
    static FilePickerEditor* GetInstance();
    virtual wxPGWindowList CreateControls( wxPropertyGrid* propgrid, wxPGProperty* property, const wxPoint& pos, const wxSize& size ) const NOC_OVERRIDE;
    int GetButton1ID() const;
    int GetButton2ID() const;
  };
}

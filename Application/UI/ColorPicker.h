#pragma once

#include "Application/API.h"
#include "Platform/Types.h"

#include <wx/clrpicker.h>

namespace Helium
{
  /////////////////////////////////////////////////////////////////////////////
  // A color picker is a control that consists of a colored button.  Clicking
  // on the button shows a color picker dialog (which has slots where the user
  // can save custom colors).  Picking a new color in this dialog updates the 
  // color of the button.  This implementation provides functions for saving
  // and loading the custom colors of the color picker dialog.  The base class
  // uses a static variable to store these colors, but they don't persist 
  // because the variable is reset everytime a new color picker control is
  // created.  This implementation provides a workaround for this limitation.
  // 
  class APPLICATION_API ColorPicker : public wxColourPickerCtrl
  {
  private:
    bool m_AutoSaveCustomColors;
    tstring m_RegistryLocation;
    tstring m_Key;

  public:
    ColorPicker( wxWindow* parent, wxWindowID id, const wxColour& col = *wxBLACK, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLRP_DEFAULT_STYLE, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT( "Editor::ColorPicker" ) );
    virtual ~ColorPicker();

    bool SaveCustomColors( tstring& colors );
    bool LoadCustomColors( const tstring& colors );

    void EnableAutoSaveCustomColors( bool enable = true, const tstring& key = TXT( "" ), const tstring& registryLocation = TXT( "" ) );
    void DisableAutoSaveCustomColors();

  protected:
    void OnButtonClick( wxCommandEvent& args );
  };
}

#pragma once

#include "API.h"


namespace FileBrowser
{
  /////////////////////////////////////////////////////////////////////////////
  // Property for choosing a file path, used in a property grid.  Automatically
  // assigns itself to use the FilePickerEditor which provides two buttons for
  // browsing for a file.  One of the buttons is for bringing up the normal file
  // picker dialog.  The other button shows the searchable Asset Finder dialog.
  // 
  class FILEBROWSER_API PropertyGridFilePicker : public wxStringProperty
  {
    WX_PG_DECLARE_DERIVED_PROPERTY_CLASS( PropertyGridFilePicker );

  private:
    long m_Flags;
    wxString m_Title;
    wxString m_Filter;

  public:
    PropertyGridFilePicker( const wxString& name = wxPG_LABEL, const wxString& label = wxPG_LABEL, const wxString& value = wxEmptyString );
    virtual ~PropertyGridFilePicker();

   virtual bool OnEvent( wxPropertyGrid* propGrid, wxWindow* primary, wxEvent& event ) NOC_OVERRIDE;

  protected:
    bool ShowManagedFileDialog( wxPropertyGrid* propGrid, wxString& value );
    bool ShowFileBrowser( wxPropertyGrid* propGrid, wxString& value );

  public:
    WX_PG_DECLARE_VALIDATOR_METHODS();
    WX_PG_DECLARE_ATTRIBUTE_METHODS();
  };
}

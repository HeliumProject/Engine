#include "stdafx.h"
#include "PropertyGridFilePicker.h"

#include "FileBrowser.h"
#include "FilePickerEditor.h"

#include "Common/Assert.h"
#include "FileSystem/FileSystem.h"
#include "FileUI/ManagedFileDialog.h"
#include "UIToolKit/PropertyGridAttribs.h"

namespace FileBrowser
{
  WX_PG_IMPLEMENT_DERIVED_PROPERTY_CLASS( PropertyGridFilePicker, wxStringProperty, const wxString& );

  /////////////////////////////////////////////////////////////////////////////
  // Constructor
  // 
  PropertyGridFilePicker::PropertyGridFilePicker( const wxString& name, const wxString& label, const wxString& value )
    : wxStringProperty( name, label, value )
    , m_Flags( UIToolKit::FileDialogStyles::DefaultSave )
    , m_Title( wxFileSelectorPromptStr )
  {
    SetEditor( FilePickerEditor::GetInstance() );
  }

  /////////////////////////////////////////////////////////////////////////////
  // Destructor
  // 
  PropertyGridFilePicker::~PropertyGridFilePicker()
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Main event handler.  Listens for button click events and handles each of
  // the two possible ones that come from the FilePickerEditor.
  // 
  bool PropertyGridFilePicker::OnEvent( wxPropertyGrid* propGrid, wxWindow* primary, wxEvent& event )
  {
    bool handled = false;

    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
      wxString value( GetValueAsString() );
      wxString original( value );

      if ( event.GetId() == FilePickerEditor::GetInstance()->GetButton1ID() )
      {
        handled = ShowManagedFileDialog( propGrid, value );
      }
      else if ( event.GetId() == FilePickerEditor::GetInstance()->GetButton2ID() )
      {
        handled = ShowFileBrowser( propGrid, value );
      }

      if ( handled && value != original )
      {
        propGrid->ChangePropertyValue( this, value );
      }
    }
    
    if ( !handled )
    {
      handled = __super::OnEvent( propGrid, primary, event );
    }

    return handled;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Callback for when the "..." button is pressed.  Displays the dialog for
  // selecting a file location.
  // 
  bool PropertyGridFilePicker::ShowManagedFileDialog( wxPropertyGrid* propGrid, wxString& value )
  {
    bool result = false;
    File::ManagedFileDialog dlg( propGrid, m_Title, wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, m_Flags );

    if ( !m_Filter.empty() )
    {
      dlg.SetFilter( m_Filter.c_str() );
    }

    if ( dlg.ShowModal() == wxID_OK )
    {
      value = dlg.GetFilePath().c_str();
      result = true;
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Callback for when the search button is pressed.  Displays the FileBrowser.
  // 
  bool PropertyGridFilePicker::ShowFileBrowser( wxPropertyGrid* propGrid, wxString& value )
  {
    bool result = false;

    File::FileBrowser fileBrowser( propGrid, wxID_ANY, m_Title.c_str() );
    fileBrowser.SetFilter( m_Filter.c_str() );

    if ( fileBrowser.ShowModal() == wxID_OK )
    {
      std::string path = fileBrowser.GetPath().c_str();
      value = path.c_str();
      result = true;
    }

    return result;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Returns the validator for this class.
  // 
  wxValidator* PropertyGridFilePicker::DoGetValidator() const
  {
#pragma TODO( "Implement wxValidator-derived class" )
    return NULL;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Set attributes on this class.
  // 
  bool PropertyGridFilePicker::DoSetAttribute( const wxString& name, wxVariant& value )
  {
    if ( name == UIToolKit::PropertyGridAttribs::DialogTitle )
    {
      m_Title = value.GetString();
      return true;
    }

    if ( name == UIToolKit::PropertyGridAttribs::DialogFlags )
    {
      m_Flags = value.GetLong();
      return true;
    }

    if ( name == UIToolKit::PropertyGridAttribs::DialogFilter )
    {
      m_Filter = value.GetString();
      return true;
    }

    return false;
  }
}
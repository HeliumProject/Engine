#include "StdAfx.h"

#include "ManagedFileProperty.h"

#include "ManagedFileDialog.h"

#include "Common/Assert.h"
#include "UIToolKit/PropertyGridAttribs.h"

namespace File
{
  WX_PG_IMPLEMENT_PROPERTY_CLASS(ManagedFileProperty,wxLongStringProperty, wxString, const wxString&, TextCtrlAndButton);

  ManagedFileProperty::ManagedFileProperty( const wxString& name, const wxString& label, const wxString& value )
    : wxLongStringProperty( name, label, value )
    , m_Flags( UIToolKit::FileDialogStyles::DefaultSave )
    , m_Title( wxFileSelectorPromptStr )
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Destructor
  // 
  ManagedFileProperty::~ManagedFileProperty()
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Callback for when the "..." button is pressed.  Displays the dialog for
  // selecting a file location.
  // 
  bool ManagedFileProperty::OnButtonClick( wxPropertyGrid* propGrid, wxString& value )
  {
    bool result = false;
    ManagedFileDialog dlg( propGrid, m_Title, wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, m_Flags );

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
  // Returns the validator for this class.
  // 
  wxValidator* ManagedFileProperty::DoGetValidator() const
  {
#pragma TODO( "Implement wxValidator-derived class" )
    return NULL;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Set attributes on this class.
  // 
  bool ManagedFileProperty::DoSetAttribute( const wxString& name, wxVariant& value )
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
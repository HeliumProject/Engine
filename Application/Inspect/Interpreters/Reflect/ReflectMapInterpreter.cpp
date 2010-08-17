#include "ReflectMapInterpreter.h"
#include "ReflectMapEntryDialog.h"
#include "InspectReflectInit.h"

#include "Application/Inspect/Controls/InspectAction.h"
#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/Controls/InspectGroup.h"
#include "Application/Inspect/Controls/InspectList.h"
#include "Application/Inspect/Controls/InspectPanel.h"
#include "Application/Inspect/InspectData.h"

#include "Foundation/TUID.h"

using namespace Helium;
using namespace Helium::Inspect;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ReflectMapInterpreter::ReflectMapInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates UI for the field variable specified.
// 
void ReflectMapInterpreter::InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent )
{
  if ( field->m_Flags & Reflect::FieldFlags::Hide )
  {
    return;
  }

  // create the panel
  PanelPtr panel = m_Container->GetCanvas()->Create<Panel>( this );
  parent->AddControl( panel );

  tstring temp;
  bool converted = Helium::ConvertString( field->m_UIName, temp );
  HELIUM_ASSERT( converted );

  panel->SetText( temp );

  // create the serializers
  std::vector< Reflect::Element* >::const_iterator itr = instances.begin();
  std::vector< Reflect::Element* >::const_iterator end = instances.end();
  for ( ; itr != end; ++itr )
  {
    Reflect::SerializerPtr ser = Reflect::AssertCast< Reflect::Serializer >( Reflect::Registry::GetInstance()->CreateInstance( field->m_SerializerID ) );
    uintptr fieldAddress = ( uintptr )( *itr ) + field->m_Offset;

    ser->ConnectData( ( void* )fieldAddress );

    m_Serializers.push_back( ser );
  }

  // create the list
  ListPtr list = parent->GetCanvas()->Create<List>( this );
  panel->AddControl( list );
  list->SetMap( true );

  // bind the data to the controls
  list->Bind( new MultiStringFormatter< Reflect::Serializer >( (std::vector<Reflect::Serializer*>&)m_Serializers ) );

  // create the buttons if we are not read only
  if ( !(field->m_Flags & Reflect::FieldFlags::ReadOnly) )
  {
    ContainerPtr buttonContainer = parent->GetCanvas()->Create<Container>( this );
    panel->AddControl( buttonContainer );

    ActionPtr buttonAdd = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonAdd );
    buttonAdd->SetText( TXT( "Add" ) );
    buttonAdd->AddListener( ActionSignature::Delegate ( this, &ReflectMapInterpreter::OnAdd ) );
    buttonAdd->SetInterpreterClientData( new ClientData( list ) );

    ActionPtr buttonRemove = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonRemove );
    buttonRemove->SetText( TXT( "Remove" ) );
    buttonRemove->AddListener( ActionSignature::Delegate ( this, &ReflectMapInterpreter::OnRemove ) );
    buttonRemove->SetInterpreterClientData( new ClientData( list ) );

    ActionPtr buttonEdit = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonEdit );
    buttonEdit->SetText( TXT( "Edit" ) );
    buttonEdit->AddListener( ActionSignature::Delegate ( this, &ReflectMapInterpreter::OnEdit ) );
    buttonEdit->SetInterpreterClientData( new ClientData( list ) );
  }

  // for now let's just disable this panel if there is more than one item selected. I'm not sure if it will behave properly in this case.
  if ( instances.size() > 1 )
  {
    panel->SetEnabled( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
// Runs the dialog for editing a key-value pair.  Returns a string that is the
// new item formed by concatenating and delimiting the key and value.
// 
tstring RunDialog( wxWindow* parent, const tstring& initialKey = TXT( "" ), const tstring& initialVal = TXT( "" ) )
{
  tstring result;
  ReflectMapEntryDialog dlg( parent, wxID_ANY, TXT( "" ) );
  dlg.m_Key->SetValue( initialKey );
  dlg.m_Value->SetValue( initialVal );
  if ( dlg.ShowModal() == wxID_OK && !dlg.m_Key->GetValue().empty() && !dlg.m_Value->GetValue().empty() )
  {
    tstring key( dlg.m_Key->GetValue() );
    tstring value( dlg.m_Value->GetValue() );
    result = key + List::s_MapKeyValDelim + value;
  }
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the add button is pressed.  Displays a dialog that lets
// you enter a new key-value pair.  If you enter a key that already exists in
// the list, you will be asked if you want to replace it or not.
// 
void ReflectMapInterpreter::OnAdd( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetInterpreterClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );

    tstring result = RunDialog( button->GetCanvas()->GetControl() );

    if ( !result.empty() )
    {
      bool warn = false;
      const tstring editedKey = result.substr( 0, result.find_first_of( List::s_MapKeyValDelim ) );
      std::vector< tstring > items;
      items.push_back( result );

      std::vector< tstring >::const_iterator itr = list->GetItems().begin();
      std::vector< tstring >::const_iterator end = list->GetItems().end();
      for ( ; itr != end; ++itr )
      {
        const tstring& item ( *itr );

        const tstring key = item.substr( 0, item.find_first_of( List::s_MapKeyValDelim ) );
        if ( key == editedKey )
        {
          warn = true;
        }
        else
        {
          items.push_back( item );
        }
      }

      bool cancel = warn;
      if ( warn )
      {
        tstring msg = tstring( TXT( "There is already an item with key '" ) ) + editedKey + TXT( "'.  Would you like to replace this value?" );
        cancel = ( wxMessageBox( msg.c_str(), TXT( "Replace?" ), wxCENTER | wxICON_QUESTION | wxYES_NO ) != wxYES );
      }

      if ( !cancel )
      {
        list->AddItems( items );
        button->GetCanvas()->Read();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the remove button is pressed.  If there are any items 
// selected in the list control, they will be removed from the list.
// 
void ReflectMapInterpreter::OnRemove( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetInterpreterClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const std::vector< tstring >& selectedItems = list->GetSelectedItems();
    if ( selectedItems.size() > 0 )
    {
      std::vector< tstring >::const_iterator selBegin = selectedItems.begin();
      std::vector< tstring >::const_iterator selEnd = selectedItems.end();

      std::vector< tstring > items;
      std::vector< tstring >::const_iterator itr = list->GetItems().begin();
      std::vector< tstring >::const_iterator end = list->GetItems().end();
      for ( ; itr != end; ++itr )
      {
        const tstring& item ( *itr );
        std::vector< tstring >::const_iterator found = std::find( selBegin, selEnd, item );
        if ( found == selEnd )
        {
          items.push_back( item );
        }
      }

      list->AddItems( items );
      button->GetCanvas()->Read();
    }
    else
    {
      // Select an item to delete
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the Edit button is pressed.  If there is an item selected
// in the list control, a dialog will be shown that allows you to change the key
// and value pair for that item.
// 
void ReflectMapInterpreter::OnEdit( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetInterpreterClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
  {
    ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const std::vector< tstring >& selected = list->GetSelectedItems();
    if ( selected.size() == 1 )
    {
      tstring key;
      tstring val;
      tstring::size_type pos = selected[0].find_first_of( List::s_MapKeyValDelim );
      if ( pos != tstring::npos )
      {
        key = selected[0].substr( 0, pos );
        val = selected[0].substr( pos + _tcslen( List::s_MapKeyValDelim ) );
      }
      tstring result = RunDialog( button->GetCanvas()->GetControl(), key, val );

      if ( !result.empty() )
      {
        std::vector< tstring > newList;

        std::vector< tstring >::const_iterator itr = list->GetItems().begin();
        std::vector< tstring >::const_iterator end = list->GetItems().end();
        for ( ; itr != end; ++itr )
        {
          const tstring& item ( *itr );

          if ( item == selected[0] )
          {
            newList.push_back( result );
          }
          else
          {
            newList.push_back( item );
          }
        }

        list->AddItems( newList );
        button->GetCanvas()->Read();
      }
    }
    else
    {
      // Only select one item
    }
  }
}

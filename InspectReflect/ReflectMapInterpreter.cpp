#include "stdafx.h"
#include "ReflectMapInterpreter.h"
#include "ReflectMapEntryDialog.h"
#include "InspectReflectInit.h"

#include "Inspect/Action.h"
#include "Inspect/Canvas.h"
#include "Inspect/Group.h"
#include "Inspect/List.h"
#include "Inspect/Panel.h"
#include "Inspect/StringData.h"

#include "TUID/TUID.h"

using namespace Inspect;

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

  bool isFileIdKeyValue = ( field->m_SerializerID == Reflect::GetType< Reflect::U64U64MapSerializer >() ) && ( field->m_Flags & Reflect::FieldFlags::FileID ) != 0;

  // create the panel
  PanelPtr panel = m_Container->GetCanvas()->Create<Panel>( this );
  parent->AddControl( panel );
  panel->SetText( field->m_UIName );

  // create the serializers
  std::vector< Reflect::Element* >::const_iterator itr = instances.begin();
  std::vector< Reflect::Element* >::const_iterator end = instances.end();
  for ( ; itr != end; ++itr )
  {
    Reflect::SerializerPtr ser = Reflect::AssertCast< Reflect::Serializer >( Reflect::Registry::GetInstance()->CreateInstance( field->m_SerializerID ) );
    PointerSizedUInt fieldAddress = ( PointerSizedUInt )( *itr ) + field->m_Offset;

    if ( isFileIdKeyValue )
    {
      ser->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &ReflectMapInterpreter::TranslateInputKeyValueTUID ) );
      ser->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &ReflectMapInterpreter::TranslateOutputKeyValueTUID ) );
    }

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
    buttonAdd->SetText( "Add" );
    buttonAdd->AddListener( ActionSignature::Delegate ( this, &ReflectMapInterpreter::OnAdd ) );
    buttonAdd->SetClientData( new ClientDataControl( list ) );

    ActionPtr buttonRemove = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonRemove );
    buttonRemove->SetText( "Remove" );
    buttonRemove->AddListener( ActionSignature::Delegate ( this, &ReflectMapInterpreter::OnRemove ) );
    buttonRemove->SetClientData( new ClientDataControl( list ) );

    ActionPtr buttonEdit = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonEdit );
    buttonEdit->SetText( "Edit" );
    buttonEdit->AddListener( ActionSignature::Delegate ( this, &ReflectMapInterpreter::OnEdit ) );
    buttonEdit->SetClientData( new ClientDataControl( list ) );
  }

  // for now let's just disable this panel if there is more than one item selected. I'm not sure if it will behave properly in this case.
  if ( instances.size() > 1 )
  {
    panel->SetEnabled( false );
  }
}

void ReflectMapInterpreter::TranslateInputKeyValueTUID( Reflect::TranslateInputEventArgs& args )
{
  if ( args.m_Serializer->GetType() == Reflect::GetType< Reflect::U64U64MapSerializer >() )
  {
    Reflect::U64U64MapSerializer* ser = Reflect::DangerousCast< Reflect::U64U64MapSerializer >( args.m_Serializer );

    std::streamsize size = args.m_Stream.rdbuf()->in_avail();
    if ( size > 0 )
    {
      std::string data;
      data.resize( size );
      args.m_Stream.read( const_cast< char* >( data.c_str() ), size );

      size_t delimLength = strlen( Reflect::s_ContainerItemDelimiter );
      std::string::size_type bgn = 0;
      std::string::size_type mid = data.find( Reflect::s_ContainerItemDelimiter, bgn );
      std::string::size_type end = data.find( Reflect::s_ContainerItemDelimiter, mid + 1 );
      while ( bgn != std::string::npos && mid != std::string::npos )
      {
        std::string key = data.substr( bgn, mid - bgn );
        std::string value = end != std::string::npos ? data.substr( mid + 1, end - mid - delimLength ) : data.substr( mid + 1 );
        tuid tuidKey = TUID::Null;
        tuid tuidValue = TUID::Null;
        if ( TUID::Parse( key, tuidKey ) && TUID::Parse( value, tuidValue ) )
        {
          ser->m_Data->insert( Reflect::SimpleMapSerializer< u64, Reflect::U64Serializer, u64, Reflect::U64Serializer >::DataType::value_type( tuidKey, tuidValue ) );
        }
        else
        {
          NOC_BREAK();
        }

        if ( end != std::string::npos )
        {
          bgn = end + 1;
          mid = data.find( Reflect::s_ContainerItemDelimiter, bgn );
          end = data.find( Reflect::s_ContainerItemDelimiter, mid + 1 );
        }
        else
        {
          bgn = end;
          mid = end;
        }
      }
    }
  }
  else
  {
    NOC_BREAK();
  }
}

void ReflectMapInterpreter::TranslateOutputKeyValueTUID( Reflect::TranslateOutputEventArgs& args )
{
  if ( args.m_Serializer->GetType() == Reflect::GetType< Reflect::U64U64MapSerializer >() )
  {
    Reflect::U64U64MapSerializer* ser = Reflect::DangerousCast< Reflect::U64U64MapSerializer > ( args.m_Serializer );

    bool first = true;
    M_tuid::const_iterator itr = ser->m_Data->begin();
    M_tuid::const_iterator end = ser->m_Data->end();
    for ( ; itr != end; ++itr )
    {
      const u64& key = itr->first;
      const u64& val = itr->second;

      if ( !first )
      {
        args.m_Stream << Reflect::s_ContainerItemDelimiter;
      }
      args.m_Stream << TUID::HexFormat << key;
      args.m_Stream << Reflect::s_ContainerItemDelimiter;
      args.m_Stream << TUID::HexFormat << val;
     
      first = false;
    }
  }
  else
  {
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Runs the dialog for editing a key-value pair.  Returns a string that is the
// new item formed by concatenating and delimiting the key and value.
// 
std::string RunDialog( wxWindow* parent, const std::string& initialKey = "", const std::string& initialVal = "" )
{
  std::string result;
  ReflectMapEntryDialog dlg( parent, wxID_ANY, "" );
  dlg.m_Key->SetValue( initialKey );
  dlg.m_Value->SetValue( initialVal );
  if ( dlg.ShowModal() == wxID_OK && !dlg.m_Key->GetValue().empty() && !dlg.m_Value->GetValue().empty() )
  {
    std::string key( dlg.m_Key->GetValue() );
    std::string value( dlg.m_Value->GetValue() );
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
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );

    std::string result = RunDialog( button->GetCanvas()->GetControl() );

    if ( !result.empty() )
    {
      bool warn = false;
      const std::string editedKey = result.substr( 0, result.find_first_of( List::s_MapKeyValDelim ) );
      V_string items;
      items.push_back( result );

      V_string::const_iterator itr = list->GetItems().begin();
      V_string::const_iterator end = list->GetItems().end();
      for ( ; itr != end; ++itr )
      {
        const std::string& item ( *itr );

        const std::string key = item.substr( 0, item.find_first_of( List::s_MapKeyValDelim ) );
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
        std::string msg = "There is already an item with key '" + editedKey + "'.  Would you like to replace this value?";
        cancel = ( wxMessageBox( msg.c_str(), "Replace?", wxCENTER | wxICON_QUESTION | wxYES_NO ) != wxYES );
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
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const V_string& selectedItems = list->GetSelectedItems();
    if ( selectedItems.size() > 0 )
    {
      V_string::const_iterator selBegin = selectedItems.begin();
      V_string::const_iterator selEnd = selectedItems.end();

      V_string items;
      V_string::const_iterator itr = list->GetItems().begin();
      V_string::const_iterator end = list->GetItems().end();
      for ( ; itr != end; ++itr )
      {
        const std::string& item ( *itr );
        V_string::const_iterator found = std::find( selBegin, selEnd, item );
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
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const V_string& selected = list->GetSelectedItems();
    if ( selected.size() == 1 )
    {
      std::string key;
      std::string val;
      std::string::size_type pos = selected[0].find_first_of( List::s_MapKeyValDelim );
      if ( pos != std::string::npos )
      {
        key = selected[0].substr( 0, pos );
        val = selected[0].substr( pos + strlen( List::s_MapKeyValDelim ) );
      }
      std::string result = RunDialog( button->GetCanvas()->GetControl(), key, val );

      if ( !result.empty() )
      {
        V_string newList;

        V_string::const_iterator itr = list->GetItems().begin();
        V_string::const_iterator end = list->GetItems().end();
        for ( ; itr != end; ++itr )
        {
          const std::string& item ( *itr );

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

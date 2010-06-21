#include "ReflectSetInterpreter.h"
#include "InspectReflectInit.h"

#include "Application/Inspect/Widgets/Button Controls/Action.h"
#include "Application/Inspect/Widgets/Canvas.h"
#include "Application/Inspect/Widgets/Group.h"
#include "Application/Inspect/Widgets/Other Controls/List.h"
#include "Application/Inspect/Widgets/InspectPanel.h"
#include "Application/Inspect/Data/StringData.h"

using namespace Inspect;


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ReflectSetInterpreter::ReflectSetInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates UI for the field variable specified.
// 
void ReflectSetInterpreter::InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent )
{
  if ( field->m_Flags & Reflect::FieldFlags::Hide )
  {
    return;
  }

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
    uintptr fieldAddress = ( uintptr )( *itr ) + field->m_Offset;
    ser->ConnectData( ( void* )fieldAddress );
    m_Serializers.push_back( ser );
  }

  // create the list
  ListPtr list = parent->GetCanvas()->Create<List>( this );
  panel->AddControl( list );

  // bind the ui to the serialiers
  list->Bind( new MultiStringFormatter< Reflect::Serializer >( (std::vector<Reflect::Serializer*>&)m_Serializers ) );

  // create the buttons if we are not read only
  if ( !( field->m_Flags & Reflect::FieldFlags::ReadOnly ) )
  {
    ContainerPtr buttonContainer = parent->GetCanvas()->Create<Container>( this );
    panel->AddControl( buttonContainer );

    ActionPtr buttonAdd = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonAdd );
    buttonAdd->SetText( "Add" );
    buttonAdd->AddListener( ActionSignature::Delegate ( this, &ReflectSetInterpreter::OnAdd ) );
    buttonAdd->SetClientData( new ClientDataControl( list ) );

    ActionPtr buttonRemove = parent->GetCanvas()->Create<Action>( this );
    buttonContainer->AddControl( buttonRemove );
    buttonRemove->SetText( "Remove" );
    buttonRemove->AddListener( ActionSignature::Delegate ( this, &ReflectSetInterpreter::OnRemove ) );
    buttonRemove->SetClientData( new ClientDataControl( list ) );
  }

  // for now let's just disable this panel if there is more than one item selected. I'm not sure if it will behave properly in this case.
  if ( instances.size() > 1 )
  {
    panel->SetEnabled( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the add button is pressed.  Displays a dialog that lets
// you enter a new key-value pair.  If you enter a key that already exists in
// the list, you will be asked if you want to replace it or not.
// 
void ReflectSetInterpreter::OnAdd( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    wxTextEntryDialog dlg( m_Container->GetCanvas()->GetControl(), "", "Add" );
    if ( dlg.ShowModal() == wxID_OK )
    {
      std::string input = dlg.GetValue().c_str();
      if ( !input.empty() )
      {
        List* list = static_cast< List* >( data->m_Control );

        V_string items = list->GetItems();
        items.push_back( input );

        std::sort( items.begin(), items.end() );
        std::unique( items.begin(), items.end() );

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
void ReflectSetInterpreter::OnRemove( Button* button )
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

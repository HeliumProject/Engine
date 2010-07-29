#include "ReflectArrayInterpreter.h"
#include "InspectReflectInit.h"

#include "Application/Inspect/Controls/Label.h"
#include "Application/Inspect/Controls/Value.h"
#include "Application/Inspect/Controls/Choice.h"
#include "Application/Inspect/Controls/List.h"
#include "Application/Inspect/Controls/Action.h"
#include "Application/Inspect/Data/StringData.h"
#include "Application/Inspect/InspectInit.h"

#include "Foundation/String/Tokenize.h"

using namespace Reflect;
using namespace Inspect;

ReflectArrayInterpreter::ReflectArrayInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
{

}

void ReflectArrayInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  if (field->m_Flags & FieldFlags::Hide)
  {
    return;
  }

  // create the label
  ContainerPtr labelContainer = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl( labelContainer );
  LabelPtr label = labelContainer->GetCanvas()->Create<Label>(this);
  labelContainer->AddControl( label );
  tstring temp;
  bool converted = Platform::ConvertString( field->m_UIName, temp );
  HELIUM_ASSERT( converted );
  label->SetText( temp );

  // create the list view
  ContainerPtr listContainer = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl( listContainer );
  ListPtr list = m_Container->GetCanvas()->Create<List>(this);
  listContainer->AddControl( list );

  // create the buttons
  ActionPtr addButton;
  ActionPtr removeButton;
  ActionPtr upButton;
  ActionPtr downButton;
  if ( !(field->m_Flags & FieldFlags::ReadOnly) )
  {
    addButton = AddAddButton( list );
    removeButton = AddRemoveButton( list );
    upButton = AddMoveUpButton( list );
    downButton = AddMoveDownButton( list );
  }

  // add the buttons to the panel
  ContainerPtr buttonGroup = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl( buttonGroup );
  if ( addButton )
  {
    buttonGroup->AddControl( addButton );
  }
  if ( removeButton )
  {
    buttonGroup->AddControl( removeButton );
  }
  if ( upButton )
  {
    buttonGroup->AddControl( upButton );
  }
  if ( downButton )
  {
    buttonGroup->AddControl( downButton );
  }

  // create the serializers
  std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
  std::vector<Reflect::Element*>::const_iterator end = instances.end();
  for ( ; itr != end; ++itr )
  {
    SerializerPtr s = field->CreateSerializer();

    OnCreateFieldSerializer( s );

    s->ConnectField(*itr, field);

    m_Serializers.push_back(s);
  }

  // bind the ui to the serializers
  Helium::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Reflect::Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers );
  list->Bind( data );

  // setup the default value
  if (field->m_Default != NULL)
  {
    tstringstream outStream;
    *field->m_Default >> outStream;
    list->SetDefault( outStream.str() );
  }
}

ActionPtr ReflectArrayInterpreter::AddAddButton( List* list )
{
  ActionPtr addButton = m_Container->GetCanvas()->Create<Action>(this);
  addButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnAdd ) );
  addButton->SetClientData( new ClientDataControl( list ) );
  addButton->SetText( TXT( "Add" ) );

  return addButton;
}

ActionPtr ReflectArrayInterpreter::AddRemoveButton( List* list )
{
  ActionPtr removeButton = m_Container->GetCanvas()->Create<Action>(this);
  removeButton->SetText( TXT( "Remove" ) );
  removeButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnRemove ) );
  removeButton->SetClientData( new ClientDataControl( list ) );
  
  return removeButton;
}

ActionPtr ReflectArrayInterpreter::AddMoveUpButton( List* list )
{
  ActionPtr upButton = m_Container->GetCanvas()->Create<Action>(this);
  upButton->SetIcon( TXT( "actions/go-up" ) );
  upButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnMoveUp ) );
  upButton->SetClientData( new ClientDataControl( list ) );
  
  return upButton;
}

ActionPtr ReflectArrayInterpreter::AddMoveDownButton( List* list )
{
  ActionPtr downButton = m_Container->GetCanvas()->Create<Action>(this);
  downButton->SetIcon( TXT( "actions/go-down" ) );
  downButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnMoveDown ) );
  downButton->SetClientData( new ClientDataControl( list ) );
  
  return downButton;
}

void ReflectArrayInterpreter::OnAdd( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    wxTextEntryDialog dlg( button->GetCanvas()->GetControl(), TXT( "" ), TXT( "Add" ) );
    if ( dlg.ShowModal() == wxID_OK )
    {
      tstring input = dlg.GetValue().c_str();
      if ( !input.empty() )
      {
        List* list = static_cast< List* >( data->m_Control );
        list->AddItem( input );
      }
    }

    button->GetCanvas()->Read();
  }
}

void ReflectArrayInterpreter::OnRemove( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    const std::vector< tstring >& selectedItems = list->GetSelectedItems();
    if ( !selectedItems.empty() )
    {
      std::vector< tstring >::const_iterator itr = selectedItems.begin();
      std::vector< tstring >::const_iterator end = selectedItems.end();
      list->Freeze();
      for ( ; itr != end; ++itr )
      {
        const tstring& selection = *itr;
        list->RemoveItem( selection );
      }
      list->Thaw();
    }
  }
}

void ReflectArrayInterpreter::OnMoveUp( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    list->MoveSelectedItems( Inspect::MoveDirections::Up );
  }
}

void ReflectArrayInterpreter::OnMoveDown( Button* button )
{
  Reflect::ObjectPtr clientData = button->GetClientData();
  if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataControl>() ) )
  {
    ClientDataControl* data = static_cast< ClientDataControl* >( clientData.Ptr() );
    List* list = static_cast< List* >( data->m_Control );
    list->MoveSelectedItems( Inspect::MoveDirections::Down );
  }
}
#include "stdafx.h"
#include "ReflectArrayInterpreter.h"
#include "InspectReflectInit.h"

#include "Inspect/Label.h"
#include "Inspect/Value.h"
#include "Inspect/Choice.h"
#include "Inspect/List.h"
#include "Inspect/Action.h"
#include "Inspect/StringData.h"
#include "Inspect/InspectInit.h"

#include "Common/String/Tokenize.h"

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
  label->SetText( field->m_UIName );

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
  Nocturnal::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Reflect::Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers );
  list->Bind( data );

  // setup the default value
  if (field->m_Default != NULL)
  {
    std::stringstream outStream;
    *field->m_Default >> outStream;
    list->SetDefault(outStream.str());
  }
}

ActionPtr ReflectArrayInterpreter::AddAddButton( List* list )
{
  ActionPtr addButton = m_Container->GetCanvas()->Create<Action>(this);
  addButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnAdd ) );
  addButton->SetClientData( new ClientDataControl( list ) );
  addButton->SetText( "Add" );

  return addButton;
}

ActionPtr ReflectArrayInterpreter::AddRemoveButton( List* list )
{
  ActionPtr removeButton = m_Container->GetCanvas()->Create<Action>(this);
  removeButton->SetText( "Remove" );
  removeButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnRemove ) );
  removeButton->SetClientData( new ClientDataControl( list ) );
  
  return removeButton;
}

ActionPtr ReflectArrayInterpreter::AddMoveUpButton( List* list )
{
  ActionPtr upButton = m_Container->GetCanvas()->Create<Action>(this);
  upButton->SetIcon( "arrow_up_16.png" );
  upButton->AddListener( ActionSignature::Delegate ( &ReflectArrayInterpreter::OnMoveUp ) );
  upButton->SetClientData( new ClientDataControl( list ) );
  
  return upButton;
}

ActionPtr ReflectArrayInterpreter::AddMoveDownButton( List* list )
{
  ActionPtr downButton = m_Container->GetCanvas()->Create<Action>(this);
  downButton->SetIcon( "arrow_down_16.png" );
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
    wxTextEntryDialog dlg( button->GetCanvas()->GetControl(), "", "Add" );
    if ( dlg.ShowModal() == wxID_OK )
    {
      std::string input = dlg.GetValue().c_str();
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
    const V_string& selectedItems = list->GetSelectedItems();
    if ( !selectedItems.empty() )
    {
      V_string::const_iterator itr = selectedItems.begin();
      V_string::const_iterator end = selectedItems.end();
      list->Freeze();
      for ( ; itr != end; ++itr )
      {
        const std::string& selection = *itr;
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
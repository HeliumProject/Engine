#include "stdafx.h"
#include "ReflectValueInterpreter.h"

#include "Inspect/InspectInit.h"
#include "Inspect/Script.h"
#include "Inspect/CheckBox.h"
#include "Inspect/Choice.h"
#include "Inspect/Action.h"
#include "Inspect/StringData.h"
#include "Inspect/Value.h"
#include "Inspect/List.h"

// Using
using namespace Reflect;
using namespace Inspect;

ReflectValueInterpreter::ReflectValueInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectValueInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  if (field->m_Flags & FieldFlags::Hide)
  {
    return;
  }

  //
  // Create the ui we are generating
  //

  ContainerPtr group = m_Container->GetCanvas()->Create<Container>(this);

  bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;

  //
  // Parse
  //

  std::string fieldUI;
  field->GetProperty( "UIScript", fieldUI );
  bool result = Script::Parse(fieldUI, this, parent->GetCanvas(), group, field->m_Flags);

  if (!result)
  {
    if ( field->m_SerializerID == Reflect::GetType<EnumerationSerializer>() )
    {
      ChoicePtr choice = m_Container->GetCanvas()->Create<Choice>(this);

      const EnumerationField* enumInfo = static_cast<const EnumerationField*>(field);

      V_Item items;
      items.resize( enumInfo->m_Enumeration->m_Elements.size() );

      V_EnumerationElement::const_iterator itr = enumInfo->m_Enumeration->m_Elements.begin();
      V_EnumerationElement::const_iterator end = enumInfo->m_Enumeration->m_Elements.end();
      for ( size_t index=0; itr != end; ++itr, ++index )
      {
        Item& item = items[index];
        item.m_Key = (*itr)->m_Label.c_str();
        item.m_Data = (*itr)->m_Label.c_str();
      }

      choice->SetItems( items );
      choice->SetDropDown( true );
      choice->SetReadOnly( readOnly );

      group->AddControl(choice);
    }
    else
    {
      if ( field->m_SerializerID == Reflect::GetType<BoolSerializer>() )
      {
        CheckBoxPtr checkBox = m_Container->GetCanvas()->Create<CheckBox>(this);
        checkBox->SetReadOnly( readOnly );
        group->AddControl( checkBox );
      }
      else
      {
        ValuePtr value = m_Container->GetCanvas()->Create<Value>( this );
        value->SetReadOnly( readOnly );
        group->AddControl( value );
      }
    }
  }

  //
  // Setup label
  //

  LabelPtr label = NULL;

  {
    V_Control::const_iterator itr = group->GetControls().begin();
    V_Control::const_iterator end = group->GetControls().end();
    for( ; itr != end; ++itr )
    {
      Label* label = Reflect::ObjectCast<Label>( *itr );
      if (label)
      {
        break;
      }
    }
  }

  if (label == NULL)
  {
    label = group->GetCanvas()->Create<Label>(this);
    label->SetText( field->m_UIName );

    group->InsertControl(0, label);
  }

  //
  // Bind data
  //

  std::vector<Serializer*> ser;

  {
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
      SerializerPtr s = field->CreateSerializer();

      if (!s->HasType(Reflect::GetType<ContainerSerializer>()))
      {
        s->ConnectField(*itr, field);

        ser.push_back(s);

        m_Serializers.push_back(s);
      }
    }
  }

  Nocturnal::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( ser );

  group->Bind( data );

  //
  // Set default
  //

  if (field->m_Default != NULL)
  {
    std::stringstream outStream;

    *field->m_Default >> outStream;

    group->SetDefault(outStream.str());
  }

  //
  // Close
  //

  parent->AddControl(group);
}
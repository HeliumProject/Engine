#include "ReflectValueInterpreter.h"

#include "Application/Inspect/InspectInit.h"
#include "Application/Inspect/Script.h"
#include "Application/Inspect/Controls/CheckBox.h"
#include "Application/Inspect/Controls/Choice.h"
#include "Application/Inspect/Controls/Action.h"
#include "Application/Inspect/Data/StringData.h"
#include "Application/Inspect/Controls/Value.h"
#include "Application/Inspect/Controls/List.h"

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

  tstring fieldUI;
  field->GetProperty( TXT( "UIScript" ), fieldUI );
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
        bool converted = Platform::ConvertString( (*itr)->m_Label.c_str(), item.m_Key );
        NOC_ASSERT( converted );

        converted = Platform::ConvertString( (*itr)->m_Label.c_str(), item.m_Data );
        NOC_ASSERT( converted );
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

    tstring temp;
    bool converted = Platform::ConvertString( field->m_UIName, temp );
    NOC_ASSERT( converted );

   label->SetText( temp );

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
    tstringstream outStream;

    *field->m_Default >> outStream;

    group->SetDefault( outStream.str() );
  }

  //
  // Close
  //

  parent->AddControl(group);
}
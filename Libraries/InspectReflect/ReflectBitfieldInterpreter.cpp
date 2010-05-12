#include "stdafx.h"
#include "ReflectBitfieldInterpreter.h"
#include "ReflectBitfieldCheckBox.h"

#include "Inspect/Label.h"
#include "Inspect/Panel.h"
#include "Inspect/StringData.h"

#include "Reflect/Field.h"
#include "Reflect/Enumeration.h"

using namespace Reflect;
using namespace Inspect;

ReflectBitfieldInterpreter::ReflectBitfieldInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectBitfieldInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  // If you hit this, you are misusing this interpreter
  NOC_ASSERT( field->m_SerializerID == Reflect::GetType<Reflect::BitfieldSerializer>() );

  if ( field->m_SerializerID != Reflect::GetType<Reflect::BitfieldSerializer>() )
  {
    return;
  }

  if ( field->m_Flags & FieldFlags::Hide )
  {
    return;
  }

  const EnumerationField* enumField = static_cast< const EnumerationField* >( field );

  // create the panel
  PanelPtr panel = m_Container->GetCanvas()->Create<Panel>(this);
  panel->SetText( field->m_UIName );
  panel->SetExpanded( true );
  parent->AddControl(panel);

  // build the child gui elements
  bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;
  M_StrEnumerationElement::const_iterator enumItr = enumField->m_Enumeration->m_ElementsByLabel.begin();
  M_StrEnumerationElement::const_iterator enumEnd = enumField->m_Enumeration->m_ElementsByLabel.end();
  for ( ; enumItr != enumEnd; ++enumItr )
  {
    ContainerPtr row = m_Container->GetCanvas()->Create<Container>( this );
    panel->AddControl( row );

    LabelPtr label = m_Container->GetCanvas()->Create<Label>( this );
    row->AddControl( label );
    label->SetText( enumItr->first );

    BitfieldCheckBoxPtr checkbox = m_Container->GetCanvas()->Create<ReflectBitfieldCheckBox>( this );
    row->AddControl( checkbox );
    checkbox->SetBitfieldString( enumItr->first );
    checkbox->SetReadOnly( readOnly );
  }

  // create the serializers
  std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
  std::vector<Reflect::Element*>::const_iterator end = instances.end();
  for ( ; itr != end; ++itr )
  {
    SerializerPtr s = field->CreateSerializer();
    s->ConnectField(*itr, field);
    m_Serializers.push_back(s);
  }

  // bind the ui to the serializers
  panel->Bind( new MultiStringFormatter<Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers ) );

  // setup the default value
  if (field->m_Default != NULL)
  {
    std::stringstream outStream;
    *field->m_Default >> outStream;
    panel->SetDefault(outStream.str());
  }
}

#include "ReflectBitfieldInterpreter.h"
#include "ReflectBitfieldCheckBox.h"

#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectPanel.h"
#include "Application/Inspect/Data/StringData.h"

#include "Foundation/Reflect/Field.h"
#include "Foundation/Reflect/Enumeration.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectBitfieldInterpreter::ReflectBitfieldInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectBitfieldInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  // If you hit this, you are misusing this interpreter
  HELIUM_ASSERT( field->m_SerializerID == Reflect::GetType<Reflect::BitfieldSerializer>() );

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
  
  tstring temp;
  bool converted = Helium::ConvertString( field->m_UIName, temp );
  panel->SetText( temp );

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

    tstring temp;
    bool converted = Helium::ConvertString( enumItr->first, temp );
    HELIUM_ASSERT( converted );
    label->SetText( temp );

    BitfieldCheckBoxPtr checkbox = m_Container->GetCanvas()->Create<ReflectBitfieldCheckBox>( this );
    row->AddControl( checkbox );

    converted = Helium::ConvertString( enumItr->first, temp );
    HELIUM_ASSERT( converted );
    checkbox->SetBitfieldString( temp );
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
    tstringstream outStream;
    *field->m_Default >> outStream;
    panel->SetDefault( outStream.str() );
  }
}

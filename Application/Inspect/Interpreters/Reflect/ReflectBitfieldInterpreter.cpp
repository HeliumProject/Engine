#include "ReflectBitfieldInterpreter.h"
#include "ReflectBitfieldCheckBox.h"

#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/InspectData.h"

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

  // create the container
  ContainerPtr container = new Container ();
  
  tstring temp;
  bool converted = Helium::ConvertString( field->m_UIName, temp );
  container->a_Name.Set( temp );
  parent->AddChild(container);

  // build the child gui elements
  bool readOnly = ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly;
  M_StrEnumerationElement::const_iterator enumItr = enumField->m_Enumeration->m_ElementsByLabel.begin();
  M_StrEnumerationElement::const_iterator enumEnd = enumField->m_Enumeration->m_ElementsByLabel.end();
  for ( ; enumItr != enumEnd; ++enumItr )
  {
    ContainerPtr row = CreateControl< Container >();
    container->AddChild( row );

    LabelPtr label = CreateControl< Label >();
    row->AddChild( label );

    tstring temp;
    bool converted = Helium::ConvertString( enumItr->first, temp );
    HELIUM_ASSERT( converted );
    label->BindText( temp );

    BitfieldCheckBoxPtr checkbox = CreateControl< ReflectBitfieldCheckBox >();
    row->AddChild( checkbox );

    converted = Helium::ConvertString( enumItr->first, temp );
    HELIUM_ASSERT( converted );
    checkbox->SetBitfieldString( temp );
    checkbox->a_IsReadOnly.Set( readOnly );
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
  container->Bind( new MultiStringFormatter<Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers ) );

  // setup the default value
  if (field->m_Default != NULL)
  {
    tstringstream outStream;
    *field->m_Default >> outStream;
    container->a_Default.Set( outStream.str() );
  }
}

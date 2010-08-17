#include "ReflectVectorInterpreter.h"

#include "Application/Inspect/Controls/InspectLabel.h"
#include "Application/Inspect/Controls/InspectValue.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/InspectData.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

ReflectVectorInterpreter::ReflectVectorInterpreter (Container* container)
: ReflectFieldInterpreter (container)
{

}

void ReflectVectorInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
  if ( field->m_Flags & FieldFlags::Hide )
  {
    return;
  }

  // create the container
  ContainerPtr container = m_Container->GetCanvas()->Create<Container>(this);
  parent->AddControl(container);

  // create the label
  LabelPtr label = parent->GetCanvas()->Create<Label>( this );
  container->AddControl( label );

  tstring temp;
  bool converted = Helium::ConvertString( field->m_UIName, temp );
  HELIUM_ASSERT( converted );

  label->SetText( temp );

  // compute dimensions
  int dimensions = 2;
  if ( field->m_SerializerID == Reflect::GetType<Vector3Serializer>() )
  {
    dimensions += 1;
  }
  if ( field->m_SerializerID == Reflect::GetType<Vector4Serializer>() )
  {
    dimensions += 2;
  }

  // create the dimension ui
  for ( int offset = 0; offset < dimensions*4; offset += 4 )
  {
    // create the serializers
    std::vector<Reflect::Serializer*> data;
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
      SerializerPtr s = new F32Serializer ();

      s->ConnectField(*itr, field, offset); 

      m_Serializers.push_back(s);

      data.push_back(s);
    }

    // create the text box
    ValuePtr value = parent->GetCanvas()->Create<Value>( this );
    container->AddControl( value );
    value->SetReadOnly( ( field->m_Flags & FieldFlags::ReadOnly ) == FieldFlags::ReadOnly );

    // bind the ui to the serializers
    value->Bind( new MultiStringFormatter<Serializer>( data ) );
  }
}

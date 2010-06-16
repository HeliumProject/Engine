#include "ParametricKeyInterpreter.h"

#include "Application/Inspect/Widgets/InspectPanel.h"
#include "Application/Inspect/Content/ParametricKeyControl.h"
#include "Application/Inspect/Content/ParametricKeyData.h"

#include "Content/ParametricKey.h"

using namespace Inspect;

ParametricKeyInterpreter::ParametricKeyInterpreter( Container* container )
: ReflectFieldInterpreter( container )
{

}

void ParametricKeyInterpreter::InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent )
{
  if ( field->m_Flags & Reflect::FieldFlags::Hide )
  {
    return;
  }

  if ( field->m_SerializerID != Reflect::GetType< Reflect::ElementArraySerializer >() )
  {
    return;
  }

  // Validation is complete, create the UI
  PanelPtr panel = m_Container->GetCanvas()->Create<Panel>( this );
  parent->AddControl( panel );
  panel->SetText( field->m_UIName );

  // get the field field for this data
  const Reflect::M_FieldIDToInfo::const_iterator foundField = field->m_Type->m_FieldIDToInfo.find( field->m_FieldID );
  if ( foundField != field->m_Type->m_FieldIDToInfo.end() )
  {
    field = foundField->second;
  }
  NOC_ASSERT( field );

  // create the serializers
  std::vector< Reflect::Element* >::const_iterator itr = instances.begin();
  std::vector< Reflect::Element* >::const_iterator end = instances.end();
  for ( ; itr != end; ++itr )
  {
    Reflect::ElementArraySerializerPtr ser = Reflect::AssertCast< Reflect::ElementArraySerializer >( Reflect::Registry::GetInstance()->CreateInstance( field->m_SerializerID ) );   
    ser->ConnectField( *itr, field );
    m_Serializers.push_back( ser );
  }

  // create the key control
  ParametricKeyControlPtr keyControl = m_Container->GetCanvas()->Create<ParametricKeyControl>( this );
  panel->AddControl( keyControl );

  // bind the ui to the serializers
  keyControl->Bind( new MultiParametricKeySerializerFormatter( field->m_Create, m_Serializers ) );
}

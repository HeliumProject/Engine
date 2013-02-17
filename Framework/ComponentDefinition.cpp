
#include "FrameworkPch.h"
#include "ComponentDefinition.h"

#include "Framework/FrameworkDataDeduction.h"

HELIUM_IMPLEMENT_OBJECT(Helium::ComponentDefinition, Framework, GameObjectType::FLAG_ABSTRACT);

//////////////////////////////////////////////////////////////////////////
HELIUM_IMPLEMENT_OBJECT(Helium::ColorComponentDefinition, Framework, 0);

void Helium::ColorComponentDefinition::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &ColorComponentDefinition::m_Color, TXT("m_Color") );
    comp.AddField( &ColorComponentDefinition::m_Pointer, TXT("m_Pointer") );
}

OBJECT_DEFINE_COMPONENT(Helium::ColorComponent);

void Helium::ColorComponent::Finalize(const Helium::ColorComponentDefinition *_descriptor)
{
    m_Color = _descriptor->m_Color;

    if (_descriptor->m_Pointer.Get())
    {
        m_Pointer.Set(_descriptor->m_Pointer->GetCreatedComponent());
    }
}
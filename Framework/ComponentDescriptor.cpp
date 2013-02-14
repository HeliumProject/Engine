
#include "FrameworkPch.h"
#include "ComponentDescriptor.h"

#include "Framework/FrameworkDataDeduction.h"

HELIUM_IMPLEMENT_OBJECT(Helium::ComponentDescriptor, Framework, GameObjectType::FLAG_ABSTRACT);
HELIUM_IMPLEMENT_OBJECT(Helium::ComponentDescriptor_ColorComponent, Framework, 0);

void Helium::ComponentDescriptor_ColorComponent::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &ComponentDescriptor_ColorComponent::m_Color, TXT("m_Color") );
    comp.AddField( &ComponentDescriptor_ColorComponent::m_Pointer, TXT("m_Pointer") );
}

OBJECT_DEFINE_COMPONENT(Helium::ColorComponent);

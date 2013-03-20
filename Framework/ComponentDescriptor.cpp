
#include "FrameworkPch.h"
#include "ComponentDescriptor.h"

#include "Framework/FrameworkDataDeduction.h"

HELIUM_IMPLEMENT_OBJECT(Helium::ComponentDescriptor_ColorComponent, Framework, 0);

void Helium::ComponentDescriptor_ColorComponent::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &ComponentDescriptor_ColorComponent::m_Color, TXT("m_Color") );
    comp.AddField( &ComponentDescriptor_ColorComponent::m_Pointer, TXT("m_Pointer") );
}

OBJECT_DEFINE_COMPONENT(Helium::ColorComponent);

void Helium::ColorComponent::FinalizeComponent(const Helium::ComponentDescriptor *_descriptor)
{
    const ComponentDescriptor_ColorComponent *d = Reflect::AssertCast<ComponentDescriptor_ColorComponent>(_descriptor);
    m_Color = d->m_Color;

    if (d->m_Pointer.Get())
    {
        m_Pointer.AssignComponent(d->m_Pointer->GetCreatedComponent());
    }
}

Helium::Component * Helium::ComponentDescriptor_ColorComponent::CreateComponentInternal( Helium::Components::ComponentSet &_target ) const
{
    return Helium::Components::Allocate<Helium::ColorComponent>(_target);
}

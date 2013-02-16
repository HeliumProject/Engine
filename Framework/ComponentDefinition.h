
#pragma once
#ifndef HELIUM_FRAMEWORK_COMPONENT_DESCRIPTOR_H
#define HELIUM_FRAMEWORK_COMPONENT_DESCRIPTOR_H

#include "Math/Color4.h"

#include "Engine/GameObject.h"
#include "Engine/Components.h"

namespace Helium
{

    
    class HELIUM_FRAMEWORK_API ColorComponent : public Helium::Components::Component
    {
    public:
        OBJECT_DECLARE_COMPONENT(Helium::ColorComponent, Helium::Components::Component);
        
        virtual void FinalizeComponent(const Helium::ComponentDefinition *_descriptor);
        
    private:
        Color4 m_Color;
        ComponentPtr<ColorComponent> m_Pointer;
    };

    class HELIUM_FRAMEWORK_API ComponentDescriptor_ColorComponent : public Helium::ComponentDefinition
    {
        HELIUM_DECLARE_OBJECT(ComponentDescriptor_ColorComponent, Helium::ComponentDefinition);
        static void PopulateComposite( Reflect::Composite& comp );
        
        virtual Helium::Component *CreateComponentInternal(Helium::Components::ComponentSet &_target) const;

    public:
        Color4 m_Color;
        ComponentDefinitionPtr m_Pointer;
    };

}

#endif
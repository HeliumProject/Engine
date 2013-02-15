
#pragma once
#ifndef HELIUM_FRAMEWORK_COMPONENT_DESCRIPTOR_H
#define HELIUM_FRAMEWORK_COMPONENT_DESCRIPTOR_H

#include "Math/Color4.h"

#include "Engine/GameObject.h"
#include "Engine/Components.h"

namespace Helium
{
    class HELIUM_FRAMEWORK_API ComponentDescriptor : public Helium::GameObject
    {
    public:
        HELIUM_DECLARE_OBJECT(ComponentDescriptor, Helium::GameObject);

        virtual Helium::Component *CreateComponent() { return NULL; }
        //virtual void LinkComponent(
    };
    typedef Helium::StrongPtr<ComponentDescriptor> ComponentDescriptorPtr;
    
    class HELIUM_FRAMEWORK_API ColorComponent : public Helium::Components::Component
    {
    public:
        OBJECT_DECLARE_COMPONENT(Helium::ColorComponent, Helium::Components::Component);

    private:
        Color4 m_Color;
    };

    class HELIUM_FRAMEWORK_API ComponentDescriptor_ColorComponent : public Helium::ComponentDescriptor
    {
        HELIUM_DECLARE_OBJECT(ComponentDescriptor_ColorComponent, Helium::ComponentDescriptor);
        static void PopulateComposite( Reflect::Composite& comp );

    public:
        Color4 m_Color;
        GameObjectPtr m_Pointer;
    };

}

#endif
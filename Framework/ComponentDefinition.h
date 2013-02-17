
#pragma once
#ifndef HELIUM_FRAMEWORK_COMPONENT_DESCRIPTOR_H
#define HELIUM_FRAMEWORK_COMPONENT_DESCRIPTOR_H

#include "Math/Color4.h"

#include "Engine/GameObject.h"
#include "Engine/Components.h"

namespace Helium
{
    class HELIUM_FRAMEWORK_API ComponentDefinition : public Helium::GameObject
    {
    public:
        HELIUM_DECLARE_OBJECT(ComponentDefinition, Helium::GameObject);

        Helium::Component *CreateComponent(struct Components::ComponentSet &_target) const
        {
            m_Instance.Set(CreateComponentInternal(_target));
            return m_Instance.Get();
        }

        virtual Helium::Component *CreateComponentInternal(struct Components::ComponentSet &_target) const { HELIUM_ASSERT(0); return 0; }
        virtual void FinalizeComponent() const { }

        Helium::Component *GetCreatedComponent() const { return m_Instance.Get(); }

    private:
        mutable Helium::ComponentPtr<Component> m_Instance;
    };
    typedef Helium::StrongPtr<ComponentDefinition> ComponentDefinitionPtr;
        
    //////////////////////////////////////////////////////////////////////////
    class ColorComponentDefinition;
    class HELIUM_FRAMEWORK_API ColorComponent : public Helium::Components::Component
    {
    public:
        OBJECT_DECLARE_COMPONENT(Helium::ColorComponent, Helium::Components::Component);
        
        void Finalize(const Helium::ColorComponentDefinition *_descriptor);
        
    private:
        Color4 m_Color;
        ComponentPtr<ColorComponent> m_Pointer;
    };

    class HELIUM_FRAMEWORK_API ColorComponentDefinition : public Helium::ComponentDefinition
    {
    public:
        HELIUM_DECLARE_OBJECT(ColorComponentDefinition, Helium::ComponentDefinition);
        static void PopulateComposite( Reflect::Composite& comp );
        
        Helium::Component *CreateComponentInternal(struct Components::ComponentSet &_target) const
        {
            return Helium::Components::Allocate<ColorComponent>(_target);
        }

        virtual void FinalizeComponent() const
        {
            ColorComponent *color_component = static_cast<ColorComponent *>(GetCreatedComponent());
        }

        Color4 m_Color;
        ComponentDefinitionPtr m_Pointer;
    };

}

#endif
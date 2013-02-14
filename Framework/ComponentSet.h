
#pragma once

#ifndef HELIUM_FRAMEWORK_COMPONENT_SET_H
#define HELIUM_FRAMEWORK_COMPONENT_SET_H

#include "Foundation/DynamicArray.h"

#include "Engine/GameObject.h"
#include "Engine/Components.h"

#include "Framework/ComponentDescriptor.h"

namespace Helium
{
    class HELIUM_FRAMEWORK_API ComponentInitContext
    {
        struct ComponentListEntry
        {
            Name m_Name;
            ComponentDescriptor *m_Descriptor;
            Helium::Components::Component *m_Component;
        };

        DynamicArray<ComponentListEntry> m_Components;
    };

    class ComponentFactory;

    class HELIUM_FRAMEWORK_API ComponentSet : public Helium::GameObject
    {
    public:
        HELIUM_DECLARE_OBJECT(Helium::ComponentSet, Helium::GameObject);
        void AddDescriptor( Helium::Name _name, Helium::StrongPtr<Helium::ComponentDescriptor> _color_descriptor );
        void AddLinkage( Helium::Name _to, Helium::Name _from, Helium::Name _field_name );
        
        friend ComponentFactory;

    private:

        struct DescriptorListEntry
        {
            Name m_ComponentName;
            Helium::StrongPtr<ComponentDescriptor> m_ComponentDescriptor;
        };

        struct Linkage
        {
            Name m_ComponentName;
            Name m_ComponentFieldName;
            
            Name m_OtherComponentName;
        };

        struct NamedParameter
        {
            Name m_ComponentName;
            Name m_ComponentFieldName;

            Name m_ParamName;
        };

        DynamicArray<DescriptorListEntry> m_Descriptors;
        DynamicArray<Linkage> m_Linkages;
        DynamicArray<NamedParameter> m_NamedParameter;
    };
    
    struct IParameter
    {
        virtual ~IParameter() { }
    };
    
    template <class T>
    struct Parameter : public IParameter
    {
        Name m_Name;
        T m_Parameter;
    };

    class HELIUM_FRAMEWORK_API ComponentFactory
    {
    public:
        ComponentFactory()
            : m_Heap(64)
        {

        }

        ~ComponentFactory()
        {
            for (size_t count = m_Parameters.GetSize() - 1; count >= 0; --count)
            {
                HELIUM_DELETE(m_Heap, m_Parameters[count]);
                m_Parameters.Clear();
            }
        }

        DynamicArray<IParameter *> m_Parameters;
        Helium::StackMemoryHeap<> m_Heap;

        template <class T>
        void SetParameter(Name name, T value)
        {
            Parameter<T> *param = HELIUM_NEW(m_Heap, Parameter<T>);
            param->m_Name = name;
            param->m_Parameter = value;
            m_Parameters.Add(param);
        }

        void DeployComponents(ComponentSet &set, Helium::Components::ComponentSet &_components);
    };
}

#endif
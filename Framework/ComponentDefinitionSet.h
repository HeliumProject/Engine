
#pragma once

#ifndef HELIUM_FRAMEWORK_COMPONENT_SET_H
#define HELIUM_FRAMEWORK_COMPONENT_SET_H

#include "Foundation/DynamicArray.h"

#include "Engine/GameObject.h"
#include "Engine/Components.h"

#include "Framework/ComponentDefinition.h"
#include "Reflect/Data/Data.h"

namespace Helium
{
    class ComponentDefinitionSet;
    class ParameterSet;

    namespace Components
    {
        void HELIUM_FRAMEWORK_API DeployComponents(Helium::ComponentDefinitionSet &_components, ParameterSet &_parameters, Helium::Components::ComponentSet &_target);
    }

    class HELIUM_FRAMEWORK_API ComponentDefinitionSet : public Helium::GameObject
    {
    public:
        HELIUM_DECLARE_OBJECT(Helium::ComponentDefinitionSet, Helium::GameObject);
        void AddDescriptor( Helium::Name _name, Helium::StrongPtr<Helium::ComponentDefinition> _color_descriptor );
        void AddParameter( Helium::Name _param_name, Helium::Name _component_name, Helium::Name _field_name );
        
        friend void Helium::Components::DeployComponents(Helium::ComponentDefinitionSet &_components, ParameterSet &_parameters, Helium::Components::ComponentSet &_target);

    private:

        struct DescriptorListEntry
        {
            Name m_ComponentName;
            Helium::StrongPtr<ComponentDefinition> m_ComponentDescriptor;
        };

        struct Parameter
        {
            Name m_ComponentName;
            Name m_ComponentFieldName;

            Name m_ParamName;
        };

        DynamicArray<DescriptorListEntry> m_Descriptors;
        DynamicArray<Parameter> m_Parameters;
    };
    typedef Helium::StrongPtr<ComponentDefinitionSet> ComponentDefinitionSetPtr;

    class HELIUM_FRAMEWORK_API ParameterSet
    {
    public:
        struct IParameter
        {
            virtual Name GetName() = 0;
            virtual Reflect::DataPtr GetDataPtr() = 0;

            // This has to be here or I get warnings. TODO: Figure out what needs to be in this function if anything.
            void operator delete(void *, Helium::MemoryHeap &) { }
            virtual ~IParameter() { }
        };
    
        template <class T>
        struct Parameter : public IParameter
        {
            virtual ~Parameter() { }

            virtual Name GetName() { return m_Name; }
            virtual Reflect::DataPtr GetDataPtr()
            {
                Reflect::DataPtr data(Reflect::AssertCast<Helium::Reflect::Data>(Reflect::GetDataClass<T>()->m_Creator()));
                data->ConnectData(&m_Parameter);
                return data;
            }

            Name m_Name;
            T m_Parameter;
        };

        ParameterSet(size_t _block_size = 128)
            : m_Heap(_block_size)
        {

        }

        ~ParameterSet()
        {
            for (size_t index = 0; index < m_Parameters.GetSize(); ++index)
            {
                HELIUM_DELETE(m_Heap, m_Parameters[m_Parameters.GetSize() - index - 1]);
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
        
        friend void Helium::Components::DeployComponents(Helium::ComponentDefinitionSet &_components, ParameterSet &_parameters, Helium::Components::ComponentSet &_target);

    };
}

#endif
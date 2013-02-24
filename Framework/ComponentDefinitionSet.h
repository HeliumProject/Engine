
#pragma once

#ifndef HELIUM_FRAMEWORK_COMPONENT_SET_H
#define HELIUM_FRAMEWORK_COMPONENT_SET_H

#include "Foundation/DynamicArray.h"

#include "Engine/Asset.h"
#include "Engine/Components.h"

#include "Framework/ComponentDefinition.h"
#include "Reflect/Data/Data.h"

namespace Helium
{
    class ComponentDefinitionSet;
    class ParameterSet;

    namespace Components
    {
        // Construct a new set of components and apply them to the target. This isn't a member
        // function because I plan to support giving an array of definitions to combine, and possibly an
        // out parameter that includes extra name/component lookups, etc.
        void HELIUM_FRAMEWORK_API DeployComponents(Entity *pEntity, const Helium::ComponentDefinitionSet &components, const ParameterSet &parameters, Helium::Components::ComponentSet &target);
    }

    // Holds a set of definitions and allows them to construct and wire up together. Parameters can be provided, and the components themselves
    // can be components
    class HELIUM_FRAMEWORK_API ComponentDefinitionSet : public Helium::Asset
    {
    public:
        HELIUM_DECLARE_ASSET(Helium::ComponentDefinitionSet, Helium::Asset);

        // Add a component definition to list of definitions to construct
        void AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition );

        // Define a parameter that can be set via parameter set or a named component
        void ExposeParameter( Helium::Name paramName, Helium::Name componentName, Helium::Name fieldName );
        
        friend void Helium::Components::DeployComponents(Entity *pEntity, const Helium::ComponentDefinitionSet &components, const ParameterSet &parameters, Helium::Components::ComponentSet &target);

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
}

#endif
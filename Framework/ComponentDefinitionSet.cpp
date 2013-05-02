
#include "FrameworkPch.h"
#include "ComponentDefinitionSet.h"
#include "Framework/ParameterSet.h"

HELIUM_IMPLEMENT_ASSET(Helium::ComponentDefinitionSet, Framework, 0);

struct NewComponent
{
    Helium::Component *m_Component;
    Helium::StrongPtr<Helium::ComponentDefinition> m_Descriptor;
};

struct DefinedParameter
{
    Helium::Name m_Name;
    Helium::Reflect::DataPtr m_Data;
};

void Helium::Components::DeployComponents( IHasComponents &rHasComponents, const Helium::ComponentDefinitionSet &componentDefinitionSet, const Helium::ParameterSet &parameterSet)
{
    // 1. Clone all component descriptors
    typedef Map<Name, NewComponent> M_NewComponents;
    M_NewComponents components;

    // For each descriptor
    for (size_t i = 0; i < componentDefinitionSet.m_Descriptors.GetSize(); ++i)
    {
        M_NewComponents::Iterator iter = components.Find(componentDefinitionSet.m_Descriptors[i].m_ComponentName);

        if (iter != components.End())
        {
            //TODO: Warn of duplicate component name, treat as unnamed component?
            continue;
        }

        // Clone it
        Reflect::ObjectPtr object_ptr = componentDefinitionSet.m_Descriptors[i].m_ComponentDescriptor->Clone();
        Helium::ComponentDefinitionPtr descriptor_ptr = Reflect::AssertCast<Helium::ComponentDefinition>(object_ptr.Get());

        // Add it to the list
        NewComponent new_component;
        new_component.m_Descriptor = descriptor_ptr;
        new_component.m_Component = NULL;

        components.Insert(iter, M_NewComponents::ValueType(componentDefinitionSet.m_Descriptors[i].m_ComponentName, new_component));

        Log::Print("%s cloned to %x\n", componentDefinitionSet.m_Descriptors[i].m_ComponentName.Get(), descriptor_ptr.Get());
    }
    
    // 2. Build the parameter list using parameters
    typedef HashMap<Name, Reflect::DataPtr> HM_ParametersValues;
    HM_ParametersValues parameter_values;

    for (size_t i = 0; i < parameterSet.m_Parameters.GetSize(); ++i)
    {
        HM_ParametersValues::Iterator iter = parameter_values.Find(parameterSet.m_Parameters[i]->GetName());
        if (iter != parameter_values.End())
        {
            // TODO: Warn duplicate parameter value?
            continue;
        }

        parameter_values.Insert(iter, HM_ParametersValues::ValueType(parameterSet.m_Parameters[i]->GetName(), parameterSet.m_Parameters[i]->GetDataPtr()));
    }

    // 3. Add any components to the parameter list (but don't overwrite named parameters_values)
    for (M_NewComponents::Iterator component_iter = components.Begin(); component_iter != components.End(); ++component_iter)
    {
        HM_ParametersValues::Iterator parameter_value_iter = parameter_values.Find(component_iter->First());
        if (parameter_value_iter != parameter_values.End())
        {
            // TODO: Warn duplicate parameter value?
            continue;
        }      
        
        // TODO: Does this shorthand work?
        //Helium::Reflect::DataPtr ptr(Helium::AssetPointerData::Create(&components[i].m_Descriptor));
        Helium::Reflect::DataPtr ptr(new Helium::AssetPointerData());
        static_cast<Helium::AssetPointerData *>(ptr.Get())->ConnectData(&component_iter->Second().m_Descriptor);
        parameter_values.Insert(parameter_value_iter, HM_ParametersValues::ValueType(component_iter->First(), ptr));
    }

    // 4. Plug in the parameters to the components
    for (size_t parameter_index = 0; parameter_index < componentDefinitionSet.m_Parameters.GetSize(); ++parameter_index)
    {
        // NOTE: It's ok to have duplicate parameters.. we'll just assign the value to more than one place!
        const Helium::ComponentDefinitionSet::Parameter &parameter = componentDefinitionSet.m_Parameters[parameter_index];
        
        HM_ParametersValues::Iterator value_iter = parameter_values.Find(parameter.m_ParamName);
        if (value_iter == parameter_values.End())
        {
            // TODO: Warn that a parameter was unsupplied?
            continue;
        }

        M_NewComponents::Iterator component_iter = components.Find(parameter.m_ComponentName);
        if (component_iter == components.End())
        {
            // TODO: Param points at a non-existent component.. warn
            continue;
        }
        
        uint32_t fieldNameCrc = Crc32( parameter.m_ComponentFieldName.Get() );
        const Helium::Reflect::Field *field = component_iter->Second().m_Descriptor->GetClass()->FindFieldByName(fieldNameCrc);

        if (!field)
        {
            // TODO: Field doesn't exist on the component type.. warn
            continue;
        }

        Helium::Reflect::DataPtr data = field->CreateData(component_iter->Second().m_Descriptor.Get());
        data->Set(value_iter->Second(), Reflect::CopyFlags::Shallow);
    }

    // 5. Create components and populate component table
    for (M_NewComponents::Iterator iter = components.Begin(); iter != components.End(); ++iter)
    {
        //iter.->m_Component = iter.Second()->m_Descriptor->CreateComponent(target);
        iter->Second().m_Component = iter->Second().m_Descriptor->CreateComponent(rHasComponents);
    }
    
    // 5. Create components and populate component table
    for (M_NewComponents::Iterator iter = components.Begin(); iter != components.End(); ++iter)
    {
        iter->Second().m_Descriptor->FinalizeComponent();
    }

    // 6. Destroy descriptor clones? Maybe keep them in TOOLS builds?
    // 7. Make each component point back to the original descriptor that made it?
}

void Helium::ComponentDefinitionSet::AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition )
{
    DescriptorListEntry entry;
    entry.m_ComponentName = name;
    entry.m_ComponentDescriptor = pComponentDefinition;
    m_Descriptors.Add(entry);
}

void Helium::ComponentDefinitionSet::ExposeParameter( Helium::Name paramName, Helium::Name componentName, Helium::Name fieldName )
{
    Parameter l;
    l.m_ParamName = paramName;
    l.m_ComponentName = componentName;
    l.m_ComponentFieldName = fieldName;
    m_Parameters.Add(l);
}

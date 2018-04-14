#include "FrameworkPch.h"
#include "ComponentSet.h"
#include "Framework/ParameterSet.h"
#include "Foundation/Log.h"
#include "Framework/ComponentDefinition.h"
#include "Reflect/TranslatorDeduction.h"

struct NewComponent
{
	Helium::Component *m_Component;
	Helium::StrongPtr<Helium::ComponentDefinition> m_Descriptor;
};

void Helium::Components::DeployComponents( 
	IHasComponents &rHasComponents, 
	const Helium::ComponentSet &componentDefinitionSet, 
	const Helium::ParameterSet *parameterSet)
{
	ParameterSet emptyParameterSet;
	if (!parameterSet)
	{
		parameterSet = &emptyParameterSet;
	}

	HELIUM_TRACE(
		TraceLevels::Debug,
		"Helium::Components::DeployComponents() - Beginning to deploy components from component set\n");

	//////////////////////////////////////////////////////////////////////////
	// 1. Clone all component descriptors
	//////////////////////////////////////////////////////////////////////////
	typedef Map<Name, NewComponent> M_NewComponents;
	M_NewComponents components;

	// For each descriptor
	for (size_t i = 0; i < componentDefinitionSet.m_Components.GetSize(); ++i)
	{
		const ComponentSet::NameDefinitionPair &component_to_clone = componentDefinitionSet.m_Components[i];
		M_NewComponents::Iterator iter = components.Find(component_to_clone.m_Name);

		if (iter != components.End())
		{
			//TODO: Warn of duplicate component name, treat as unnamed component?
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Multiple components named '%s'\n",
				*component_to_clone.m_Name);
			continue;
		}

		if ( !component_to_clone.m_Definition.ReferencesObject() )
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Cannot clone null component named '%s'\n",
				*component_to_clone.m_Name);
			continue;
		}

		// Clone it
		Reflect::ObjectPtr object_ptr = component_to_clone.m_Definition->Clone();
		Helium::ComponentDefinitionPtr new_descriptor = Reflect::AssertCast<Helium::ComponentDefinition>(object_ptr.Get());

		// Add it to the list
		NewComponent new_component;
		new_component.m_Descriptor = new_descriptor;
		new_component.m_Component = NULL;

		components.Insert(iter, M_NewComponents::ValueType(component_to_clone.m_Name, new_component));

		HELIUM_TRACE( TraceLevels::Debug, 
			"  Component '%s' cloned to %x\n", 
			component_to_clone.m_Name.Get(), 
			new_descriptor.Get());
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 2. Add passed in parameters to the parameter list
	//////////////////////////////////////////////////////////////////////////
	typedef HashMap<Name, Reflect::Pointer> HM_ParametersValues;
	HM_ParametersValues parameter_values;

	DynamicArray<Parameter> parameters;
	parameterSet->EnumerateParameters(parameters);

	for (size_t i = 0; i < parameters.GetSize(); ++i)
	{
		HM_ParametersValues::Iterator iter = parameter_values.Find(parameters[i].GetName());
		if (iter != parameter_values.End())
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Duplicate parameter '%s' - ignored\n", 
				*parameters[i].GetName());
			continue;
		}

		parameter_values.Insert( iter, HM_ParametersValues::ValueType(
			parameters[i].GetName(), 
			parameters[i].GetPointer() ));
	}

	//////////////////////////////////////////////////////////////////////////
	// 3. Add components to the parameter list (but don't overwrite named parameters_values)
	//////////////////////////////////////////////////////////////////////////
	for (M_NewComponents::Iterator component_iter = components.Begin(); component_iter != components.End(); ++component_iter)
	{
		HM_ParametersValues::Iterator parameter_value_iter = parameter_values.Find(component_iter->First());
		if (parameter_value_iter != parameter_values.End())
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Duplicate parameter value '%s' - ignored.\n", 
				*component_iter->First());

			continue;
		}
		
		// TODO: Does this shorthand work?
		//Helium::Reflect::DataPtr ptr(Helium::AssetPointerData::Create(&components[i].m_Descriptor));
		//Helium::Reflect::DataPtr ptr(new Helium::AssetPointerData());
		//static_cast<Helium::AssetPointerData *>(ptr.Get())->ConnectData(&component_iter->Second().m_Descriptor);

		//component_iter->Second().m_Descriptor
		parameter_values.Insert(parameter_value_iter, HM_ParametersValues::ValueType(
			component_iter->First(), 
			Reflect::Pointer( component_iter->Second().m_Descriptor ) ));
	}

	//////////////////////////////////////////////////////////////////////////
	// 4. Plug in the parameters to the components
	//////////////////////////////////////////////////////////////////////////
	for (size_t parameter_index = 0; parameter_index < componentDefinitionSet.m_Parameters.GetSize(); ++parameter_index)
	{
		// NOTE: It's ok to have duplicate parameters.. we'll just assign the value to more than one place!
		const Helium::ComponentSet::Parameter &parameter = componentDefinitionSet.m_Parameters[parameter_index];
		
		HM_ParametersValues::Iterator value_iter = parameter_values.Find(parameter.m_ParameterName);
		if (value_iter == parameter_values.End())
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Unsupplied parameter value '%s' - ignored.\n",
				*parameter.m_ParameterName);

			continue;
		}

		M_NewComponents::Iterator component_iter = components.Find(parameter.m_ComponentName);
		if (component_iter == components.End())
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Supplied parameter value '%s' refers to a component '%s' that cannot be found - ignored.\n",
				*parameter.m_ParameterName,
				*parameter.m_ComponentName);

			continue;
		}
		
		uint32_t fieldNameCrc = Crc32( parameter.m_ComponentFieldName.Get() );
		const Helium::Reflect::Field *field = component_iter->Second().m_Descriptor->GetMetaClass()->FindFieldByName(fieldNameCrc);

		if (!field)
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"  Supplied parameter value '%s' cannot find field named '%s' on component '%s' - ignored.\n",
				*parameter.m_ParameterName,
				*parameter.m_ComponentFieldName,
				*parameter.m_ComponentName);

			continue;
		}

		field->m_Translator->Copy( 
			value_iter->Second(), 
			Reflect::Pointer( field, component_iter->Second().m_Descriptor.Get() ),
			Reflect::CopyFlags::Shallow );

		//Helium::Reflect::DataPtr data = field->CreateData(component_iter->Second().m_Descriptor.Get());
		//data->Set(value_iter->Second(), Reflect::CopyFlags::Shallow);
	}

	// 5. Create components and populate component table
	for (M_NewComponents::Iterator iter = components.Begin(); iter != components.End(); ++iter)
	{
		//iter.->m_Component = iter.Second()->m_Descriptor->CreateComponent(target);
		iter->Second().m_Component = iter->Second().m_Descriptor->CreateComponent(rHasComponents);
	}
	
	// 6. Second pass to allow components to get references to each other if need be
	for (M_NewComponents::Iterator iter = components.Begin(); iter != components.End(); ++iter)
	{
		iter->Second().m_Descriptor->FinalizeComponent();
	}

	// 7. Destroy descriptor clones? Maybe keep them in TOOLS builds?
	// 8. Make each component point back to the original descriptor that made it?
}

void HELIUM_FRAMEWORK_API Helium::Components::DeployComponents( IHasComponents &rHasComponents, const DynamicArray<ComponentDefinitionPtr> &components )
{
	for (DynamicArray<ComponentDefinitionPtr>::ConstIterator iter = components.Begin();
		iter != components.End(); ++iter)
	{
		if (*iter)
		{
			(*iter)->CreateComponent(rHasComponents);
		}
		else
		{
			HELIUM_TRACE( 
				TraceLevels::Warning, 
				"DeployComponents - A ComponentDefinitionPtr in the supplied list was null - ignoring.\n" );
		}
	}

	for (DynamicArray<ComponentDefinitionPtr>::ConstIterator iter = components.Begin();
		iter != components.End(); ++iter)
	{
		if (*iter)
		{
			(*iter)->FinalizeComponent();
		}
	}
}

HELIUM_DEFINE_BASE_STRUCT(Helium::ComponentSet);

void Helium::ComponentSet::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &ComponentSet::m_Components, "m_Components" );
	comp.AddField( &ComponentSet::m_Parameters, "m_Parameters" );
}

void Helium::ComponentSet::AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition )
{
	NameDefinitionPair entry;
	entry.m_Name = name;
	entry.m_Definition = pComponentDefinition;
	m_Components.Add(entry);
}

void Helium::ComponentSet::ExposeParameter( Helium::Name paramName, Helium::Name componentName, Helium::Name fieldName )
{
	Parameter l;
	l.m_ParameterName = paramName;
	l.m_ComponentName = componentName;
	l.m_ComponentFieldName = fieldName;
	m_Parameters.Add(l);
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ComponentSet::NameDefinitionPair );

void Helium::ComponentSet::NameDefinitionPair::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &NameDefinitionPair::m_Name, "m_Name" );
	comp.AddField( &NameDefinitionPair::m_Definition, "m_Definition" );
}

bool Helium::ComponentSet::NameDefinitionPair::operator==( const NameDefinitionPair& _rhs ) const
{
	return ( 
		m_Name == _rhs.m_Name &&
		m_Definition == _rhs.m_Definition
		);
}

bool Helium::ComponentSet::NameDefinitionPair::operator!=( const NameDefinitionPair& _rhs ) const
{
	return !( *this == _rhs );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::ComponentSet::Parameter );

void Helium::ComponentSet::Parameter::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Parameter::m_ComponentName, "m_ComponentName" );
	comp.AddField( &Parameter::m_ComponentFieldName, "m_ComponentFieldName" );
	comp.AddField( &Parameter::m_ParameterName, "m_ParameterName" );
}

bool Helium::ComponentSet::Parameter::operator==( const Parameter& _rhs ) const
{
	return ( 
		m_ComponentName == _rhs.m_ComponentName &&
		m_ComponentFieldName == _rhs.m_ComponentFieldName &&
		m_ParameterName == _rhs.m_ParameterName
		);
}

bool Helium::ComponentSet::Parameter::operator!=( const Parameter& _rhs ) const
{
	return !( *this == _rhs );
}

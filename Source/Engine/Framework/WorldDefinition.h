#pragma once

#include "Framework/Framework.h"

#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentSet.h"

namespace Helium
{    
	class World;
	typedef StrongPtr< World > WorldPtr;

	class ComponentSet;
	typedef StrongPtr< ComponentSet > ComponentDefinitionSetPtr;

	class ComponentDefinition;

	/// Base type for in-world entities.
	class HELIUM_FRAMEWORK_API WorldDefinition : public Asset
	{
		HELIUM_DECLARE_ASSET( WorldDefinition, Asset );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

	public:

		/// @name Construction/Destruction
		//@{
		WorldDefinition();
		virtual ~WorldDefinition();
		//@}
				
		void AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition );

		ComponentSet &GetComponentDefinitions() { return m_ComponentSet; }
		
		WorldPtr CreateWorld() const;
		
	private:

		ComponentSet m_ComponentSet;
		DynamicArray<ComponentDefinitionPtr> m_Components;
	};
	typedef Helium::StrongPtr<WorldDefinition> WorldDefinitionPtr;
}

#include "Framework/WorldDefinition.inl"

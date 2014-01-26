#pragma once

#include "Framework/Framework.h"
#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"
#include "Framework/Entity.h"

namespace Helium
{
	class SceneDefinition;
	typedef Helium::WeakPtr< SceneDefinition > SceneDefinitionWPtr;
	typedef Helium::WeakPtr< const SceneDefinition > ConstSceneDefinitionWPtr;

	class Entity;
	typedef Helium::StrongPtr< Entity > EntityPtr;

	class ParameterSet;
		
	/// Base type for in-world entities.
	class HELIUM_FRAMEWORK_API EntityDefinition : public Asset
	{
		HELIUM_DECLARE_ASSET( EntityDefinition, Asset );
		static void PopulateMetaType( Reflect::MetaStruct& comp );

	public:

		/// @name Construction/Destruction
		//@{
		EntityDefinition();
		virtual ~EntityDefinition();
		//@}
		
		void AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition );

		ComponentDefinitionSet *GetComponentDefinitions() { return m_ComponentDefinitionSet; }

		// Two phase construction to allow the entity to be set up before components get finalized
		EntityPtr CreateEntity();
		void FinalizeEntity(Entity *pEntity, const ParameterSet *pParameterSet = 0);

	private:
		void InitComponentDefinitionSet()
		{
			Asset::Create<ComponentDefinitionSet>(m_ComponentDefinitionSet, Name(NULL_NAME), 0);
		}

		ComponentDefinitionSetPtr m_ComponentDefinitionSet;
		DynamicArray<ComponentDefinitionPtr> m_Components;
	};
	typedef Helium::StrongPtr<EntityDefinition> EntityDefinitionPtr;
}

#pragma once

#include "Framework/Framework.h"

#include "Framework/ComponentDefinition.h"
#include "Framework/ComponentDefinitionSet.h"

namespace Helium
{    
    class World;
    typedef StrongPtr< World > WorldPtr;

	class ComponentDefinitionSet;
	typedef StrongPtr< ComponentDefinitionSet > ComponentDefinitionSetPtr;

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

        ComponentDefinitionSet *GetComponentDefinitions() { return m_ComponentDefinitionSet.Get(); }
        
        WorldPtr CreateWorld() const;
        
    private:
        void InitComponentDefinitionSet()
        {
            Asset::Create<ComponentDefinitionSet>(m_ComponentDefinitionSet, Name(TXT("")), 0);
        }

		ComponentDefinitionSetPtr m_ComponentDefinitionSet;
		DynamicArray<ComponentDefinitionPtr> m_Components;
    };
    typedef Helium::StrongPtr<WorldDefinition> WorldDefinitionPtr;
}

#include "Framework/WorldDefinition.inl"

//----------------------------------------------------------------------------------------------------------------------
// EntityDefinition.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_ENTITY_DEFINITION_H
#define HELIUM_FRAMEWORK_ENTITY_DEFINITION_H

#include "Framework/Framework.h"
#include "Engine/Asset.h"

#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"

#include "Framework/ComponentDefinitionSet.h"
#include "Framework/SceneDefinition.h"

namespace Helium
{
    class SceneDefinition;
    typedef Helium::WeakPtr< SceneDefinition > SceneDefinitionWPtr;
    typedef Helium::WeakPtr< const SceneDefinition > ConstSceneDefinitionWPtr;

    class Entity;
    typedef Helium::StrongPtr< Entity > EntityPtr;
    
    /// Base type for in-world entities.
    class HELIUM_FRAMEWORK_API EntityDefinition : public Asset
    {
        HELIUM_DECLARE_ASSET( EntityDefinition, Asset );
        static void PopulateStructure( Reflect::Structure& comp );

    public:

        /// @name Construction/Destruction
        //@{
        EntityDefinition();
        virtual ~EntityDefinition();
        //@}
        
        void AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition )
        {
            if (!m_ComponentDefinitionSet)
            {
                InitComponentDefinitionSet();
            }

            m_ComponentDefinitionSet->AddComponentDefinition(name, pComponentDefinition);
        }

        ComponentDefinitionSet *GetComponentDefinitions() { return m_ComponentDefinitionSet; }

        // Two phase construction to allow the entity to be set up before components get finalized
        EntityPtr CreateEntity();
        void FinalizeEntity(Entity *pEntity);

    private:
        void InitComponentDefinitionSet()
        {
            Asset::Create<ComponentDefinitionSet>(m_ComponentDefinitionSet, Name(TXT("")), 0);
        }

        ComponentDefinitionSetPtr m_ComponentDefinitionSet;
    };
    typedef Helium::StrongPtr<EntityDefinition> EntityDefinitionPtr;
}

#include "Framework/EntityDefinition.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_DEFINITION_H

//----------------------------------------------------------------------------------------------------------------------
// EntityDefinition.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WORLD_DEFINITION_H
#define HELIUM_FRAMEWORK_WORLD_DEFINITION_H

#include "Framework/Framework.h"
#include "Engine/Asset.h"

#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"

#include "Framework/ComponentDefinitionSet.h"

namespace Helium
{    
    class World;
    typedef StrongPtr< World > WorldPtr;

    /// Base type for in-world entities.
    class HELIUM_FRAMEWORK_API WorldDefinition : public Asset
    {
        HELIUM_DECLARE_ASSET( WorldDefinition, Asset );

    public:

        /// @name Construction/Destruction
        //@{
        WorldDefinition();
        virtual ~WorldDefinition();
        //@}
                
        void AddComponentDefinition( Helium::Name name, Helium::ComponentDefinition *pComponentDefinition )
        {
            if (!m_ComponentDefinitions)
            {
                InitComponentDefinitionSet();
            }

            m_ComponentDefinitions->AddComponentDefinition(name, pComponentDefinition);
        }

        ComponentDefinitionSet *GetComponentDefinitions() { return m_ComponentDefinitions; }
        
        WorldPtr CreateWorld() const;
        
    private:
        void InitComponentDefinitionSet()
        {
            Asset::Create<ComponentDefinitionSet>(m_ComponentDefinitions, Name(TXT("")), 0);
        }

        ComponentDefinitionSetPtr m_ComponentDefinitions;
    };
    typedef Helium::StrongPtr<WorldDefinition> WorldDefinitionPtr;
}

#include "Framework/WorldDefinition.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_DEFINITION_H

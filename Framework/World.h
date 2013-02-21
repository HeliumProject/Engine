//----------------------------------------------------------------------------------------------------------------------
// World.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WORLD_H
#define HELIUM_FRAMEWORK_WORLD_H

#include "Framework/Framework.h"
#include "Engine/Asset.h"

#include "MathSimd/Quat.h"
#include "MathSimd/Vector3.h"
#include "Graphics/GraphicsScene.h"

#include "Framework/WorldDefinition.h"

namespace Helium
{
    class Entity;
    class EntityDefinition;

    class GraphicsScene;
    typedef Helium::StrongPtr< GraphicsScene > GraphicsScenePtr;
    typedef Helium::StrongPtr< const GraphicsScene > ConstGraphicsScenePtr;

    class Slice;
    typedef Helium::StrongPtr< Slice > SlicePtr;

    class SceneDefinition;
    
    class WorldDefinition;
    typedef Helium::StrongPtr< WorldDefinition > WorldDefinitionPtr;
    typedef Helium::StrongPtr< const WorldDefinition > ConstWorldDefinitionPtr;

    /// World instance.
    ///
    /// A world contains a discrete group of entities that can be simulated within an application environment.  Multiple
    /// world instances can exist at the same time, allowing the use of specific worlds for special-case scenarios, such
    /// as rendering scenes outside the game world to a texture or editor preview windows.
    class HELIUM_FRAMEWORK_API World : public Reflect::Object
    {
        REFLECT_DECLARE_OBJECT( Helium::World, Reflect::Object);

    public:
        /// @name Construction/Destruction
        //@{
        World();
        virtual ~World();
        //@}

        /// @name World Initialization
        //@{
        virtual bool Initialize(WorldDefinitionPtr _world_definition);
        virtual void Shutdown();
        //@}

        /// @name World Updating
        //@{
        virtual void UpdateGraphicsScene();
        //@}

        /// @name Asset Interface
        //@{
        virtual void PreDestroy();
        //@}

        /// @name EntityDefinition Creation
        //@{
        virtual EntityDefinition* CreateEntity(
            SceneDefinition* pSlice, Entity* pEntity);
        virtual bool DestroyEntity( Entity* pEntity );
        //@}

        /// @name SceneDefinition Registration
        //@{
        virtual bool AddSlice( Slice* pSlice );
        virtual bool RemoveSlice( Slice* pSlice );

        inline size_t GetSliceCount() const;
        Slice* GetSlice( size_t index ) const;
        //@}

        /// @name Scene Access
        //@{
        GraphicsScene* GetGraphicsScene() const;
        WorldDefinition* GetWorldDefinition() { return m_spWorldDefinition.Get(); }
        //@}

    private:
        Helium::StrongPtr<WorldDefinition> m_spWorldDefinition;

        /// Active slices.
        DynamicArray< SlicePtr > m_slices;

        /// Graphics scene instance.
        GraphicsScenePtr m_spGraphicsScene;
    };

    typedef Helium::StrongPtr< World > WorldPtr;
    typedef Helium::StrongPtr< const World > ConstWorldPtr;
}

#include "Framework/World.inl"

#endif  // HELIUM_FRAMEWORK_WORLD_H

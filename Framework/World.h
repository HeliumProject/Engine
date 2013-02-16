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
#include "Engine/GameObject.h"

#include "MathSimd/Quat.h"
#include "MathSimd/Vector3.h"
#include "Graphics/GraphicsScene.h"

namespace Helium
{
    class EntityDefinition;

    class GraphicsScene;
    typedef Helium::StrongPtr< GraphicsScene > GraphicsScenePtr;
    typedef Helium::StrongPtr< const GraphicsScene > ConstGraphicsScenePtr;

    class Slice;
    typedef Helium::StrongPtr< Slice > SlicePtr;
    typedef Helium::StrongPtr< const Slice > ConstSlicePtr;

    /// World instance.
    ///
    /// A world contains a discrete group of entities that can be simulated within an application environment.  Multiple
    /// world instances can exist at the same time, allowing the use of specific worlds for special-case scenarios, such
    /// as rendering scenes outside the game world to a texture or editor preview windows.
    class HELIUM_FRAMEWORK_API World : public GameObject
    {
        HELIUM_DECLARE_OBJECT( World, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        World();
        virtual ~World();
        //@}

        /// @name World Initialization
        //@{
        virtual bool Initialize();
        virtual void Shutdown();
        //@}

        /// @name World Updating
        //@{
        virtual void UpdateGraphicsScene();
        //@}

        /// @name GameObject Interface
        //@{
        virtual void PreDestroy();
        //@}

        /// @name EntityDefinition Creation
        //@{
        virtual EntityDefinition* CreateEntity(
            Slice* pSlice, const GameObjectType* pType, const Simd::Vector3& rPosition = Simd::Vector3( 0.0f ),
            const Simd::Quat& rRotation = Simd::Quat::IDENTITY, const Simd::Vector3& rScale = Simd::Vector3( 1.0f ),
            EntityDefinition* pTemplate = NULL, Name name = NULL_NAME, bool bAssignInstanceIndex = true );
        virtual bool DestroyEntity( EntityDefinition* pEntity );
        //@}

        /// @name Slice Registration
        //@{
        virtual bool AddSlice( Slice* pSlice );
        virtual bool RemoveSlice( Slice* pSlice );

        inline size_t GetSliceCount() const;
        Slice* GetSlice( size_t index ) const;
        //@}

        /// @name Scene Access
        //@{
        GraphicsScene* GetGraphicsScene() const;
        //@}

    private:
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

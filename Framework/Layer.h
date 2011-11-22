//----------------------------------------------------------------------------------------------------------------------
// Layer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_LAYER_H
#define HELIUM_FRAMEWORK_LAYER_H

#include "Framework/Framework.h"
#include "Engine/GameObject.h"

#include "Foundation/Math/SimdQuat.h"
#include "Foundation/Math/SimdVector3.h"
#include "Framework/Entity.h"

namespace Helium
{
    HELIUM_DECLARE_PTR( Entity );
    HELIUM_DECLARE_WPTR( World );

    /// Container for entities.
    ///
    /// Layers typically fall under one of two categories.
    /// - Level layers.  The Level instance is either created from the package information and cached (in editor mode)
    ///   or loaded from the cache file (in runtime mode).  When in runtime mode, the loaded level is typically linked
    ///   directly to a world, with all of its entities attached when linked.
    /// - Dynamic layers.  These can be constructed in editor mode for use with special-case worlds (such as for a
    ///   preview window).  In runtime mode, dynamic layers can be created as necessary for various runtime-created
    ///   entities.
    class HELIUM_FRAMEWORK_API Layer : public GameObject
    {
        HELIUM_DECLARE_OBJECT( Layer, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        Layer();
        virtual ~Layer();
        //@}

        /// @name Serialization
        //@{
        //virtual void Serialize( Serializer& s );
        //@}

        /// @name Dynamic Package Binding
        //@{
        void BindPackage( Package* pPackage );
        inline Package* GetPackage() const;
        //@}

        /// @name Entity Creation
        //@{
        virtual Entity* CreateEntity(
            const GameObjectType* pType, const Simd::Vector3& rPosition = Simd::Vector3( 0.0f ),
            const Simd::Quat& rRotation = Simd::Quat::IDENTITY, const Simd::Vector3& rScale = Simd::Vector3( 1.0f ),
            Entity* pTemplate = NULL, Name name = NULL_NAME, bool bAssignInstanceIndex = true );
        virtual bool DestroyEntity( Entity* pEntity );
        //@}

        /// @name Entity Access
        //@{
        inline size_t GetEntityCount() const;
        inline Entity* GetEntity( size_t index ) const;
        //@}

        /// @name World Registration
        //@{
        inline const WorldWPtr& GetWorld() const;
        inline size_t GetWorldIndex() const;
        void SetWorldInfo( World* pWorld, size_t worldIndex );
        void SetWorldIndex( size_t worldIndex );
        void ClearWorldInfo();
        //@}

    private:
        /// Bound package.
        PackagePtr m_spPackage;
        /// Entities.
        DynArray< EntityPtr > m_entities;

        /// Layer world.
        WorldWPtr m_spWorld;
        /// Runtime index for the layer within its world.
        size_t m_worldIndex;

        /// @name Private Utility Functions
        //@{
        void AddPackageEntities();
        void StripNonPackageEntities();
        //@}
    };
}

#include "Framework/Layer.inl"

#endif  // HELIUM_FRAMEWORK_LAYER_H

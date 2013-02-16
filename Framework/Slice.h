//----------------------------------------------------------------------------------------------------------------------
// Slice.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_SLICE_H
#define HELIUM_FRAMEWORK_SLICE_H

#include "Framework/Framework.h"
#include "Engine/GameObject.h"

#include "MathSimd/Quat.h"
#include "MathSimd/Vector3.h"
#include "Framework/EntityDefinition.h"

namespace Helium
{
    class EntityDefinition;
    typedef Helium::StrongPtr< EntityDefinition > EntityPtr;
    typedef Helium::StrongPtr< const EntityDefinition > ConstEntityPtr;

    class World;
    typedef Helium::WeakPtr< World > WorldWPtr;
    typedef Helium::WeakPtr< const World > ConstWorldWPtr;

    /// Container for entities.
    ///
    /// Slices typically fall under one of two categories.
    /// - Level slices.  The Level instance is either created from the package information and cached (in editor mode)
    ///   or loaded from the cache file (in runtime mode).  When in runtime mode, the loaded level is typically linked
    ///   directly to a world, with all of its entities attached when linked.
    /// - Dynamic slices.  These can be constructed in editor mode for use with special-case worlds (such as for a
    ///   preview window).  In runtime mode, dynamic slices can be created as necessary for various runtime-created
    ///   entities.
    class HELIUM_FRAMEWORK_API Slice : public GameObject
    {
        HELIUM_DECLARE_OBJECT( Slice, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        Slice();
        virtual ~Slice();
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

        /// @name EntityDefinition Creation
        //@{
        virtual EntityDefinition* CreateEntity(
            const GameObjectType* pType, const Simd::Vector3& rPosition = Simd::Vector3( 0.0f ),
            const Simd::Quat& rRotation = Simd::Quat::IDENTITY, const Simd::Vector3& rScale = Simd::Vector3( 1.0f ),
            EntityDefinition* pTemplate = NULL, Name name = NULL_NAME, bool bAssignInstanceIndex = true );
        virtual bool DestroyEntity( EntityDefinition* pEntity );
        //@}

        /// @name EntityDefinition Access
        //@{
        inline size_t GetEntityCount() const;
        inline EntityDefinition* GetEntity( size_t index ) const;
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
        DynamicArray< EntityPtr > m_entities;

        /// Slice world.
        WorldWPtr m_spWorld;
        /// Runtime index for the slice within its world.
        size_t m_worldIndex;

        /// @name Private Utility Functions
        //@{
        void AddPackageEntities();
        void StripNonPackageEntities();
        //@}
    };
}

#include "Framework/Slice.inl"

#endif  // HELIUM_FRAMEWORK_SLICE_H

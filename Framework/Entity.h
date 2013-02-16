//----------------------------------------------------------------------------------------------------------------------
// Entity.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_ENTITY_H
#define HELIUM_FRAMEWORK_ENTITY_H

#include "Framework/Framework.h"
#include "Engine/GameObject.h"

#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"
#include "Framework/WorldManager.h"

namespace Helium
{
    class World;

    class Slice;
    typedef Helium::WeakPtr< Slice > SliceWPtr;
    typedef Helium::WeakPtr< const Slice > ConstSliceWPtr;

    class World;
    typedef Helium::WeakPtr< World > WorldWPtr;
    typedef Helium::WeakPtr< const World > ConstWorldWPtr;

    /// Base type for in-world entities.
    class HELIUM_FRAMEWORK_API Entity : public GameObject
    {
        HELIUM_DECLARE_OBJECT( Entity, GameObject );

    public:

        /// @name Construction/Destruction
        //@{
        Entity();
        virtual ~Entity();
        //@}

        //@{
        inline const Simd::Vector3& GetPosition() const;
        virtual void SetPosition( const Simd::Vector3& rPosition );

        inline const Simd::Quat& GetRotation() const;
        virtual void SetRotation( const Simd::Quat& rRotation );

        inline const Simd::Vector3& GetScale() const;
        virtual void SetScale( const Simd::Vector3& rScale );
        //@}

        /// @name Slice Registration
        //@{
        inline const SliceWPtr& GetSlice() const;
        inline size_t GetSliceIndex() const;
        void SetSliceInfo( Slice* pSlice, size_t sliceIndex );
        void SetSliceIndex( size_t sliceIndex );
        void ClearSliceInfo();

        WorldWPtr GetWorld() const;
        //@}

        virtual void PreUpdate(float dt);

    protected:

    private:
        /// Entity position.
        Simd::Vector3 m_position;
        /// Entity rotation.
        Simd::Quat m_rotation;
        /// Entity scale.
        Simd::Vector3 m_scale;

        /// Entity slice.
        SliceWPtr m_spSlice;
        /// Runtime index for the entity within its slice.
        size_t m_sliceIndex;
    };
}

#include "Framework/Entity.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_H

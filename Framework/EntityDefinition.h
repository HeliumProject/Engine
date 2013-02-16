//----------------------------------------------------------------------------------------------------------------------
// EntityDefinition.h
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

#include "Framework/ComponentDefinitionSet.h"

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
    class HELIUM_FRAMEWORK_API EntityDefinition : public GameObject
    {
        HELIUM_DECLARE_OBJECT( EntityDefinition, GameObject );

    public:

        /// @name Construction/Destruction
        //@{
        EntityDefinition();
        virtual ~EntityDefinition();
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
        /// EntityDefinition position.
        Simd::Vector3 m_position;
        /// EntityDefinition rotation.
        Simd::Quat m_rotation;
        /// EntityDefinition scale.
        Simd::Vector3 m_scale;

        /// EntityDefinition slice.
        SliceWPtr m_spSlice;
        /// Runtime index for the entity within its slice.
        size_t m_sliceIndex;

        ComponentDefinitionSet m_ComponentDefinitions;
    };

    class Entity : public Reflect::Object, Helium::Components::HasComponents
    {
    public:
        REFLECT_DECLARE_OBJECT(Helium::Entity, Helium::Reflect::Object);
        static void PopulateComposite( Reflect::Composite& comp );

    private:

    };
}

#include "Framework/EntityDefinition.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_H

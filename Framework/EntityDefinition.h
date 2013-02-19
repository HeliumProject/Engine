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
#include "Engine/GameObject.h"

#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"

#include "Framework/ComponentDefinitionSet.h"
#include "Framework/SliceDefinition.h"

namespace Helium
{
    class SliceDefinition;
    typedef Helium::WeakPtr< SliceDefinition > SliceDefinitionWPtr;
    typedef Helium::WeakPtr< const SliceDefinition > ConstSliceDefinitionWPtr;

    class Entity;
    typedef Helium::StrongPtr< Entity > EntityPtr;
    
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

        /// @name SliceDefinition Registration
        //@{
        inline const SliceDefinitionWPtr& GetSlice() const;
        inline size_t GetSliceIndex() const;
        void SetSliceInfo( SliceDefinition* pSlice, size_t sliceIndex );
        void SetSliceIndex( size_t sliceIndex );
        void ClearSliceInfo();
        //@}

        EntityPtr CreateEntity();
    protected:

    private:
        /// EntityDefinition position.
        Simd::Vector3 m_position;
        /// EntityDefinition rotation.
        Simd::Quat m_rotation;
        /// EntityDefinition scale.
        Simd::Vector3 m_scale;

        /// EntityDefinition slice.
        SliceDefinitionWPtr m_spSlice;
        /// Runtime index for the entity within its slice.
        size_t m_sliceIndex;

        ComponentDefinitionSetPtr m_ComponentDefinitions;
    };
    typedef Helium::StrongPtr<EntityDefinition> EntityDefinitionPtr;
}

#include "Framework/EntityDefinition.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_DEFINITION_H

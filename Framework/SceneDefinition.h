//----------------------------------------------------------------------------------------------------------------------
// SceneDefinition.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_SLICE_DEFINITION_H
#define HELIUM_FRAMEWORK_SLICE_DEFINITION_H

#include "Framework/Framework.h"

#include "Engine/Package.h"
#include "Engine/Asset.h"

#include "MathSimd/Quat.h"
#include "MathSimd/Vector3.h"
#include "Framework/EntityDefinition.h"

#include "Framework/World.h"

namespace Helium
{
    class EntityDefinition;
    typedef Helium::StrongPtr< EntityDefinition > EntityDefinitionPtr;
    typedef Helium::StrongPtr< const EntityDefinition > ConstEntityDefinitionPtr;

    class World;
    typedef Helium::WeakPtr< World > WorldWPtr;
    typedef Helium::WeakPtr< const World > ConstWorldWPtr;
    
    class SceneDefinition;
    typedef Helium::StrongPtr< SceneDefinition > SceneDefinitionPtr;
    typedef Helium::StrongPtr< const SceneDefinition > ConstSceneDefinitionPtr;

    class Slice;
    typedef Helium::StrongPtr< Slice > SlicePtr;
    typedef Helium::StrongPtr< const Slice > ConstSlicePtr;

    class Entity;

    /// Container for entities.
    ///
    /// Slices typically fall under one of two categories.
    /// - Level slices.  The Level instance is either created from the package information and cached (in editor mode)
    ///   or loaded from the cache file (in runtime mode).  When in runtime mode, the loaded level is typically linked
    ///   directly to a world, with all of its entities attached when linked.
    /// - Dynamic slices.  These can be constructed in editor mode for use with special-case worlds (such as for a
    ///   preview window).  In runtime mode, dynamic slices can be created as necessary for various runtime-created
    ///   entities.
    class HELIUM_FRAMEWORK_API SceneDefinition : public Asset
    {
        HELIUM_DECLARE_ASSET( SceneDefinition, Asset );

    public:
        /// @name Construction/Destruction
        //@{
        SceneDefinition();
        virtual ~SceneDefinition();
        //@}
        
        /// @name EntityDefinition Creation
        //@{
        virtual EntityDefinition* AddEntityDefinition(
            const AssetType* pType, const Simd::Vector3& rPosition = Simd::Vector3( 0.0f ),
            const Simd::Quat& rRotation = Simd::Quat::IDENTITY, const Simd::Vector3& rScale = Simd::Vector3( 1.0f ),
            EntityDefinition* pTemplate = NULL, Name name = NULL_NAME, bool bAssignInstanceIndex = true );
        virtual bool DestroyEntityDefinition( EntityDefinition* pEntity );
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

        /// @name EntityDefinition Access
        //@{
        inline size_t GetEntityDefinitionCount() const;
        inline EntityDefinition* GetEntityDefinition( size_t index ) const;
        //@}


    private:
        /// Bound package.
        PackagePtr m_spPackage;
        /// Entities.
        DynamicArray< EntityDefinitionPtr > m_entityDefinitions;

        /// @name Private Utility Functions
        //@{
        void AddPackageEntities();
        void StripNonPackageEntities();
        //@}
    };
    typedef Helium::StrongPtr<SceneDefinition> SceneDefinitionPtr;
}

#include "Framework/SceneDefinition.inl"

#endif  // HELIUM_FRAMEWORK_SLICE_H

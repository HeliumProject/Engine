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

#include "Platform/Math/Simd/Vector3.h"
#include "Platform/Math/Simd/Quat.h"
#include "Framework/FrameworkInterface.h"
#include "Framework/WorldManager.h"

namespace Helium
{
    class World;

    HELIUM_DECLARE_WPTR( Layer );
    HELIUM_DECLARE_WPTR( World );

    /// Base type for in-world entities.
    class HELIUM_FRAMEWORK_API Entity : public GameObject
    {
        HELIUM_DECLARE_OBJECT( Entity, GameObject );

    public:
        /// Valid entity update phase flags.
        enum EUpdatePhaseFlag
        {
            /// Entity needs PreUpdate()/PostUpdate() called during asynchronous update phases.
            UPDATE_PHASE_FLAG_ASYNCHRONOUS = ( 1 << 0 ),
            /// Entity needs SynchronousUpdate() called each frame (regardless of deferred work status).
            UPDATE_PHASE_FLAG_SYNCHRONOUS  = ( 1 << 1 )
        };

        /// Deferred entity work flags.
        enum EDeferredWorkFlag
        {
            /// Destroy this entity.
            DEFERRED_WORK_FLAG_DESTROY  = ( 1 << 0 ),
            /// Generic call to SynchronousUpdate() required (performed before any entity reattachment).
            DEFERRED_WORK_FLAG_UPDATE   = ( 1 << 1 ),
            /// Reattach this entity to the world.
            DEFERRED_WORK_FLAG_REATTACH = ( 1 << 2 ),
        };

        /// @name Construction/Destruction
        //@{
        Entity();
        virtual ~Entity();
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
        //@}

        /// @name Entity Registration
        //@{
        virtual void Attach();
        virtual void Detach();
        void DeferredReattach();
        //@}

        /// @name Entity Updating
        //@{
        virtual void PreUpdate( float32_t deltaSeconds );
        inline void CommitPendingDeferredWorkFlags();
        virtual void PostUpdate( float32_t deltaSeconds );
        virtual void SynchronousUpdate( float32_t deltaSeconds );

        inline uint32_t GetUpdatePhaseFlags() const;
        inline bool NeedsAsynchronousUpdate() const;
        inline bool NeedsSynchronousUpdate() const;

        inline void VerifySafety() const;
        inline void VerifySafety();
        inline void VerifySafetySelfOnly() const;
        inline void VerifySafetySelfOnly();

        inline uint32_t GetDeferredWorkFlags() const;
        inline void ApplyDeferredWorkFlags( uint32_t flags );
        inline void ClearDeferredWorkFlags();
        //@}

        /// @name Transform Data
        //@{
        inline const Simd::Vector3& GetPosition() const;
        virtual void SetPosition( const Simd::Vector3& rPosition );

        inline const Simd::Quat& GetRotation() const;
        virtual void SetRotation( const Simd::Quat& rRotation );

        inline const Simd::Vector3& GetScale() const;
        virtual void SetScale( const Simd::Vector3& rScale );
        //@}

        /// @name Layer Registration
        //@{
        inline const LayerWPtr& GetLayer() const;
        inline size_t GetLayerIndex() const;
        void SetLayerInfo( Layer* pLayer, size_t layerIndex );
        void SetLayerIndex( size_t layerIndex );
        void ClearLayerInfo();

        WorldWPtr GetWorld() const;
        //@}

    protected:
        /// @name Entity Updating Support
        //@{
        void SetUpdatePhaseFlags( uint32_t flags );

        inline uint32_t GetPendingDeferredWorkFlags() const;
        inline void ApplyPendingDeferredWorkFlags( uint32_t flags );
        inline void ClearPendingDeferredWorkFlags();
        //@}

    private:
        /// Entity position.
        Simd::Vector3 m_position;
        /// Entity rotation.
        Simd::Quat m_rotation;
        /// Entity scale.
        Simd::Vector3 m_scale;

        /// Entity layer.
        LayerWPtr m_spLayer;
        /// Runtime index for the entity within its layer.
        size_t m_layerIndex;

        /// Required update phase flags.
        uint32_t m_updatePhaseFlags;

        /// Pending deferred work flags (can be set during read-only updates without the need for message passing).
        uint32_t m_pendingDeferredWorkFlags;
        /// Deferred work flags.
        uint32_t m_deferredWorkFlags;
    };
}

#include "Framework/Entity.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_H

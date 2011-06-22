//----------------------------------------------------------------------------------------------------------------------
// Entity.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Commit pending deferred work flags for this entity.
    ///
    /// This should only be called by the WorldManager during the post-update phase.
    void Entity::CommitPendingDeferredWorkFlags()
    {
        VerifySafety();

        m_deferredWorkFlags |= m_pendingDeferredWorkFlags;
        m_pendingDeferredWorkFlags = 0;
    }

    /// Get the flags specifying the update phases in which this entity requires processing.
    ///
    /// @return  Required update phase flags for this entity.
    ///
    /// @see NeedsAsynchronousUpdate(), NeedsSynchronousUpdate()
    uint32_t Entity::GetUpdatePhaseFlags() const
    {
        VerifySafety();
        return m_updatePhaseFlags;
    }

    /// Get whether this entity should be updated during asynchronous updates.
    ///
    /// @return  True if this entity should be updated during asynchronous updates, false if not.
    ///
    /// @see NeedsSynchronousUpdate(), GetUpdatePhaseFlags()
    bool Entity::NeedsAsynchronousUpdate() const
    {
        VerifySafety();
        return ( ( m_updatePhaseFlags & UPDATE_PHASE_FLAG_ASYNCHRONOUS ) != 0 );
    }

    /// Get whether this entity should be updated during synchronous updates.
    ///
    /// This checks the current set of deferred work flags as well as the standard set of update phase flags.
    ///
    /// @see NeedsAsynchronousUpdate(), GetUpdatePhaseFlags(), GetDeferredWorkFlags()
    bool Entity::NeedsSynchronousUpdate() const
    {
        VerifySafety();

        uint32_t combinedFlags = ( ( m_updatePhaseFlags & UPDATE_PHASE_FLAG_SYNCHRONOUS ) |
            ( m_deferredWorkFlags & DEFERRED_WORK_FLAG_UPDATE ) );

        return ( combinedFlags != 0 );
    }

    /// Verify the safety of accessing this entity from its current context.
    ///
    /// This will verify that this entity can be accessed in a read-only fashion from any entity's update.
    void Entity::VerifySafety() const
    {
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
        WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();
        if( updatePhase == WorldManager::UPDATE_PHASE_POST )
        {
            const Entity* pCurrentEntity = rWorldManager.GetCurrentThreadUpdateEntity();
            HELIUM_ASSERT_MSG(
                pCurrentEntity == this,
                ( TXT( "Can only access the entity being processed by the current thread during the post-update " )
                  TXT( "phase." ) ) );
            if( pCurrentEntity != this )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Accessed entity other than the entity currently being updated by the current thread " )
                      TXT( "during the post-update phase.\n" ) ) );
            }
        }
#endif
    }

    /// Verify the safety of accessing this entity from its current context.
    ///
    /// This will verify that this entity can be accessed in a write-allowed fashion from any entity's update.
    void Entity::VerifySafety()
    {
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
        WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();
        HELIUM_ASSERT_MSG(
            updatePhase != WorldManager::UPDATE_PHASE_PRE,
            TXT( "Can only perform read-only access to entities during the pre-update phase." ) );
        if( updatePhase == WorldManager::UPDATE_PHASE_PRE )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Accessed entity with write-allowed semantics during pre-update phase.\n" ) );
        }
        else if( updatePhase == WorldManager::UPDATE_PHASE_POST )
        {
            const Entity* pCurrentEntity = rWorldManager.GetCurrentThreadUpdateEntity();
            HELIUM_ASSERT_MSG(
                pCurrentEntity == this,
                ( TXT( "Can only access the entity being processed by the current thread during the post-update " )
                  TXT( "phase." ) ) );
            if( pCurrentEntity != this )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Accessed entity other than the entity currently being updated by the current thread " )
                      TXT( "during the post-update phase.\n" ) ) );
            }
        }
#endif
    }

    /// Verify the safety of accessing this entity for calls that should only be allowed during an entity's own update.
    ///
    /// This will verify that this entity can be accessed in a read-only or private write-allowed fashion from its own
    /// update only.
    void Entity::VerifySafetySelfOnly() const
    {
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
        WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();
        if( updatePhase != WorldManager::UPDATE_PHASE_INVALID )
        {
            const Entity* pCurrentEntity = rWorldManager.GetCurrentThreadUpdateEntity();
            HELIUM_ASSERT_MSG(
                pCurrentEntity == this,
                TXT( "Call can only be used on the entity being updated by the current thread." ) );
            if( pCurrentEntity != this )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Call made to method that can only be used on the entity being updated by the current " )
                      TXT( "thread.\n" ) ) );
            }
        }
#endif
    }

    /// Verify the safety of accessing this entity for calls that should only be allowed during an entity's own update.
    ///
    /// This will verify that this entity can be accessed in a write-allowed fashion from its own update only.
    void Entity::VerifySafetySelfOnly()
    {
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        WorldManager& rWorldManager = WorldManager::GetStaticInstance();
        WorldManager::EUpdatePhase updatePhase = rWorldManager.GetUpdatePhase();
        HELIUM_ASSERT_MSG(
            updatePhase != WorldManager::UPDATE_PHASE_PRE,
            TXT( "Can only perform read-only access to entities during the pre-update phase." ) );
        if( updatePhase == WorldManager::UPDATE_PHASE_PRE )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "Accessed entity with write-allowed semantics during pre-update phase.\n" ) );
        }
        else if( updatePhase != WorldManager::UPDATE_PHASE_INVALID )
        {
            const Entity* pCurrentEntity = rWorldManager.GetCurrentThreadUpdateEntity();
            HELIUM_ASSERT_MSG(
                pCurrentEntity == this,
                TXT( "Call can only be used on the entity being updated by the current thread." ) );
            if( pCurrentEntity != this )
            {
                HELIUM_TRACE(
                    TRACE_ERROR,
                    ( TXT( "Call made to method that can only be used on the entity being updated by the current " )
                      TXT( "thread.\n" ) ) );
            }
        }
#endif
    }

    /// Get the current set of deferred work flags.
    ///
    /// @return  Deferred work flags for this entity.
    ///
    /// @see ApplyDeferredWorkFlags(), ClearDeferredWorkFlags()
    uint32_t Entity::GetDeferredWorkFlags() const
    {
        VerifySafety();
        return m_deferredWorkFlags;
    }

    /// Set deferred work flags for this entity.
    ///
    /// Flags that are currently set for this entity when calling this function will be preserved.
    ///
    /// @param[in] flags  Deferred work flags to set.
    ///
    /// @see GetDeferredWorkFlags(), ClearDeferredWorkFlags()
    void Entity::ApplyDeferredWorkFlags( uint32_t flags )
    {
        VerifySafety();
        m_deferredWorkFlags |= flags;
    }

    /// Clear the set of deferred work flags for this entity.
    ///
    /// @see GetDeferredWorkFlags(), ApplyDeferredWorkFlags()
    void Entity::ClearDeferredWorkFlags()
    {
        VerifySafety();
        m_deferredWorkFlags = 0;
    }

    /// Get the entity position.
    ///
    /// @return  Current position.
    ///
    /// @see GetRotation(), GetScale()
    const Simd::Vector3& Entity::GetPosition() const
    {
        VerifySafety();
        return m_position;
    }

    /// Get the entity rotation.
    ///
    /// @return  Current rotation.
    ///
    /// @see GetPosition(), GetScale()
    const Simd::Quat& Entity::GetRotation() const
    {
        VerifySafety();
        return m_rotation;
    }

    /// Get the entity scale.
    ///
    /// @return  Current scale.
    ///
    /// @see GetPosition(), GetRotation()
    const Simd::Vector3& Entity::GetScale() const
    {
        VerifySafety();
        return m_scale;
    }

    /// Get the layer to which this entity is currently bound.
    ///
    /// @return  Entity layer.
    ///
    /// @see GetLayerIndex(), SetLayerInfo(), SetLayerIndex(), ClearLayerInfo()
    const LayerWPtr& Entity::GetLayer() const
    {
        VerifySafety();
        return m_spLayer;
    }

    /// Get the index of this entity in the layer to which it is currently bound.
    ///
    /// Note that this index may change at runtime, so it should not be used to track an entity.
    ///
    /// @return  Entity index within its layer.
    ///
    /// @see GetLayer(), SetLayerInfo(), SetLayerIndex(), ClearLayerInfo()
    size_t Entity::GetLayerIndex() const
    {
        VerifySafety();
        return m_layerIndex;
    }

    /// Get the set of deferred work flags that will be applied during the post-update phase.
    ///
    /// Pending deferred work flags will be OR'd with the current set of deferred work flags immediately prior to
    /// post-update message processing.
    ///
    /// @return  Pending set of deferred work flags.
    ///
    /// @see ApplyPendingDeferredWorkFlags(), ClearPendingDeferredWorkFlags(), GetDeferredWorkFlags()
    uint32_t Entity::GetPendingDeferredWorkFlags() const
    {
        return m_pendingDeferredWorkFlags;
    }

    /// Set pending deferred work flags for this entity.
    ///
    /// Pending deferred work flags can be modified during the pre-update phase, as they cannot be read by other
    /// entities.  Pending flags that are currently set for this entity when calling this function will be preserved.
    ///
    /// @param[in] flags  Deferred work flags to set.
    ///
    /// @see GetPendingDeferredWorkFlags(), ClearPendingDeferredWorkFlags(), GetDeferredWorkFlags()
    void Entity::ApplyPendingDeferredWorkFlags( uint32_t flags )
    {
        m_pendingDeferredWorkFlags |= flags;
    }

    /// Clear the set of pending deferred work flags for this entity.
    ///
    /// Pending deferred work flags can be modified during the pre-update phase, as they cannot be read by other
    /// entities.
    ///
    /// @see GetPendingDeferredWorkFlags(), ApplyPendingDeferredWorkFlags(), GetDeferredWorkFlags()
    void Entity::ClearPendingDeferredWorkFlags()
    {
        m_pendingDeferredWorkFlags = 0;
    }
}

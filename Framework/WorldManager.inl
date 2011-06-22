//----------------------------------------------------------------------------------------------------------------------
// WorldManager.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the elapsed world timer tick count for the start of the current frame, adjusted for frame rate limits.
    ///
    /// Ticks are expressed in units determined by the Timer class.  Conversion between ticks and seconds can be
    /// performed using Timer::GetTicksPerSecond() and Timer::GetSecondsPerTick().
    ///
    /// @return  Elapsed ticks, adjusted for frame rate limits.
    ///
    /// @see GetFrameDeltaTickCount(), GetFrameDeltaSeconds()
    uint64_t WorldManager::GetFrameTickCount() const
    {
        return m_frameTickCount;
    }

    /// Get the number of timer ticks elapsed since the previous frame, adjusted for frame rate limits.
    ///
    /// Ticks are expressed in units determined by the Timer class.  Conversion between ticks and seconds can be
    /// performed using Timer::GetTicksPerSecond() and Timer::GetSecondsPerTick().
    ///
    /// @return  Timer ticks since the previous frame, adjusted for frame rate limits.
    ///
    /// @see GetFrameTickCount(), GetFrameDeltaSeconds()
    uint64_t WorldManager::GetFrameDeltaTickCount() const
    {
        return m_frameDeltaTickCount;
    }

    /// Get the number of seconds elapsed since the previous frame, adjusted for frame rate limits.
    ///
    /// @return  Seconds since the previous frame, adjusted for frame rate limits.
    ///
    /// @see GetFrameTickCount(), GetFrameDeltaTickCount()
    float32_t WorldManager::GetFrameDeltaSeconds() const
    {
        return m_frameDeltaSeconds;
    }

    /// Get the current entity update phase.
    ///
    /// @return  Current entity update phase.
    WorldManager::EUpdatePhase WorldManager::GetUpdatePhase() const
    {
        return m_updatePhase;
    }

#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    /// Get the entity being updated by the current thread.
    ///
    /// @return  Current entity being updated.
    ///
    /// @see SetCurrentThreadUpdateEntity()
    const Entity* WorldManager::GetCurrentThreadUpdateEntity() const
    {
        const Entity* pEntity = static_cast< const Entity* >( m_currentEntityTls.GetPointer() );

        return pEntity;
    }

    /// Set the entity being updated by the current thread.
    ///
    /// Note that only the pointer value is stored, so the reference count of the entity will not be increased.
    ///
    /// @param[in] pEntity  Entity to set.
    ///
    /// @see GetCurrentThreadUpdateEntity()
    void WorldManager::SetCurrentThreadUpdateEntity( const Entity* pEntity )
    {
        m_currentEntityTls.SetPointer( const_cast< Entity* >( pEntity ) );
    }
#endif
}

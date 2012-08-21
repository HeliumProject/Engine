//----------------------------------------------------------------------------------------------------------------------
// WorldManager.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_WORLD_MANAGER_H
#define HELIUM_FRAMEWORK_WORLD_MANAGER_H

#include "Framework/World.h"

#include "Platform/Thread.h"

/// Non-zero to enable debug verification of methods called on Entity-based instances during world updates.
#define HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING ( !HELIUM_RELEASE )

namespace Helium
{
    class World;
    typedef Helium::StrongPtr< World > WorldPtr;
    typedef Helium::StrongPtr< const World > ConstWorldPtr;

    /// Manager for individual World instances.
    class HELIUM_FRAMEWORK_API WorldManager : NonCopyable
    {
    public:
        /// Update phases.
        enum EUpdatePhase
        {
            UPDATE_PHASE_FIRST   =  0,
            UPDATE_PHASE_INVALID = -1,

            /// Pre-update (entities can read data, write only to private scratch space, can access other entities).
            UPDATE_PHASE_PRE,
            /// Post-update (entities can only read and write their own data, cannot access other entities).
            UPDATE_PHASE_POST,
            /// Synchronous update (slow, necessary for object creation, deletion, and entity reattachment).
            UPDATE_PHASE_SYNCHRONOUS,

            UPDATE_PHASE_MAX,
            UPDATE_PHASE_LAST = UPDATE_PHASE_MAX - 1
        };

        /// @name Initialization
        //@{
        bool Initialize();
        void Shutdown();
        //@}

        /// @name World Creation
        //@{
        GameObjectPath GetWorldPackagePath() const;
        Package* GetWorldPackage() const;

        Name GetDefaultWorldName() const;

        World* CreateDefaultWorld( const GameObjectType* pType = World::GetStaticType() );
        //@}

        /// @name Updating
        //@{
        void Update();
        //@}

        /// @name Timing
        //@{
        inline uint64_t GetFrameTickCount() const;
        inline uint64_t GetFrameDeltaTickCount() const;
        inline float32_t GetFrameDeltaSeconds() const;
        //@}

        /// @name Data Access
        //@{
        inline EUpdatePhase GetUpdatePhase() const;
#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        inline const Entity* GetCurrentThreadUpdateEntity() const;
        inline void SetCurrentThreadUpdateEntity( const Entity* pEntity );
#endif
        //@}

        /// @name Static Access
        //@{
        static WorldManager& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

    private:
        /// World package.
        PackagePtr m_spWorldPackage;
        /// World instances.
        DynArray< WorldPtr > m_worlds;

        /// Actual application tick count at the start of the current frame.
        uint64_t m_actualFrameTickCount;
        /// Elapsed tick count for the current frame (adjusted for frame rate limits).
        uint64_t m_frameTickCount;
        /// Ticks since the previous frame (adjusted for frame rate limits).
        uint64_t m_frameDeltaTickCount;
        /// Seconds elapsed since the previous frame (adjusted for frame rate limits).
        float32_t m_frameDeltaSeconds;

        /// Current world update phase.
        EUpdatePhase m_updatePhase;

#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
        /// Thread-local storage for the entity currently being updated on a given thread.
        ThreadLocalPointer m_currentEntityTls;
#endif

        /// True if the first frame has been processed.
        bool m_bProcessedFirstFrame;

        /// Singleton instance.
        static WorldManager* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        WorldManager();
        ~WorldManager();
        //@}

        /// @name Time Updating
        //@{
        void UpdateTime();
        //@}
    };
}

#include "Framework/WorldManager.inl"

#endif  // HELIUM_FRAMEWORK_WORLD_MANAGER_H

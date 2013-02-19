//----------------------------------------------------------------------------------------------------------------------
// WorldManager.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/WorldManager.h"

#include "Platform/Timer.h"
#include "Engine/JobContext.h"
#include "Framework/FrameworkInterface.h"
#include "Framework/Slice.h"

using namespace Helium;

WorldManager* WorldManager::sm_pInstance = NULL;

/// Constructor.
WorldManager::WorldManager()
: m_actualFrameTickCount( 0 )
, m_frameTickCount( 0 )
, m_frameDeltaTickCount( 0 )
, m_frameDeltaSeconds( 0.0f )
, m_bProcessedFirstFrame( false )
{
}

/// Destructor.
WorldManager::~WorldManager()
{
}

/// Initialize this manager.
///
/// @return  True if this manager was initialized successfully, false if not.
///
/// @see Shutdown()
bool WorldManager::Initialize()
{
    // Reset frame timings.
    m_actualFrameTickCount = 0;
    m_frameTickCount = 0;
    m_frameDeltaTickCount = 0;
    m_frameDeltaSeconds = 0.0f;

    // First frame still needs to be processed.
    m_bProcessedFirstFrame = false;

    return true;
}

/// Shut down the world manager, detaching all world instances and their slices.
///
/// @see Initialize()
void WorldManager::Shutdown()
{
    size_t worldCount = m_worlds.GetSize();
    for( size_t worldIndex = 0; worldIndex < worldCount; ++worldIndex )
    {
        World* pWorld = m_worlds[ worldIndex ];
        HELIUM_ASSERT( pWorld );
        pWorld->Shutdown();
    }

    m_worlds.Clear();
}

/// Create the default world instance.
///
/// @param[in] pType  World type.
///
/// @return  Default world instance.
Helium::World *WorldManager::CreateWorld( WorldDefinitionPtr _world_definition )
{
    WorldPtr world = Reflect::AssertCast<World>(World::CreateObject());
    if (world->Initialize(_world_definition))
    {
        m_worlds.Push( world );
        return world;
    }

    world.Release();
    return world;
}

/// Update all worlds for the current frame.
void WorldManager::Update()
{
    // Update the world time.
    UpdateTime();

#if 0

    // Perform the entity pre-update.
    m_updatePhase = UPDATE_PHASE_PRE;
    {
        JobContext::Spawner< 1 > entityUpdateSpawner;
        JobContext* pContext = entityUpdateSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        WorldManagerPreUpdate* pJob = pContext->Create< WorldManagerPreUpdate >();
        HELIUM_ASSERT( pJob );
        WorldManagerPreUpdate::Parameters& rParameters = pJob->GetParameters();
        rParameters.pspWorlds = m_worlds.GetData();
        rParameters.worldCount = m_worlds.GetSize();
    }

    // Perform the entity post-update.
    m_updatePhase = UPDATE_PHASE_POST;
    {
        JobContext::Spawner< 1 > entityUpdateSpawner;
        JobContext* pContext = entityUpdateSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        WorldManagerPostUpdate* pJob = pContext->Create< WorldManagerPostUpdate >();
        HELIUM_ASSERT( pJob );
        WorldManagerPostUpdate::Parameters& rParameters = pJob->GetParameters();
        rParameters.pspWorlds = m_worlds.GetData();
        rParameters.worldCount = m_worlds.GetSize();
    }

    // Perform the entity synchronous update.
    m_updatePhase = UPDATE_PHASE_SYNCHRONOUS;

    size_t worldCount = m_worlds.GetSize();
    for( size_t worldIndex = 0; worldIndex < worldCount; ++worldIndex )
    {
        World* pWorld = m_worlds[ worldIndex ];
        HELIUM_ASSERT( pWorld );
        size_t sliceCount = pWorld->GetSliceCount();
        for( size_t sliceIndex = 0; sliceIndex < sliceCount; ++sliceIndex )
        {
            Slice* pSlice = pWorld->GetSlice( sliceIndex );
            HELIUM_ASSERT( pSlice );
            size_t entityCount = pSlice->GetEntityCount();
            for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
            {
                EntityDefinition* pEntity = pSlice->GetEntity( entityIndex );
                HELIUM_ASSERT( pEntity );

#if 0
                bool bNeedsSynchronousUpdate = pEntity->NeedsSynchronousUpdate();
                uint32_t deferredWorkFlags = pEntity->GetDeferredWorkFlags();
                if( !bNeedsSynchronousUpdate && !deferredWorkFlags )
                {
                    continue;
                }

                if( deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_DESTROY )
                {
                    pSlice->DestroyEntity( pEntity );
                    --entityIndex;

                    // Only the current entity should be destroyed; entity destruction should not trigger the
                    // immediate creation or destruction of other entities.
                    --entityCount;
                    HELIUM_ASSERT( pSlice->GetEntityCount() == entityCount );

                    continue;
                }

                // Entity::NeedsSynchronousUpdate() also checks the deferred work flags.
                if( bNeedsSynchronousUpdate /*deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_UPDATE*/ )
                {
                    pEntity->SynchronousUpdate( m_frameDeltaSeconds );

                    // Update the entity count in case a new entity was created during the synchronous update.
                    entityCount = pSlice->GetEntityCount();
                    HELIUM_ASSERT( entityIndex < entityCount );

                    // Update the deferred work flags and reassess entity destruction after the synchronous update.
                    deferredWorkFlags = pEntity->GetDeferredWorkFlags();

                    if( deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_DESTROY )
                    {
                        pSlice->DestroyEntity( pEntity );
                        --entityIndex;

                        // Only the current entity should be destroyed; entity destruction should not trigger the
                        // immediate creation or destruction of other entities.
                        --entityCount;
                        HELIUM_ASSERT( pSlice->GetEntityCount() == entityCount );

                        continue;
                    }
                }

                if( deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_REATTACH )
                {
                    pEntity->Detach();
                    pEntity->Attach();

                    // Entities should only be spawned during the synchronous update call, so the count should not
                    // have changed as a result of reattachment.
                    HELIUM_ASSERT( pSlice->GetEntityCount() == entityCount );
                }

                pEntity->ClearDeferredWorkFlags();
#endif
            }
        }
    }

    m_updatePhase = UPDATE_PHASE_INVALID;

#endif
    
    // Update the graphics scene for each world.
    for( size_t worldIndex = 0; worldIndex < m_worlds.GetSize(); ++worldIndex )
    {
        World* pWorld = m_worlds[ worldIndex ];
        HELIUM_ASSERT( pWorld );
        pWorld->UpdateGraphicsScene();
    }
}

/// Get the singleton WorldManager instance, creating it if necessary.
///
/// @return  Reference to the WorldManager instance.
///
/// @see DestroyStaticInstance()
WorldManager& WorldManager::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new WorldManager;
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

/// Destroy the singleton WorldManager instance.
///
/// This also handles calling Shutdown() on the WorldManager instance if one exists.
///
/// @see GetStaticInstance()
void WorldManager::DestroyStaticInstance()
{
    if( sm_pInstance )
    {
        sm_pInstance->Shutdown();
        delete sm_pInstance;
        sm_pInstance = NULL;
    }
}

/// Update timer information for the current frame.
void WorldManager::UpdateTime()
{
    // If this is the first frame, initialize the timer.
    if( !m_bProcessedFirstFrame )
    {
        m_actualFrameTickCount = Timer::GetTickCount();
        m_frameTickCount = 0;
        m_frameDeltaTickCount = 0;
        m_frameDeltaSeconds = 0.0f;

        m_bProcessedFirstFrame = true;

        return;
    }

    // Get the actual number of ticks elapsed since the previous frame.
    uint64_t newFrameTickCount = Timer::GetTickCount();
    uint64_t deltaTickCount = newFrameTickCount - m_actualFrameTickCount;
    m_actualFrameTickCount = newFrameTickCount;

    // Clamp the timer delta based on the timer limit settings.
    if( deltaTickCount == 0 )
    {
        deltaTickCount = 1;
    }
    else
    {
        uint64_t tickLimit =
            static_cast< uint64_t >( 0.4 * static_cast< float64_t >( Timer::GetTicksPerSecond() ) );
        if( deltaTickCount > tickLimit )
        {
            deltaTickCount = tickLimit;
        }
    }

    // Update the clamped time values.
    m_frameTickCount += deltaTickCount;
    m_frameDeltaTickCount = deltaTickCount;
    m_frameDeltaSeconds =
        static_cast< float32_t >( static_cast< float64_t >( deltaTickCount ) * Timer::GetSecondsPerTick() );
}

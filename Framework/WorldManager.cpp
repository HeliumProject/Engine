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
#include "Framework/Layer.h"

using namespace Lunar;

WorldManager* WorldManager::sm_pInstance = NULL;

/// Constructor.
WorldManager::WorldManager()
: m_actualFrameTickCount( 0 )
, m_frameTickCount( 0 )
, m_frameDeltaTickCount( 0 )
, m_frameDeltaSeconds( 0.0f )
, m_updatePhase( UPDATE_PHASE_INVALID )
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
    HELIUM_ASSERT( !m_spWorldPackage );

    // Create the world package first.
    // XXX TMC: Note that we currently assume that the world package has no parents, so we don't need to handle
    // recursive package creation.  If we want to move the world package to a subpackage, this will need to be
    // updated accordingly.
    GameObjectPath worldPackagePath = GetWorldPackagePath();
    HELIUM_ASSERT( !worldPackagePath.IsEmpty() );
    HELIUM_ASSERT( worldPackagePath.GetParent().IsEmpty() );
    bool bCreateResult = GameObject::Create< Package >( m_spWorldPackage, worldPackagePath.GetName(), NULL );
    HELIUM_ASSERT( bCreateResult );
    if( !bCreateResult )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "WorldManager::Initialize(): Failed to create world package \"%s\".\n" ),
            *worldPackagePath.ToString() );

        return false;
    }

    HELIUM_ASSERT( m_spWorldPackage );

    // Reset frame timings.
    m_actualFrameTickCount = 0;
    m_frameTickCount = 0;
    m_frameDeltaTickCount = 0;
    m_frameDeltaSeconds = 0.0f;

    // First frame still needs to be processed.
    m_bProcessedFirstFrame = false;

    return true;
}

/// Shut down the world manager, detaching all world instances and their layers.
///
/// @see Initialize()
void WorldManager::Shutdown()
{
#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    m_currentEntityTls.SetPointer( NULL );
#endif

    size_t worldCount = m_worlds.GetSize();
    for( size_t worldIndex = 0; worldIndex < worldCount; ++worldIndex )
    {
        World* pWorld = m_worlds[ worldIndex ];
        HELIUM_ASSERT( pWorld );
        pWorld->Shutdown();
    }

    m_worlds.Clear();
    m_spWorldPackage.Release();
}

/// Get the path to the package containing all world instances.
///
/// @return  World package path.
GameObjectPath WorldManager::GetWorldPackagePath() const
{
    static GameObjectPath worldPackagePath;
    if( worldPackagePath.IsEmpty() )
    {
        HELIUM_VERIFY( worldPackagePath.Set( TXT( "/Worlds" ) ) );
    }

    return worldPackagePath;
}

/// Get the instance of the package containing all world instances.
///
/// @return  World package instance.
Package* WorldManager::GetWorldPackage() const
{
    return m_spWorldPackage;
}

/// Get the name assigned to the default world instance.
///
/// @return  Default world instance name.
Name WorldManager::GetDefaultWorldName() const
{
    static Name defaultWorldName( TXT( "Default" ) );

    return defaultWorldName;
}

/// Create the default world instance.
///
/// @param[in] pType  World type.
///
/// @return  Default world instance.
World* WorldManager::CreateDefaultWorld( const GameObjectType* pType )
{
    HELIUM_ASSERT( pType );
    if( !pType )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "WorldManager::CreateDefaultWorld(): No world type specified.\n" ) );

        return NULL;
    }

    bool bIsWorldType = pType->IsType( World::GetStaticType() );
    HELIUM_ASSERT( bIsWorldType );
    if( !bIsWorldType )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "WorldManager::CreateDefaultWorld(): Type \"%s\" specified is not a World subtype.\n" ),
            *pType->GetName() );

        return NULL;
    }

    HELIUM_ASSERT( m_spWorldPackage );
    if( !m_spWorldPackage )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "WorldManager::CreateDefaultWorld(): World manager has not been initialized.\n" ) );

        return NULL;
    }

    HELIUM_ASSERT( m_worlds.IsEmpty() );
    if( !m_worlds.IsEmpty() )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "WorldManager::CreateDefaultWorld(): Default world has already been created.\n" ) );

        return NULL;
    }

    GameObjectPtr spDefaultWorldObject;
    bool bCreateResult = GameObject::CreateObject(
        spDefaultWorldObject,
        pType,
        GetDefaultWorldName(),
        m_spWorldPackage );
    HELIUM_ASSERT( bCreateResult );
    if( !bCreateResult )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "WorldManager::CreateDefaultWorld(): Failed to create world of type \"%s\".\n" ),
            *pType->GetName() );

        return NULL;
    }

    WorldPtr spDefaultWorld( StaticCast< World >( spDefaultWorldObject.Get() ) );
    HELIUM_ASSERT( spDefaultWorld );

    m_worlds.Push( spDefaultWorld );

    return spDefaultWorld;
}

/// Update all worlds for the current frame.
void WorldManager::Update()
{
    // Update the world time.
    UpdateTime();

    // Perform the entity pre-update.
    m_updatePhase = UPDATE_PHASE_PRE;
    {
        JobContext::Spawner< 1 > entityUpdateSpawner;
        JobContext* pContext = entityUpdateSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        WorldManagerUpdate< EntityPreUpdate >* pJob = pContext->Create< WorldManagerUpdate< EntityPreUpdate > >();
        HELIUM_ASSERT( pJob );
        WorldManagerUpdate< EntityPreUpdate >::Parameters& rParameters = pJob->GetParameters();
        rParameters.pspWorlds = m_worlds.GetData();
        rParameters.worldCount = m_worlds.GetSize();
    }

    // Perform the entity post-update.
    m_updatePhase = UPDATE_PHASE_POST;
    {
        JobContext::Spawner< 1 > entityUpdateSpawner;
        JobContext* pContext = entityUpdateSpawner.Allocate();
        HELIUM_ASSERT( pContext );
        WorldManagerUpdate< EntityPostUpdate >* pJob = pContext->Create< WorldManagerUpdate< EntityPostUpdate > >();
        HELIUM_ASSERT( pJob );
        WorldManagerUpdate< EntityPostUpdate >::Parameters& rParameters = pJob->GetParameters();
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
        size_t layerCount = pWorld->GetLayerCount();
        for( size_t layerIndex = 0; layerIndex < layerCount; ++layerIndex )
        {
            Layer* pLayer = pWorld->GetLayer( layerIndex );
            HELIUM_ASSERT( pLayer );
            size_t entityCount = pLayer->GetEntityCount();
            for( size_t entityIndex = 0; entityIndex < entityCount; ++entityIndex )
            {
                Entity* pEntity = pLayer->GetEntity( entityIndex );
                HELIUM_ASSERT( pEntity );

                bool bNeedsSynchronousUpdate = pEntity->NeedsSynchronousUpdate();
                uint32_t deferredWorkFlags = pEntity->GetDeferredWorkFlags();
                if( !bNeedsSynchronousUpdate && !deferredWorkFlags )
                {
                    continue;
                }

                if( deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_DESTROY )
                {
                    pLayer->DestroyEntity( pEntity );
                    --entityIndex;

                    // Only the current entity should be destroyed; entity destruction should not trigger the
                    // immediate creation or destruction of other entities.
                    --entityCount;
                    HELIUM_ASSERT( pLayer->GetEntityCount() == entityCount );

                    continue;
                }

                // Entity::NeedsSynchronousUpdate() also checks the deferred work flags.
                if( bNeedsSynchronousUpdate /*deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_UPDATE*/ )
                {
                    pEntity->SynchronousUpdate( m_frameDeltaSeconds );

                    // Update the entity count in case a new entity was created during the synchronous update.
                    entityCount = pLayer->GetEntityCount();
                    HELIUM_ASSERT( entityIndex < entityCount );

                    // Update the deferred work flags and reassess entity destruction after the synchronous update.
                    deferredWorkFlags = pEntity->GetDeferredWorkFlags();

                    if( deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_DESTROY )
                    {
                        pLayer->DestroyEntity( pEntity );
                        --entityIndex;

                        // Only the current entity should be destroyed; entity destruction should not trigger the
                        // immediate creation or destruction of other entities.
                        --entityCount;
                        HELIUM_ASSERT( pLayer->GetEntityCount() == entityCount );

                        continue;
                    }
                }

                if( deferredWorkFlags & Entity::DEFERRED_WORK_FLAG_REATTACH )
                {
                    pEntity->Detach();
                    pEntity->Attach();

                    // Entities should only be spawned during the synchronous update call, so the count should not
                    // have changed as a result of reattachment.
                    HELIUM_ASSERT( pLayer->GetEntityCount() == entityCount );
                }

                pEntity->ClearDeferredWorkFlags();
            }
        }
    }

    m_updatePhase = UPDATE_PHASE_INVALID;

    // Update the graphics scene for each world.
    for( size_t worldIndex = 0; worldIndex < worldCount; ++worldIndex )
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

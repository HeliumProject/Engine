//----------------------------------------------------------------------------------------------------------------------
// WorldManager.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/WorldManager.h"
#include "Framework/WorldDefinition.h"

#include "Platform/Timer.h"
#include "Engine/JobContext.h"
#include "Framework/FrameworkInterface.h"
#include "Framework/Slice.h"
#include "Framework/Entity.h"
#include "Framework/SceneDefinition.h"
#include "Framework/TaskScheduler.h"

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
	HELIUM_ASSERT( !m_spRootSceneDefinitionsPackage );

	// Create the world package first.
	// XXX TMC: Note that we currently assume that the world package has no parents, so we don't need to handle
	// recursive package creation.  If we want to move the world package to a subpackage, this will need to be
	// updated accordingly.
	AssetPath rootSceneDefinitionsPackagePath = GetRootSceneDefinitionPackagePath();
	HELIUM_ASSERT( !rootSceneDefinitionsPackagePath.IsEmpty() );
	HELIUM_ASSERT( rootSceneDefinitionsPackagePath.GetParent().IsEmpty() );
	bool bCreateResult = Asset::Create< Package >( m_spRootSceneDefinitionsPackage, rootSceneDefinitionsPackagePath.GetName(), NULL );
	HELIUM_ASSERT( bCreateResult );
	if( !bCreateResult )
	{
		HELIUM_TRACE(
			TraceLevels::Error,
			TXT( "WorldManager::Initialize(): Failed to create world definition package \"%s\".\n" ),
			*rootSceneDefinitionsPackagePath.ToString() );

		return false;
	}

	HELIUM_ASSERT( m_spRootSceneDefinitionsPackage );

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

/// Get the path to the package containing all world instances.
///
/// @return  World package path.
AssetPath WorldManager::GetRootSceneDefinitionPackagePath() const
{
	static AssetPath worldPackagePath;
	if( worldPackagePath.IsEmpty() )
	{
		HELIUM_VERIFY( worldPackagePath.Set( TXT( "/Worlds" ) ) );
	}

	return worldPackagePath;
}

/// Get the instance of the package containing all world instances.
///
/// @return  World package instance.
Package* WorldManager::GetRootSceneDefinitionsPackage() const
{
	return m_spRootSceneDefinitionsPackage;
}

/// Create a new World instance.
///
/// @param[in] pSceneDefinition  The SceneDefinition from which to create the new World.
///
/// @return  Newly created world instance.
Helium::World* WorldManager::CreateWorld( SceneDefinition* pSceneDefinition )
{
	// Any scene that creates a world must define a world definition so that the world knows what components to init on itself
	HELIUM_ASSERT(pSceneDefinition);

	const WorldDefinition *pWorldDefinition = pSceneDefinition->GetWorldDefinition();

	WorldPtr spWorld;
	if (pWorldDefinition)
	{
		// Let the world definition provide the world
		spWorld = pWorldDefinition->CreateWorld();
	}
	else
	{
		// Make a blank, default world.
		// TODO: Consider having the concept of a "default" world definition
		spWorld = new World();
	}

	HELIUM_ASSERT(spWorld.Get());

	m_worlds.Push( spWorld );

	if ( pSceneDefinition && spWorld )
	{
		Slice *pRootSlice = spWorld->GetRootSlice();
		size_t entityDefinitionCount = pSceneDefinition->GetEntityDefinitionCount();

		for ( size_t i = 0; i < entityDefinitionCount; ++i )
		{
			EntityDefinition *pEntityDefinition = pSceneDefinition->GetEntityDefinition( i );
			HELIUM_ASSERT( pEntityDefinition );
			Entity *pEntity = pRootSlice->CreateEntity(pEntityDefinition);
		}
	}

	return spWorld;
}

/// Release a managed World instance.
///
/// @param[in] pWorld  World to release.
///
/// @return True if world was found and released, otherwise false.
bool WorldManager::ReleaseWorld( World* pWorld )
{
	for ( size_t i = 0; i < m_worlds.GetSize(); ++i )
	{
		if ( m_worlds.GetElement(i).Get() == pWorld )
		{
			m_worlds.Remove(i);
			return true;
		}
	}

	return false;
}

/// Update all worlds for the current frame.
void WorldManager::Update()
{
	// Update the world time.
	UpdateTime();
	
	Helium::TaskScheduler::ExecuteSchedule( m_worlds );
	
	Components::Tick();
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

//----------------------------------------------------------------------------------------------------------------------
// EntityPreUpdate.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/FrameworkInterface.h"

#include "Engine/JobManager.h"
#include "Framework/EntityDefinition.h"
#include "Framework/Entity.h"
#include "Framework/WorldManager.h"

using namespace Helium;

/// Run the EntityPreUpdate job.
///
/// @param[in] pContext  Context in which this job is running.
void EntityPreUpdate::Run( JobContext* /*pContext*/ )
{
    WorldManager& rWorldManager = WorldManager::GetStaticInstance();
    Entity* pEntity = GetParameters().pEntity;
    pEntity->PreUpdate( rWorldManager.GetFrameDeltaSeconds() );
#if 0
    HELIUM_ASSERT( pEntity );
    HELIUM_ASSERT( pEntity->NeedsAsynchronousUpdate() );

    WorldManager& rWorldManager = WorldManager::GetStaticInstance();

#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    rWorldManager.SetCurrentThreadUpdateEntity( pEntity );
#endif

    pEntity->PreUpdate( rWorldManager.GetFrameDeltaSeconds() );

#if HELIUM_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    rWorldManager.SetCurrentThreadUpdateEntity( NULL );
#endif

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
#endif
}

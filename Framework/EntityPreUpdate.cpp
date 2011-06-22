//----------------------------------------------------------------------------------------------------------------------
// EntityPreUpdate.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/FrameworkInterface.h"

#include "Engine/JobManager.h"
#include "Framework/Entity.h"
#include "Framework/WorldManager.h"

using namespace Helium;

/// Run the EntityPreUpdate job.
///
/// @param[in] pContext  Context in which this job is running.
void EntityPreUpdate::Run( JobContext* /*pContext*/ )
{
    Entity* pEntity = m_parameters.pEntity;
    HELIUM_ASSERT( pEntity );
    HELIUM_ASSERT( pEntity->NeedsAsynchronousUpdate() );

    WorldManager& rWorldManager = WorldManager::GetStaticInstance();

#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    rWorldManager.SetCurrentThreadUpdateEntity( pEntity );
#endif

    pEntity->PreUpdate( rWorldManager.GetFrameDeltaSeconds() );

#if L_ENABLE_WORLD_UPDATE_SAFETY_CHECKING
    rWorldManager.SetCurrentThreadUpdateEntity( NULL );
#endif

    JobManager& rJobManager = JobManager::GetStaticInstance();
    rJobManager.ReleaseJob( this );
}

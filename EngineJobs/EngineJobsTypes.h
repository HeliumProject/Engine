//----------------------------------------------------------------------------------------------------------------------
// EngineJobsTypes.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_ENGINE_JOBS_ENGINE_JOBS_TYPES_H
#define HELIUM_ENGINE_JOBS_ENGINE_JOBS_TYPES_H

#include "EngineJobs/EngineJobs.h"

namespace Helium
{
    /// Less-than comparison function for SortJob.
    ///
    /// @param[in] pElement0  First element to compare.
    /// @param[in] pElement1  Second element to compare.
    /// @param[in] pUserData  User data provided to the sort job.
    ///
    /// @return  True if the first element should be sorted before the second, false if the second element should be
    ///          sorted before the first or if both elements share the same sort priority.
    typedef bool ( *SORT_LESS_FUNC )( const void* pElement0, const void* pElement1, const void* pUserData );

    /// Swap function for SortJob.
    ///
    /// @param[in] pElement0  First element to swap.
    /// @param[in] pElement1  Second element to swap.
    typedef void ( *SORT_SWAP_FUNC )( void* pElement0, void* pElement1 );
}

#endif  // HELIUM_ENGINE_JOBS_ENGINE_JOBS_TYPES_H

#include "Precompile.h"
#include <map>
#include <sstream>
#include <stdio.h>
#include <string>
#include <sys/timeb.h>
#include <time.h>
#include <vector>

#include "RCS.h"
#include "RCSExceptions.h"

#include "Foundation/FilePath.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::RCS;

static Provider*                  g_Provider = NULL;
static Changeset                  g_DefaultChangeset;
static std::vector< std::string > g_ManagedPaths;
static std::vector< std::string > g_IgnoredPaths;
static uint64_t                   g_SyncTimestamp = 0;

///////////////////////////////////////////////////////////////////
// Utility methods

// 
// prepends projectRoot onto each element of paths 
// checks to see if the resulting prefix is found in the query path
// 
static bool _IsSubdir(const std::string& query, std::vector< std::string >& paths)
{
	FilePath queryPath(query);
	FilePath::Normalize(queryPath);

	for ( std::vector< std::string >::const_iterator it = paths.begin(), end = paths.end(); it != end; ++it )
	{
		FilePath path( *it );
		FilePath::Normalize( path );
		if ( queryPath.Get().compare(0, path.Length(), path.Data()) == 0 )
		{
			return true;
		}
	}

	return false;
}

Changeset& RCS::DefaultChangeset()
{
	return g_DefaultChangeset;
}

void RCS::SetProvider(Provider* provider)
{
	if ( provider != NULL && g_Provider != NULL )
	{
		throw RCS::Exception( "Attempt to re-set provider.  Current provider is: %s\n", g_Provider->GetName());
	}

	g_Provider = provider;
}

Provider* RCS::GetProvider()
{
	return g_Provider;
}

///////////////////////////////////////////////////////////////////
// Implementation

void RCS::SetManagedPaths(const std::vector< std::string >& paths)
{
	g_ManagedPaths = paths;
}

void RCS::SetIgnoredPaths(const std::vector< std::string >& paths)
{
	g_IgnoredPaths = paths;
}

bool RCS::PathIsManaged(const std::string &path)
{
	// check to see if we should be ignoring this path first
	if ( !g_IgnoredPaths.empty() && _IsSubdir(path, g_IgnoredPaths) )
	{
		return false;
	}

	// check to see if it's in our list of paths to pay attention to
	return _IsSubdir(path, g_ManagedPaths);
}

void RCS::SetSyncTimestamp(const uint64_t timestamp)
{
	if ( timestamp == SyncTimes::Now )
	{
		time_t t;
		time(&t);
		g_SyncTimestamp = t;
	}
	else
	{
		g_SyncTimestamp = timestamp;
	}
}

uint64_t RCS::GetSyncTimestamp()
{
	return g_SyncTimestamp;
}

// Changelist functions

void RCS::GetChangesets(RCS::V_Changeset& changesets)
{
	g_Provider->GetChangesets(changesets);

	V_File files;
	GetOpenedFiles(files);

	V_Changeset::iterator changeItr = changesets.begin();
	V_Changeset::iterator changeEnd = changesets.end();

	V_File::const_iterator itr = files.begin();
	V_File::const_iterator end = files.end();
	for ( ; itr != end; ++itr )
	{
		changeItr = changesets.begin();

		for ( ; changeItr != changeEnd; ++changeItr )
		{
			if ( ( *changeItr ).m_Id == ( *itr ).m_ChangesetId )
			{
				( *changeItr ).m_Files.push_back(( *itr ));
				break;
			}
		}
	}
}

bool RCS::IsValidChangeset(const RCS::Changeset& changeset)
{
	if ( changeset.m_Id == DefaultChangesetId )
	{
		return true;
	}

	RCS::V_Changeset currentChangesets;
	g_Provider->GetChangesets(currentChangesets);

	RCS::V_Changeset::const_iterator itr = currentChangesets.begin();
	RCS::V_Changeset::const_iterator end = currentChangesets.end();
	for ( ; itr != end; ++itr )
	{
		if ( ( *itr ).m_Id == changeset.m_Id )
		{
			return true;
		}
	}

	return false;
}

void RCS::GetInfo(V_File& files, GetInfoFlag flags)
{
	for ( V_File::iterator itr = files.begin(), end = files.end();
		itr != end;
		++itr )
	{
		( *itr ).GetInfo(flags);
	}
}

void RCS::GetInfo(const std::string& folder, V_File& files, bool recursive, uint32_t fileData, uint32_t actionData)
{
	g_Provider->GetInfo(folder, files, recursive, fileData, actionData);
}

void RCS::GetOpenedFiles(V_File &files)
{
	g_Provider->GetOpenedFiles(files);
}

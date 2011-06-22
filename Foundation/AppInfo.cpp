#include "FoundationPch.h"
#include "FoundationPch.h"
#include "Foundation/AppInfo.h"

using namespace Helium;

String AppInfo::sm_name;

/// Set the application name string.
///
/// @param[in] rName  Name to set.
///
/// @see GetName()
void AppInfo::SetName( const String& rName )
{
    sm_name = rName;
}

/// Clear out all application information values and free all allocated memory.
void AppInfo::Clear()
{
    sm_name.Clear();
}

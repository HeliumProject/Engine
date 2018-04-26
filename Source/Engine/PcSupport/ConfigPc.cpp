#include "Precompile.h"
#include "PcSupport/ConfigPc.h"

#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Engine/Config.h"
#include "Persist/Archive.h"

using namespace Helium;

/// Save the user configuration settings.
///
/// @return  True if the configuration was saved successfully, false if not.
bool ConfigPc::SaveUserConfig()
{
	HELIUM_TRACE( TraceLevels::Info, "ConfigPc: Saving user configuration.\n" );

	Config* pConfig = Config::GetInstance();
	HELIUM_ASSERT( pConfig );

	FilePath userDirectory;
	if ( !FileLocations::GetUserDirectory( userDirectory ) )
	{
		HELIUM_TRACE( TraceLevels::Warning, "ConfigPc: No user data directory could be determined.\n" );
		return false;
	}

	for ( size_t i = 0; i < pConfig->GetConfigObjectCount(); ++i )
	{
		const Name &name = pConfig->GetConfigObjectName( i );
		FilePath path = pConfig->GetUserConfigObjectFilePath( name );

		Reflect::Object *configObject = pConfig->GetConfigObject<Reflect::Object>( i );
		Reflect::ObjectPtr ptr( configObject );

		HELIUM_TRACE( TraceLevels::Info, "Writing user config to: %s", path.Get().c_str() );
		Persist::ArchiveWriter::WriteToFile( path, ptr );
	}

	HELIUM_TRACE( TraceLevels::Info, "ConfigPc: User configuration saved.\n" );

	return true;
}

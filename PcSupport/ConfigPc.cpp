#include "PcSupportPch.h"
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
	HELIUM_TRACE( TraceLevels::Info, TXT( "ConfigPc: Saving user configuration.\n" ) );

	Config& rConfig = Config::GetStaticInstance();

	FilePath userDataDirectory;
	if ( !FileLocations::GetUserDataDirectory( userDataDirectory ) )
	{
		HELIUM_TRACE( TraceLevels::Warning, TXT( "ConfigPc: No user data directory could be determined.\n" ) );
		return false;
	}

	for( int i = 0; i < rConfig.GetConfigObjectCount(); ++i )
	{
		const Name &name = rConfig.GetConfigObjectName( i );
		FilePath path = rConfig.GetUserConfigObjectFilePath( name );

		Reflect::Object *configObject = rConfig.GetConfigObject<Reflect::Object>(i);
		Reflect::ObjectPtr ptr(configObject);

		Persist::ArchiveWriter::WriteToFile(path, ptr);
	}

	HELIUM_TRACE( TraceLevels::Info, TXT( "ConfigPc: User configuration saved.\n" ) );

	return true;
}

#pragma once

#include "Platform/Types.h"

#include "Foundation/String.h"
#include "Foundation/FilePath.h"

#include "Engine/Engine.h"

namespace Helium
{
	class FileStream;

	/// Low-level file utilities
	class HELIUM_ENGINE_API FileLocations
	{
	public:
		/// @name Static Initialization
		//@{
		static void Shutdown();
		//@}

		/// @name Filesystem Information
		//@{
		static void SetBaseDirectory( const FilePath& path );
		static bool GetBaseDirectory( FilePath& path );
		static bool GetDataDirectory( FilePath& path );
		static bool GetUserDirectory( FilePath& path );
		//@}
	};
}

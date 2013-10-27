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
		static const bool GetBaseDirectory( FilePath& path );
		static const bool GetDataDirectory( FilePath& path );
		static const bool GetUserDataDirectory( FilePath& path );
		//@}

	private:
		/// @name Platform-specific Initialization
		//@{
		static void PlatformShutdown();
		//@}
	};
}

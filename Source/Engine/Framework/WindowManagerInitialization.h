#pragma once

#include "Framework/Framework.h"

namespace Helium
{
	class WindowManager;

	/// Interface for window manager initialization.
	class HELIUM_FRAMEWORK_API WindowManagerInitialization
	{
	public:
		/// @name Window Manager Initialization
		//@{
		virtual void Startup() = 0;
		virtual void Shutdown() = 0;
		//@}
	};
}

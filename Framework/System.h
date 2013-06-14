#pragma once

#include "Framework/Framework.h"
#include "Platform/Utility.h"

#include "Foundation/String.h"

namespace Helium
{
	/// Base interface for management of application and component initialization and the main application loop.
	class HELIUM_FRAMEWORK_API System : NonCopyable
	{
	public:
		/// @name Initialization
		//@{
		virtual void Shutdown();
		//@}

		/// @name Static Access
		//@{
		static System* GetStaticInstance();
		static void DestroyStaticInstance();
		//@}

		virtual void StopRunning() = 0;

	protected:
		/// Module file name.
		String m_moduleName;
		/// Command-line arguments (not including the module name).
		DynamicArray< String > m_arguments;

		/// Singleton instance.
		static System* sm_pInstance;

		/// @name Construction/Destruction
		//@{
		System();
		virtual ~System() = 0;
		//@}
	};
}

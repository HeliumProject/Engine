#pragma once

#include "FrameworkImpl/FrameworkImpl.h"
#include "Framework/WindowManagerInitialization.h"

namespace Helium
{
	/// Window manager factory implementation for Windows.
	class HELIUM_FRAMEWORK_IMPL_API WindowManagerInitializationImpl : public WindowManagerInitialization
	{
	public:
		/// @name Construction/Destruction
		//@{
#if HELIUM_DIRECT3D
		WindowManagerInitializationImpl( void* hInstance, int nCmdShow );
#else
		WindowManagerInitializationImpl();
#endif
		//@}

		/// @name Window Manager Initialization
		//@{
		virtual void Startup() override;
		virtual void Shutdown() override;
		//@}

	protected:
#if HELIUM_DIRECT3D
		/// Handle to the application instance.
		void* m_hInstance;
		/// Flags specifying how the application window should be shown.
		int m_nCmdShow;
#endif
	};
}

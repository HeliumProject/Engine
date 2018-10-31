#pragma once

#include "Windowing/Windowing.h"
#include "Windowing/Window.h"

namespace Helium
{
    /// Interface for managing window creation.
    class HELIUM_WINDOWING_API WindowManager : NonCopyable
    {
    public:
        /// @name Initialization
        //@{
#if HELIUM_DIRECT3D
		bool Initialize( void* hInstance, int nCmdShow );
#else
        bool Initialize();
#endif
        void Cleanup();
        //@}

        /// @name Updating
        //@{
        bool Update();
        void RequestQuit();
        //@}

        /// @name Window Creation
        //@{
        Window* Create( Window::Parameters& rParameters );
        void Destroy( Window* pWindow );
        //@}

        /// @name Static Access
        //@{
        static WindowManager* GetInstance();
		static void Startup();
		static void Shutdown();
        //@}

    protected:
#if HELIUM_DIRECT3D
        /// Handle to the application instance.
        void* m_hInstance;
        /// Flags specifying how the application window should be shown.
        int m_nCmdShow;
        /// Default window class atom.
        uint16_t m_windowClassAtom;
#elif HELIUM_OPENGL
		/// Flag to indicate successful initialization.
		bool m_isInitialized;
		/// Flag indicating whether a user has called RequestQuit().
		bool m_isQuitting;
#endif

        /// Singleton instance.
        static WindowManager* sm_pInstance;

		/// @name Construction/Destruction
        //@{
        WindowManager();
        ~WindowManager();
        //@}

    private:
#if HELIUM_DIRECT3D
        /// @name Window Procedure Callback
        //@{
        static uintptr_t __stdcall WindowProc( void* hWnd, uint32_t msg, uintptr_t wParam, uintptr_t lParam );
        //@}
#elif HELIUM_OPENGL
        /// @name Window System Callbacks
        //@{
		static void GLFWCloseCallback( Window::Handle pHandle );
		static void GLFWErrorCallback( int error, const char* description );
		//@}
#endif
	};
}

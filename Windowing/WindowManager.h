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
#if HELIUM_OS_WIN
		bool Initialize( HINSTANCE hInstance, int nCmdShow );
#else
        bool Initialize();
#endif
        void Shutdown();
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
        static WindowManager* GetStaticInstance();
		static WindowManager* CreateStaticInstance();
		static void DestroyStaticInstance();
        //@}

    protected:
#if HELIUM_OS_WIN
        /// Handle to the application instance.
        HINSTANCE m_hInstance;
        /// Flags specifying how the application window should be shown.
        int m_nCmdShow;
        /// Default window class atom.
        ATOM m_windowClassAtom;
#endif

        /// Singleton instance.
        static WindowManager* sm_pInstance;

		/// @name Construction/Destruction
        //@{
        WindowManager();
        ~WindowManager();
        //@}

    private:
#if HELIUM_OS_WIN
        /// @name Window Procedure Callback
        //@{
        static LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
        //@}
#endif
	};
}

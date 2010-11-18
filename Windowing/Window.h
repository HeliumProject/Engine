//----------------------------------------------------------------------------------------------------------------------
// Window.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_WINDOWING_WINDOW_H
#define LUNAR_WINDOWING_WINDOW_H

#include "Windowing/Windowing.h"

#include <boost/function.hpp>

namespace Lunar
{
    /// Interface to a window.
    class LUNAR_WINDOWING_API Window : NonCopyable
    {
    public:
        /// Callback to execute when a Window instance is actually destroyed.
        typedef boost::function< void ( Window* pWindow ) > OnDestroyed;

        /// Creation parameters.
        struct LUNAR_WINDOWING_API Parameters
        {
            /// Window title.
            const tchar_t* pTitle;
            /// Window width, in pixels.
            uint32_t width;
            /// Window height, in pixels.
            uint32_t height;
            /// True to create the window for presentation as a full-screen display, false to initialize for a windowed
            /// display.
            bool bFullscreen;

            /// @name Construction/Destruction
            //@{
            inline Parameters();
            //@}
        };

        /// @name Window Management
        //@{
        virtual void Destroy() = 0;
        //@}

        /// @name Data Access
        //@{
        inline void* GetHandle() const;
        inline const String& GetTitle() const;
        inline uint32_t GetWidth() const;
        inline uint32_t GetHeight() const;
        inline bool GetFullscreen() const;
        //@}

        /// @name Callback Registration
        //@{
        void SetOnDestroyed( const OnDestroyed& rOnDestroyed );
        inline const OnDestroyed& GetOnDestroyed() const;
        //@}

    protected:
        /// Callback to execute when this window is actually destroyed.
        OnDestroyed m_onDestroyed;

        /// Platform-dependent window handle.
        void* m_pHandle;
        /// Window title.
        String m_title;
        /// Window width, in pixels.
        uint32_t m_width;
        /// Window height, in pixels.
        uint32_t m_height;
        /// True if the window is configured for display as a full-screen window, false if it is set up for windowed
        /// display.
        bool m_bFullscreen;

        /// @name Construction/Destruction, Protected
        //@{
        Window();
        Window( void* pHandle, const tchar_t* pTitle, uint32_t width, uint32_t height, bool bFullscreen );
        virtual ~Window() = 0;
        //@}
    };
}

#include "Windowing/Window.inl"

#endif  // LUNAR_WINDOWING_WINDOW_H

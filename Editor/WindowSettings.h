#pragma once

#include "Editor/API.h"
#include "Foundation/Reflect/Element.h"

class wxAuiManager;

namespace Helium
{
    namespace Editor
    {
        class WindowSettings;
        typedef Helium::SmartPtr< WindowSettings > WindowSettingsPtr;

        class WindowSettings : public Reflect::ConcreteInheritor< WindowSettings, Reflect::Element >
        {
        public:
            WindowSettings( wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize );

            void SetFromWindow( const wxWindow* window, wxAuiManager* manager = NULL );
            void ApplyToWindow( wxWindow* window, wxAuiManager* manager = NULL, bool updateAui = false );

            wxPoint GetPosition() const;
            void SetPosition( i32 x, i32 y );
            void SetPosition( wxPoint pos ) { SetPosition( pos.x, pos.y ); }

            const Reflect::Field* Width() const;
            const Reflect::Field* Height() const;
            wxSize GetSize() const;
            void SetSize( i32 width, i32 height );
            void SetSize( wxSize size ) { SetSize( size.x, size.y ); }

            const Reflect::Field* Maximized() const;
            bool IsMaximized() const;
            void SetMaximized( bool maximized );

            const Reflect::Field* DockingState() const;
            const tstring& GetDockingState() const;
            void SetDockingState( const tstring& state );

            static void Check( WindowSettingsPtr& settings );
            static bool Validate( wxPoint pos, wxSize size );

        private:
            // String (wxWidgets AUI) representing docking state of all child panels 
            tstring m_DockingState;

            bool m_IsMaximized;

            // Position of the window (upper left corner)
            i32 m_PosX;
            i32 m_PosY;

            // Size of the window
            i32 m_Width;
            i32 m_Height;

        public:
            static const tchar* s_Reset;
            static const tchar* s_ResetLong;

            static void EnumerateClass( Reflect::Compositor<WindowSettings>& comp )
            {
                comp.AddField( &WindowSettings::m_DockingState, "m_DockingState" );
                comp.AddField( &WindowSettings::m_IsMaximized, "m_IsMaximized" );
                comp.AddField( &WindowSettings::m_PosX, "m_PosX" );
                comp.AddField( &WindowSettings::m_PosY, "m_PosY" );
                comp.AddField( &WindowSettings::m_Width, "m_Width" );
                comp.AddField( &WindowSettings::m_Height, "m_Height" );
            }
        };
    }
}
#pragma once

#include "EditorScene/SettingsManager.h"

#include "Editor/API.h"

class wxAuiManager;

namespace Helium
{
    namespace Editor
    {
        class WindowSettings;         
        typedef Helium::StrongPtr< WindowSettings > WindowSettingsPtr;

        class WindowSettings : public Settings
        {
        public:
            HELIUM_DECLARE_CLASS( WindowSettings, Settings );

            WindowSettings( wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize );

            virtual bool UserVisible() override
            {
                return false;
            }

            void SetFromWindow( const wxWindow* window, wxAuiManager* manager = NULL );
            void ApplyToWindow( wxWindow* window, wxAuiManager* manager = NULL, bool updateAui = false );

            wxPoint GetPosition() const;
            void SetPosition( int32_t x, int32_t y );
            void SetPosition( wxPoint pos ) { SetPosition( pos.x, pos.y ); }

            const Reflect::Field* Width() const;
            const Reflect::Field* Height() const;
            wxSize GetSize() const;
            void SetSize( int32_t width, int32_t height );
            void SetSize( wxSize size ) { SetSize( size.x, size.y ); }

            const Reflect::Field* Maximized() const;
            bool IsMaximized() const;
            void SetMaximized( bool maximized );

            const Reflect::Field* DockingState() const;
            const std::string& GetDockingState() const;
            void SetDockingState( const std::string& state );

            static void Check( WindowSettingsPtr& settings );
            static bool Validate( wxPoint pos, wxSize size );

        private:
            // String (wxWidgets AUI) representing docking state of all child panels 
            std::string m_DockingState;

            bool m_IsMaximized;

            // Position of the window (upper left corner)
            int32_t m_PosX;
            int32_t m_PosY;

            // Size of the window
            int32_t m_Width;
            int32_t m_Height;

        public:
            static void PopulateMetaType( Reflect::MetaStruct& comp )
            {
                comp.AddField( &WindowSettings::m_DockingState, TXT( "Docking State" ) );
                comp.AddField( &WindowSettings::m_IsMaximized, TXT( "Maximized" ) );
                comp.AddField( &WindowSettings::m_PosX, TXT( "X Position" ) );
                comp.AddField( &WindowSettings::m_PosY, TXT( "Y Position" ) );
                comp.AddField( &WindowSettings::m_Width, TXT( "Width" ) );
                comp.AddField( &WindowSettings::m_Height, TXT( "Height" ) );
            }
        };
    }
}
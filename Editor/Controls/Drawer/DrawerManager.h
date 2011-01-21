#pragma once

#include "Drawer.h"

namespace Helium
{
    namespace Editor
    {
        class MainFrame;

        class DrawerManager
        {
        public:
            DrawerManager();
            virtual ~DrawerManager();

            void SetAuiManager( wxAuiManager* auiManager );

            void AddDrawer( Drawer* drawer );
            void RemoveDrawer( Drawer* drawer );
            void RemoveAllDrawers();

            Drawer* GetCurrentDrawer() const;

            bool IsFixedSizeButtons() const;
            void SetFixedSizeButtons( bool fixedSize );

        private:
            Drawer* FindDrawer( int32_t drawerID );

            void OnDrawerOpening( const DrawerEventArgs& args );
            void OnDrawerOpened( const DrawerEventArgs& args );
            void OnDrawerClosed( const DrawerEventArgs& args );

        private:
            typedef std::map< int32_t, Drawer* > M_DrawerButtonIDs;
            M_DrawerButtonIDs m_Drawers;

            Drawer* m_CurrentDrawer;
            bool m_IsFixedSizeButtons;
        };
    }
}


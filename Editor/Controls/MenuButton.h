#pragma once

/// @file MenuButton.h
/// This adds on to the regular button giving us the ability to add a menu which appears
/// either when the left mouse button is held down for a specified amount of time, or if the 
/// right mouse button is pressed.
#include "Editor/API.h"

#include "Editor/Controls/Button.h"

#include <wx/timer.h>
#include <wx/menu.h>

namespace Helium
{
    namespace Editor
    {
        /// @class MenuButton
        /// @brief adds on to the Button class to add menu functionality through mouse interaction
        class MenuButton : public Button
        {
        public:
            // wxWidgets setup
            DECLARE_DYNAMIC_CLASS( MenuButton )
            DECLARE_EVENT_TABLE( )

        protected:
            wxMenu *m_ContextMenu;          //<! context menu associated with this button

        private:
            float m_HoldDelay;             //<! amount of seconds to delay if we need to hold the button for the menu to show
            wxTimer m_TimerShowOnHold;     //<! when the left mouse button is held, this timer fires telling us we should display the menu

            /// @function HandleTimerEvents
            /// @brief this is the function that responds to timer events and displays the context menu
            /// @param event parameters that are passed to the handler
            void HandleTimerEvents( wxTimerEvent& event );

        public:
            /// @function MenuButton
            /// @brief Constructor
            MenuButton(void)
            {
            }

            /// @function 
            /// @brief Constructor
            MenuButton(wxWindow *parent,
                wxWindowID id,
                const wxString& label = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxBU_EXACTFIT,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxButtonNameStr);

            /// @function ~MenuButton
            /// @brief destructor
            virtual ~MenuButton(void);

        public:
            /// @function SetHoldDelay
            /// @brief set the amount of time needed to left mouse down before the menu displays
            /// @param amount of Seconds before the menu displays
            void  SetHoldDelay( float delay ) { m_HoldDelay = delay; }

            /// @function GetHoldDelay
            /// @brief get the amount of time needed before the menu displays
            /// @return time to display in Seconds
            float GetHoldDelay( ) const { return m_HoldDelay; }

            /// @function DetatchContextMenu
            /// @brief unassociates a menu from the button. The button will no longer reference this menu and it is up to the user to clean it up.
            /// @return a pointer to the menu that is no longer associated with the button
            wxMenu* DetachContextMenu( );

            /// @function GetContextMenu
            /// @brief returns a pointer to the menu that is currently associated with the button. The button retains a reference to the menu.
            /// @return a pointer to the menu associated with the button.
            wxMenu* GetContextMenu( ) const;

            /// @function SetContextMenu
            /// @brief associates a menu with the button. During shutdown, the button will try to destroy this menu if it still retains a reference to it.
            /// @param the menu to associate with this button
            void SetContextMenu( wxMenu * menu ) ;

        protected:
            virtual void DoSetBitmap(const wxBitmap& bitmap, State which) HELIUM_OVERRIDE;

            /// @function OnRightMouseDown
            /// @brief Right mouse down event handler. Displays the menu
            /// @param event parameters that are passed to the handler
            virtual void OnRightMouseDown( wxMouseEvent& event );

            /// @function OnLeftMouseDown
            /// @brief starts the timer that determines if we are going to display the context menu
            /// @param event parameters that are passed to the handler
            virtual void OnLeftMouseDown( wxMouseEvent& event );

            /// @function OnLeftMouseUp
            /// @brief If the  timer has not fired yet, this will stop the event from happening (which stops the menu from showing)
            /// @param event parameters that are passed to the handler
            virtual void OnLeftMouseUp( wxMouseEvent& event );

            virtual void OnMenuOpen( wxMenuEvent& args );
            virtual void OnMenuClose( wxMenuEvent& args );

            /// @function ShowPopupMenu
            /// @brief Shows the popup menu.
            virtual void ShowPopupMenu();

        };
    }
} // namespace Helium
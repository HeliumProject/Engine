#pragma once

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class ListWidget;

        class ListWindow : public wxListBox
        {
        public:
            ListWindow(wxWindow* parent, ListWidget* listWidget, long style )
                : wxListBox (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, style)
                , m_ListWidget (listWidget)
            {

            }

            DECLARE_EVENT_TABLE();

            ListWidget* m_ListWidget;
        };

        class ListWidget : public Widget
        {
        public:
            REFLECT_DECLARE_CLASS( ListWidget, Widget );

            ListWidget()
                : m_ListControl( NULL )
                , m_ListWindow( NULL )
            {

            }

            ListWidget( Inspect::List* list );

            virtual void CreateWindow( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void DestroyWindow() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

        private:
            Inspect::List*  m_ListControl;
            ListWindow*     m_ListWindow;
        };
    }
}

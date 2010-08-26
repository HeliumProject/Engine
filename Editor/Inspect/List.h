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
            ListWidget( Inspect::List* control );

            void Create( wxWindow* parent );
            void Destroy();

            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

        private:
            Inspect::List*  m_ListControl;
            ListWindow*     m_ListWindow;
        };
    }
}

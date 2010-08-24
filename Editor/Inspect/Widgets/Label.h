#pragma once

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class LabelWidget;

        class LabelPanel : public wxPanel
        {
        public:
            LabelPanel(wxWindow* parent, LabelWidget* labelWidget);

            void OnSize(wxSizeEvent& event);

            wxString GetLabel() const;
            void SetLabel(const wxString& label);

            void UpdateToolTip( const wxString& toolTip );

            virtual bool SetForegroundColour(const wxColour& color) HELIUM_OVERRIDE;
            virtual bool SetBackgroundColour(const wxColour& color) HELIUM_OVERRIDE;

            DECLARE_EVENT_TABLE();

        private:
            LabelWidget*    m_LabelWidget;
            wxStaticText*   m_StaticText;
        };

        class LabelWidget : public Widget
        {
        public:
            LabelWidget( Inspect::Control* control );

            void Create( wxWindow* parent );
            void Destroy();

            // Inspect::Widget
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            // Editor::Widget
            virtual void ToolTipChanged( const Attribute<tstring>::ChangeArgs& args ) HELIUM_OVERRIDE;

        protected:
            Inspect::Label*     m_LabelControl;
            LabelPanel*         m_LabelWindow;
        };
    }
}
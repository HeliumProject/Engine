#include "Precompile.h"
#include "Label.h"

#include <wx/panel.h>
#include <wx/stattext.h>

using namespace Helium;
using namespace Helium::Editor;

namespace Helium
{
    namespace Editor
    {
        class LabelPanel : public wxPanel
        {
        private:
            LabelWidget* m_Label;
            wxStaticText* m_StaticText;

        public:
            LabelPanel(wxWindow* parent, LabelWidget* label)
                : wxPanel (parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, wxT( "Text" ) )
                , m_Label (label)
            {
                SetSizer( new wxBoxSizer( wxHORIZONTAL ) );
                wxSizer* sizer = GetSizer();

                m_StaticText = new wxStaticText( this, wxID_ANY, wxT( "Temp" ) );
                sizer->Add( m_StaticText, 0, wxALIGN_CENTER_VERTICAL, 0);
            }

            void OnSize(wxSizeEvent& event)
            {
                m_Label->GetControl()->Read();

                Layout();
            }

            void SetLabel(const wxString& label)
            {
                m_StaticText->SetLabel(label);
            }

            void UpdateToolTip( const wxString& toolTip )
            {
                m_StaticText->SetToolTip( toolTip );
            }

            virtual bool SetForegroundColour(wxColour& color)
            {
                return wxPanel::SetForegroundColour(color)
                    && m_StaticText->SetForegroundColour(color);
            }

            virtual bool SetBackgroundColour(wxColour& color)
            {
                return wxPanel::SetBackgroundColour(color)
                    && m_StaticText->SetBackgroundColour(color);
            }

            DECLARE_EVENT_TABLE();
        };

        BEGIN_EVENT_TABLE(LabelPanel, wxPanel)
        EVT_SIZE(LabelPanel::OnSize)
        END_EVENT_TABLE()
    }
}

LabelWidget::LabelWidget( Inspect::Control* control )
: Widget( control )
{
    
}

void LabelWidget::Create( wxWindow* parent )
{
    SetWindow( m_LabelWindow = new LabelPanel( parent, this ) );
}

void LabelWidget::ToolTipChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_LabelWindow->UpdateToolTip( args.m_NewValue );
}

void LabelWidget::TextChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_LabelWindow->SetLabel( args.m_NewValue );
}
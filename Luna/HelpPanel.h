#pragma once

#include <wx/panel.h>
#include <wx/richtext/richtextctrl.h>

#include "Platform/Types.h"

namespace Luna
{
    class HelpPanel : public wxPanel
    {
    public:
        HelpPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT( "HelpPanel" ) );
        virtual ~HelpPanel();

        void SetText( const tchar* text );

    protected:
        wxRichTextCtrl* m_HelpRichText;
    };
}

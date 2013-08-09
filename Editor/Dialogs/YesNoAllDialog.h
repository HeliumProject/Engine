#pragma once

#include "Platform/Types.h"

#include <wx/statline.h>

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // MetaClass YesNoAllDialog
        //
        class YesNoAllDialog : public wxDialog 
        {
        private:
            wxTextCtrl* m_Text;
            wxStaticLine* m_Separator;
            wxButton* m_ButtonYes;
            wxButton* m_ButtonYesAll;
            wxButton* m_ButtonNo;
            wxButton* m_ButtonNoAll;
            wxButton* m_ButtonCancel;

        public:
            YesNoAllDialog( wxWindow* parent, const std::string& title, const std::string& msg, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 370,130 ), int style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxWANTS_CHARS );
            virtual ~YesNoAllDialog();
            void SetButtonToolTip( int buttonId, const std::string& tooltip );

        private:
            void OnButtonClick( wxCommandEvent& args );

        private:
            DECLARE_EVENT_TABLE();
        };
    }
}
#pragma once

#include "Platform/Types.h"

#include <wx/statline.h>

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Class YesNoAllDlg
        //
        class YesNoAllDlg : public wxDialog 
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
            YesNoAllDlg( wxWindow* parent, const tstring& title, const tstring& msg, int id = -1, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 370,130 ), int style = wxCAPTION|wxDEFAULT_DIALOG_STYLE|wxWANTS_CHARS );
            virtual ~YesNoAllDlg();
            void SetButtonToolTip( int buttonId, const tstring& tooltip );

        private:
            void OnButtonClick( wxCommandEvent& args );

        private:
            DECLARE_EVENT_TABLE();
        };
    }
}
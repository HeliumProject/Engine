#pragma once

#include "Application/API.h"

#include "Platform/Types.h"
#include "Platform/Compiler.h"

#include <wx/textctrl.h>
#include <wx/combobox.h>

namespace Nocturnal
{

    class APPLICATION_API AutoCompleteComboBox : public wxComboBox
    {
    public:
        AutoCompleteComboBox( wxWindow* parent, wxWindowID id, 
            const wxString& value = wxT( "" ),
            const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxT( "autoCompleteComboBox" ) );

        AutoCompleteComboBox( wxWindow* parent, wxWindowID id, 
            const wxString& value, 
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxT( "autoCompleteComboBox" ) );

        virtual ~AutoCompleteComboBox();

    public:
        DECLARE_EVENT_TABLE();

    public:
        void OnTextChanged( wxCommandEvent& event );
        void OnKeyDown( wxKeyEvent& event );

    protected:
        virtual void DoClear() NOC_OVERRIDE;
        virtual void DoDeleteOneItem( unsigned int n ) NOC_OVERRIDE;
        virtual int DoInsertOneItem( const wxString& item, unsigned int pos ) NOC_OVERRIDE;

    private:
        bool GetBestPartialMatch( const wxString& current, wxString& match );

    private:
        wxArrayString m_Choices;
        wxArrayString m_Matches;

        bool      m_UsedDeletion;
    };

} // namespace Nocturnal
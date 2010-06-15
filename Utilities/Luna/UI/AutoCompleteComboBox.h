#pragma once

#include "Luna/API.h"

#include "Platform/Types.h"

#include <wx/combobox.h>

namespace Luna
{

    class LUNA_CORE_API AutoCompleteComboBox : public wxComboBox
    {
    public:
        AutoCompleteComboBox( wxWindow* parent, wxWindowID id, 
            const wxString& value = "",
            const wxPoint& pos = wxDefaultPosition, 
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = "autoCompleteComboBox" );

        AutoCompleteComboBox( wxWindow* parent, wxWindowID id, 
            const wxString& value, 
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = "autoCompleteComboBox" );

        virtual ~AutoCompleteComboBox();

    public:
        DECLARE_EVENT_TABLE();

    public:
        virtual void OnTextChanged( wxCommandEvent& event );
        virtual void OnKeyDown( wxKeyEvent& event );

        virtual void Clear();
        virtual void Delete( unsigned int n );

    protected:
        virtual int DoAppend( const wxString& item );
        virtual int DoInsert( const wxString& item, unsigned int pos );

    private:
        bool GetBestPartialMatch( const wxString& current, wxString& match );
        void UpdateList( wxArrayString& items );

    private:
        wxArrayString m_Choices;
        wxArrayString m_Matches;

        bool      m_UsedDeletion;
    };

} // namespace Luna
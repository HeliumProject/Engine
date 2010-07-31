#pragma once

#include "Editor/API.h"

namespace Helium
{
    namespace Editor
    {
        //DECLARE_EVENT_TYPE( UpdateStatusCommandEvent, -1 )
        extern LUNA_CORE_API const wxEventType UpdateStatusCommandEvent;

        // A custom event that transports a whole wxString.
        class LUNA_CORE_API UpdateStatusEvent: public wxCommandEvent
        {
        public:
            UpdateStatusEvent( wxEventType commandType = Editor::UpdateStatusCommandEvent, int id = 0 )
                :  wxCommandEvent(commandType, id) { }

            // You *must* copy here the data to be transported
            UpdateStatusEvent( const UpdateStatusEvent &event )
                :  wxCommandEvent(event) { this->SetText( event.GetText() ); }

            // Required for sending with wxPostEvent()
            wxEvent* Clone() const { return new UpdateStatusEvent(*this); }

            wxString GetText() const { return m_Text; }
            void SetText( const wxString& text ) { m_Text = text; }

        private:
            wxString m_Text;
        };

        typedef void (wxEvtHandler::*UpdateStatusEventFunction)(Editor::UpdateStatusEvent &);
    }
}

// This #define simplifies the one below, and makes the syntax less
// ugly if you want to use Connect() instead of an event table.
#define LUNA_UPDATE_STATUS_EVENT_HANDLER(func) \
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction) \
    wxStaticCastEvent(Editor::UpdateStatusEventFunction, &func)                    

// Define the event table entry. Yes, it really *does* end in a comma.
#define LUNA_EVT_UPDATE_STATUS(id, fn) \
    DECLARE_EVENT_TABLE_ENTRY( Editor::UpdateStatusCommandEvent, id, wxID_ANY, \
    (wxObjectEventFunction)(wxEventFunction) \
    (wxCommandEventFunction) wxStaticCastEvent( \
    UpdateStatusEventFunction, &fn ), (wxObject*) NULL ),

// Optionally, you can do a similar #define for LUNA_EVT_UPDATE_STATUS_RANGE.
#define LUNA_EVT_UPDATE_STATUS_RANGE(id1,id2, fn) \
    DECLARE_EVENT_TABLE_ENTRY( Editor::UpdateStatusCommandEvent, id1, id2, \
    LUNA_UPDATE_STATUS_EVENT_HANDLER(fn), (wxObject*) NULL ),

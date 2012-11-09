#pragma once

#include "Editor/API.h"
#include "Thumbnail.h"

#include "Foundation/FilePath.h"

namespace Helium
{
    namespace Editor
    {
        //DECLARE_EVENT_TYPE( ThumbnailLoadedCommandEvent, -1 )
        extern const wxEventType ThumbnailLoadedCommandEvent;

        // A custom event that transports a whole wxString.
        class ThumbnailLoadedEvent: public wxCommandEvent
        {
        public:
            ThumbnailLoadedEvent( wxEventType commandType = Editor::ThumbnailLoadedCommandEvent, int id = 0 );

            ThumbnailLoadedEvent( const ThumbnailLoadedEvent &event );

            // Required for sending with wxPostEvent()
            wxEvent* Clone() const;

            const V_ThumbnailPtr& GetThumbnails() const;
            void SetThumbnails( const V_ThumbnailPtr& thumbnails );

            const Helium::FilePath& GetPath() const;
            void SetPath( const Helium::FilePath& path );

            bool Cancelled() const;
            void SetCancelled( bool cancelled );

        private:
            V_ThumbnailPtr m_Thumbnails;
            Helium::FilePath m_Path;
            bool m_Cancelled;
        };

        typedef void (wxEvtHandler::*igThumbnailLoadedEventFunction)(ThumbnailLoadedEvent &);
    } 
}

// This #define simplifies the one below, and makes the syntax less
// ugly if you want to use Connect() instead of an event table.
#define igThumbnailLoadedEventHandler(func)                                         \
    (wxObjectEventFunction)(wxEventFunction)(wxCommandEventFunction)\
    wxStaticCastEvent(igThumbnailLoadedEventFunction, &func)                    

// Define the event table entry. Yes, it really *does* end in a comma.
#define EDITOR_EVT_THUMBNAIL_LOADED(id, fn)                                            \
    DECLARE_EVENT_TABLE_ENTRY( Editor::ThumbnailLoadedCommandEvent, id, wxID_ANY,  \
    (wxObjectEventFunction)(wxEventFunction)                     \
    (wxCommandEventFunction) wxStaticCastEvent(                  \
    igThumbnailLoadedEventFunction, &fn ), (wxObject*) NULL ),

// Optionally, you can do a similar #define for EDITOR_EVT_THUMBNAIL_LOADED_RANGE.
#define EDITOR_EVT_THUMBNAIL_LOADED_RANGE(id1,id2, fn)                                 \
    DECLARE_EVENT_TABLE_ENTRY( Editor::ThumbnailLoadedCommandEvent, id1, id2,      \
    igThumbnailLoadedEventHandler(fn), (wxObject*) NULL ),

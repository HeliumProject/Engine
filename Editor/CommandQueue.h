#pragma once

#include "Editor/API.h"

#include "Foundation/CommandQueue.h"

namespace Helium
{
    namespace Editor
    {
        class CommandQueue : public Helium::CommandQueue, public wxEvtHandler
        {
        public:
            CommandQueue( wxWindow* window );
            ~CommandQueue();

        public:
            void EnqueueFlush();

        protected:
            void HandleEvent( wxCommandEvent& event );

        private:
            wxWindow*                               m_Window;   // the windows to use (for custom message dispatch)
        };
    }
}
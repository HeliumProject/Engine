#pragma once

#include "Editor/API.h"

#include "Foundation/Automation/Event.h"

#include "Foundation/Undo/Command.h"

namespace Helium
{
    namespace Editor
    {
        //
        // Designed to be aggregated into an editor, this class
        //  allows deferred command execution to allow performing
        //  commands to coexist easier with issuing commands in callbacks
        //

        struct PushCommandArgs
        {
            const Undo::V_CommandSmartPtr m_Commands;

            PushCommandArgs( const Undo::V_CommandSmartPtr& commands )
                : m_Commands (commands)
            {

            }
        };

        typedef Helium::Signature<void, const PushCommandArgs&> PushCommandSignature;

        class CommandQueue : public wxEvtHandler
        {
        public:
            CommandQueue( wxWindow* window );
            ~CommandQueue();

        public:
            void Post( VoidSignature::Delegate delegate );
            void Flush();

        protected:
            void HandleEvent( wxCommandEvent& event );

        private:
            wxWindow*                               m_Window;   // the windows to use (for custom message dispatch)
            std::vector< VoidSignature::Delegate >  m_Commands; // the commands to redo when flushing
        };
    }
}
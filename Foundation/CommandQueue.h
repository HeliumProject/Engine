#pragma once

#include "Platform/Mutex.h"

#include "Foundation/API.h"
#include "Foundation/Automation/Event.h"

namespace Helium
{
    //
    // Designed to be aggregated into an editor, this class
    //  allows deferred command execution to allow performing
    //  commands to coexist easier with issuing commands in callbacks
    //

    class FOUNDATION_API CommandQueue
    {
    public:
        CommandQueue();
        virtual ~CommandQueue();

    public:
        void Post( VoidSignature::Delegate delegate );
        void Flush();

    protected:
        virtual void EnqueueFlush()
        {
            // override this to manually defer the Flush() call (instead of calling it explicitly in an update loop or timer)
        }

        std::vector< VoidSignature::Delegate >  m_Commands; // the commands to redo when flushing
        Helium::Mutex                           m_Mutex;    // to protect the command container
    };
}
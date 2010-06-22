#pragma once

#include <stack>

#include "Foundation/API.h"
#include "Platform/Compiler.h"

namespace Nocturnal
{
    // Callbacks for external APIs
    typedef void (*InitializeFunc)();
    typedef void (*CleanupFunc)();

    //
    // Instance-able InitializerStack class encapsulates a stack of init/cleanup functions
    //  Push() adds an init function (and makes the call)
    //  Pop() calls a cleanup function, and pops it from the stack
    //  Clean() pops and calls each cleanup function registered on the stack
    //

    class FOUNDATION_API InitializerStack
    {
    private:
        typedef std::stack< CleanupFunc > InitCleanupStack;
        InitCleanupStack m_InitCleanupStack;
        bool m_AutoCleanup;

    public:
        InitializerStack(bool autoCleanup = false);
        ~InitializerStack();

        // push and call init, call cleanup later
        void Push( InitializeFunc init, CleanupFunc cleanup );

        // just call cleanup later
        void Push( CleanupFunc cleanup )
        {
            Push( NULL, cleanup );
        }

        // pop a cleanup off the stack and call it
        void Pop();

        // pop all cleanup functions off the stack (called automatically in the destructor)
        void Cleanup();
    };
}

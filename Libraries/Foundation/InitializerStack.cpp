#include "InitializerStack.h"

using namespace Nocturnal;

InitializerStack::InitializerStack( bool autoCleanup )
: m_AutoCleanup( autoCleanup )
{

}

InitializerStack::~InitializerStack()
{
    if ( m_AutoCleanup )
    {
        Cleanup();
    }
}

void InitializerStack::Push( InitializeFunc initFunction, CleanupFunc cleanupFunction )
{
    if ( cleanupFunction )
    {
        m_InitCleanupStack.push( cleanupFunction );
    }

    // we allow NULL initializers to just toss something on the uninit stack for cleanup purposes
    if ( initFunction != NULL )
    {
        (*initFunction)();
    }
}

void InitializerStack::Pop()
{
    CleanupFunc cleanup = m_InitCleanupStack.top();
    cleanup();
    m_InitCleanupStack.pop();
}

void InitializerStack::Cleanup()
{
    while ( !m_InitCleanupStack.empty() )
    {
        Pop();
    }
}

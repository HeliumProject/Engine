#include "FoundationPch.h"
#include "InitializerStack.h"

using namespace Helium;

InitializerStack::InitializerStack( bool autoCleanup )
: m_AutoCleanup( autoCleanup )
, m_Count( 0 )
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

#include "FoundationPch.h"
#include "Test.h"

using namespace Helium;
using namespace Helium::RPC;
using namespace Helium::RPC::Test;

Test::TestInterface g_TestInterface;

TestInterface::TestInterface()
: Interface ("Test")
{
    Helium::Signature< TestArgs&>::Delegate delegate ( this, &TestInterface::Test );

    AddInvoker( new InvokerTemplate<TestArgs> ( this, delegate ) );
}

void TestInterface::Test( TestArgs& args )
{

}

void RPC::Test::AddInterface( RPC::Host& host )
{
    host.AddInterface( &g_TestInterface );
}
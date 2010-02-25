#include "RPCTest.h"

using namespace RPC;
using namespace RPC::Test;

Test::TestInterface g_TestInterface;

TestInterface::TestInterface()
: Interface ("Test")
{
  Nocturnal::Signature<void, TestArgs&>::Delegate delegate ( this, &TestInterface::Test );

  AddInvoker( new InvokerTemplate<TestArgs> ( this, delegate ) );
}

void TestInterface::Test( TestArgs& args )
{

}

void RPC::Test::AddInterface( RPC::Host& host )
{
  host.AddInterface( &g_TestInterface );
}
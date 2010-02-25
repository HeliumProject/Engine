#pragma once

#include "rpc.h"

namespace RPC
{
  namespace Test
  {
    struct TestArgs : RPC::Args
    {
      u8 m_char;
      u32 m_integer;
    };
    typedef Nocturnal::Signature<void, TestArgs&>::Delegate TestDelegate;

    class TestInterface : public RPC::Interface
    {
    public:
      TestInterface();

      void Test( TestArgs& args );
    };

    void AddInterface(RPC::Host& host);
  };
}
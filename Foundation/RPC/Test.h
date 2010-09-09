#pragma once

#include "rpc.h"

namespace Helium
{
    namespace RPC
    {
        namespace Test
        {
            struct TestArgs : RPC::Args
            {
                u8 m_char;
                u32 m_integer;
            };
            typedef Helium::Signature< TestArgs&>::Delegate TestDelegate;

            class TestInterface : public RPC::Interface
            {
            public:
                TestInterface();

                void Test( TestArgs& args );
            };

            void AddInterface(RPC::Host& host);
        };
    }
}
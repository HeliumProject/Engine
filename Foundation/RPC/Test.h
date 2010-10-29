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
                uint8_t m_char;
                uint32_t m_integer;
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
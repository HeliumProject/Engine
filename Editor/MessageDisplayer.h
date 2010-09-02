#pragma once

#include "Foundation/Message.h"

namespace Helium
{
    namespace Editor
    {
        class MessageDisplayer
        {
        public:
            MessageDisplayer( wxWindow* parent = NULL )
                : m_Parent( parent )
            {

            }

            void SetParent( wxWindow* parent )
            {
                m_Parent = parent;
            }

            void DisplayMessage( const MessageArgs& args );

        private:
            wxWindow* m_Parent;
        };
    }
}
#pragma once

#include "Foundation/Event.h"

namespace Helium
{
    //
    // Define a simple event-driven interface for message dialogs and questions
    //

    namespace MessagePriorities
    {
        enum MessagePriority
        {
            Message,
            Question,
            Warning,
            Error
        };
    }
    typedef MessagePriorities::MessagePriority MessagePriority;

    namespace MessageAppearances
    {
        enum MessageAppearance
        {
            Ok,
            YesNo,
            YesNoCancel,
            YesNoCancelToAll,
        };
    }
    typedef MessageAppearances::MessageAppearance MessageAppearance;

    namespace MessageResults
    {
        enum MessageResult
        {
            Cancel = 0,
            Yes,
            YesToAll,
            No,
            NoToAll
        };
    }
    typedef MessageResults::MessageResult MessageResult;

    struct MessageArgs
    {
        MessageArgs( const tstring& caption, const tstring& message, MessagePriority priority, MessageAppearance appearance )
            : m_Caption( caption )
            , m_Message( message )
            , m_Priority( priority )
            , m_Appearance( appearance )
            , m_Result( MessageResults::Cancel )
        {

        }

        tstring                 m_Caption;
        tstring                 m_Message;
        MessagePriority         m_Priority;
        MessageAppearance       m_Appearance;
        mutable MessageResult   m_Result;
    };

    typedef Helium::Signature< const MessageArgs& > MessageSignature;
}
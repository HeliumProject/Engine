#include "Platform/API.h"
#include "Connection.h"

#include "Platform/Assert.h"

#include <string.h>

using namespace Helium::IPC;

namespace Helium
{
    namespace IPC
    {
        namespace ProtocolMessageIDs
        {
            enum ProtocolMessageID
            {
                Disconnect,
            };
        }
    }
}

// Debug printing
//#define IPC_CONNECTION_DEBUG

static const tchar* ConnectionStateNames[] = 
{
    TXT( "Waiting" ),
    TXT( "Active" ),
    TXT( "Closed" ),
    TXT( "Failed" ),
};

HELIUM_COMPILE_ASSERT( ConnectionStates::Count == (sizeof(ConnectionStateNames) / sizeof(const tchar*)) );

Connection::Connection()
: m_Server (false)
, m_Terminating (false)
, m_State (ConnectionStates::Closed)
, m_ConnectCount (0)
, m_RemotePlatform ((Platform::Type)-1)
, m_NextTransaction (0)
{
    SetState(ConnectionStates::Closed);
}

Connection::~Connection()
{

}

bool Connection::Initialize(bool server, const tchar* name)
{
    _tcscpy(m_Name, name);
    m_Server = server;

    if (server)
    {
        m_NextTransaction = -1;
    }
    else
    {
        m_NextTransaction = 1;
    }

    return true;
}

void Connection::Cleanup()
{
    if (!m_Terminating)
    {
        m_Terminating = true;
        m_Terminate.Signal();

        if (m_ConnectThread.Valid())
        {
            // wait for them to quit
            m_ConnectThread.Wait();

            // close handle
            m_ConnectThread.Close();
        }

        m_ReadQueue.Clear();
        m_WriteQueue.Clear();

        SetState(ConnectionStates::Closed);

        m_Terminating = false;
        m_Terminate.Reset();
    }
}

void Connection::SetState(ConnectionState state)
{
    Platform::TakeMutex mutex (m_Mutex);

    if (m_State != state)
    {
        // report status change
#ifdef IPC_CONNECTION_DEBUG
        Platform::Print("%s: State changing from '%s' to '%s'\n", m_Name, ConnectionStateNames[m_State], ConnectionStateNames[state]);
#else
        if (m_State != ConnectionStates::Active && state == ConnectionStates::Active)
        {
            Platform::Print( TXT( "%s: Connected\n" ), m_Name);
        }

        if (m_State == ConnectionStates::Active && state != ConnectionStates::Active)
        {
            Platform::Print( TXT( "%s: Disconnected\n" ), m_Name);
        }

        if (m_State == ConnectionStates::Active && state == ConnectionStates::Waiting)
        {
            Platform::Print( TXT( "%S: Waiting for connection\n" ), m_Name);
        }
#endif

        // inrement our connect count if necessary
        if (m_State != ConnectionStates::Active && state == ConnectionStates::Active)
        {
            m_ConnectCount++;
        }

        // set the state
        m_State = state;
    }
}

Message* Connection::CreateMessage(u32 id, u32 size, i32 trans, u32 type)
{
    HELIUM_ASSERT(m_NextTransaction != 0);

    if (trans == 0)
    {
        if (m_Server)
        {
            trans = m_NextTransaction--;
        }
        else
        {
            trans = m_NextTransaction++;
        }
    }

    IPC::Message* msg = new Message (id, trans, size, type);

    if (!msg)
    {
        Platform::Print( TXT( "%s: Failed to create message (ID: %u, TRN: %u, Size: %u)\n" ), m_Name, id, trans, size);
    }

    return msg;
}

bool Connection::CreatedMessage(i32 transaction)
{
    if (m_Server)
    {
        return transaction < 0;
    }
    else
    {
        return transaction > 0;
    }
}

void Connection::Wait()
{
    m_ReadQueue.Wait();
}

ConnectionState Connection::Send(Message* message)
{
    ConnectionState result = GetState();

    if (result != ConnectionStates::Active)
    {
        return result;
    }

    m_WriteQueue.Add(message);

    return result;
}

ConnectionState Connection::Receive(Message** msg, bool wait)
{
    ConnectionState result = GetState();

    if (result != ConnectionStates::Active)
    {
        if (msg)
        {
            *msg = 0;
        }

        return result;
    }

    if (msg)
    {
        if (wait || m_ReadQueue.Count())
        {
            *msg = m_ReadQueue.Remove();
        }
    }

    return result;
}

void Connection::CleanupThread()
{
    // nothing to do by default
}

bool Connection::ReadPump()
{
    Message* msg = NULL;

    // attempt to read the message from the connction
    bool result = ReadMessage(&msg);

    // success, add it to the incoming queue if it exists
    if (msg)
    {
        if ( msg->GetType() == MessageTypes::Protocol )
        {
            ProcessProtocolMessage(msg);
        }
        else
        {
            m_ReadQueue.Add(msg);
        }

#ifdef IPC_CONNECTION_DEBUG
        Platform::Print("%s: Got message %d, id '%d', transaction '%d', size '%d'\n", m_Name, msg->GetNumber(), msg->GetID(), msg->GetTransaction(), msg->GetSize());
#endif
    }

    if (!result)
    {
        // close the connection
        SetState(ConnectionStates::Closed);
    }

    return result;
}

bool Connection::WritePump()
{
    bool result = false;

    Message* msg = m_WriteQueue.Remove();
    if (msg)
    {
        // the result will be true unless there was heinous breakage
        result = WriteMessage(msg);

#ifdef IPC_CONNECTION_DEBUG
        Platform::Print("%s: Put message %d, id '%d', transaction '%d', size '%d'\n", m_Name, msg->GetNumber(), msg->GetID(), msg->GetTransaction(), msg->GetSize());
#endif

        // free the memory
        delete msg;
    }

    // there was a failure
    if (!result)
    {
        // close the connection
        SetState(ConnectionStates::Closed);
    }

    return result;
}

void Connection::ReadThread()
{
    while (1)
    {
        if (GetState() == ConnectionStates::Closed || m_Terminating)
        {
            break;
        }

        if (!ReadPump())
        {
            break;
        }
    }

    CleanupThread();

    return;
}

void Connection::WriteThread()
{
    while (1)
    {
        if (GetState() == ConnectionStates::Closed || m_Terminating)
        {
            break;
        }

        if (!WritePump())
        {
            break;
        }
    }

    CleanupThread();

    return;
}

void Connection::ConnectThread()
{
    if (m_Server)
    {
        // client write first
        if ( !ReadHostType() )
        {
            return;
        }

        // server write second
        if ( !WriteHostType() )
        {
            return;
        }
    }
    else
    {
        // client write first
        if ( !WriteHostType() )
        {
            return;
        }

        // server write second
        if ( !ReadHostType() )
        {
            return;
        }
    }

    // we have handshaked, go active
    SetState(ConnectionStates::Active);

    // report our remote platform type
    Platform::Print( TXT( "%s: Remote platform is '%s'\n" ), m_Name, Platform::GetTypeName(m_RemotePlatform));

    // start read thread
    if (!m_ReadThread.Create(&Platform::Thread::EntryHelper<Connection, &Connection::ReadThread>, this, TXT( "IPC Read Thread" )))
    {
        HELIUM_BREAK();
        return;
    }

    // start write thread
    if (!m_WriteThread.Create(&Platform::Thread::EntryHelper<Connection, &Connection::WriteThread>, this, TXT( "IPC Write Thread" )))
    {
        HELIUM_BREAK();
        return;
    }

    // wait for the read thread to quit
    m_ReadThread.Wait();
    m_ReadThread.Close();

    // wake up any reader blocking waiting for messages
    m_ReadQueue.Add(NULL);

    // wake up the writer thread so that it detects the disconnect
    m_WriteQueue.Add(NULL);

    // wait for the write thread to quit
    m_WriteThread.Wait();
    m_WriteThread.Close();

    // erase messages
    m_ReadQueue.Clear();
    m_WriteQueue.Clear();

    // send the disconnect message
    SendProtocolMessage( ProtocolMessageIDs::Disconnect );

    CleanupThread();
}

void Connection::ProcessProtocolMessage( Message* msg )
{
    if ( msg )
    {
        switch( msg->m_ID )
        {
        case ProtocolMessageIDs::Disconnect:
            {
                SetState(ConnectionStates::Closed);
                break;
            }
        }

        delete msg;
    }
}

void Connection::SendProtocolMessage( u32 message )
{
    Message* msg = CreateMessage( ProtocolMessageIDs::Disconnect, 0, 0, message );
    if (msg)
    {
        WriteMessage(msg);
        delete msg;
    }
}

bool Connection::ReadHostType()
{
    u8 byte;

    if (!Read(&byte, sizeof(byte)))
    {
        Platform::Print( TXT( "%s: Failed to read remote platform type!\n" ), m_Name);
        return false;
    }

    m_RemotePlatform = (Platform::Type)byte;

    return true;
}

bool Connection::WriteHostType()
{
    u8 byte = (u8)Platform::GetType();
    if (!Write(&byte, sizeof(byte)))
    {
        Platform::Print( TXT( "%s: Failed to write remote Platform type!\n" ), m_Name);
        return false;
    }

    return true;
}

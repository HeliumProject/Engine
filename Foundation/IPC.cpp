#include "FoundationPch.h"
#include "IPC.h"

#include "Platform/Assert.h"
#include "Platform/Console.h"

#include "Foundation/String.h"

#include <string.h>

using namespace Helium;
using namespace Helium::IPC;

Message::Message(uint32_t id, int32_t trn, uint32_t size, MessageType type)
: MessageHeader( id, trn, size, type )
, m_Next (NULL)
, m_Number (0)
{
    if (size)
    {
        m_Data = new uint8_t[size];
    }
    else
    {
        m_Data = 0;
    }
}

Message::~Message()
{
    if (m_Data)
    {
        delete [] m_Data;
        m_Data = 0;
    }
}

MessageQueue::MessageQueue()
: m_Head (0)
, m_Tail (0)
, m_Count (0)
, m_Total (0)
{

}

MessageQueue::~MessageQueue()
{
    Clear();
}

void MessageQueue::Add(Message* msg)
{
    IPC_SCOPE_TIMER("");

    if (msg)
    {
        Helium::MutexScopeLock mutex (m_Mutex);

        if (m_Tail == 0)
        {
            // header must also be zero and count must be zero
            if(m_Count !=0)
            {
                HELIUM_BREAK();
            }
            if (m_Head != 0)
            {
                HELIUM_BREAK();
            }

            msg->m_Next = 0;
            m_Head = msg;
            m_Tail = msg;
        }
        else
        {
            Message* end = m_Tail;

            // tail points to new message
            m_Tail = msg;

            // old end message points to us
            end->m_Next = msg;

            // we point to null
            msg->m_Next = 0;       
        }

        m_Count++;
        m_Total++;
        msg->SetNumber( m_Total );
    }

    m_Append.Increment();
}

Message* MessageQueue::Remove()
{
    IPC_SCOPE_TIMER("");

    m_Append.Decrement();

    Helium::MutexScopeLock mutex (m_Mutex);

    Message* result = 0;
    if (m_Head != 0)
    {
        m_Count--;

        // take the head of the queue
        result = m_Head;

        // move the the head dow
        m_Head = m_Head->m_Next;

        // if the head points to null, set the tail to null because we have popped the last entry
        if (m_Head == 0)
        {
            m_Tail = 0;
        }
    }

    return result;
}

void MessageQueue::Clear()
{
    IPC_SCOPE_TIMER("");

    Helium::MutexScopeLock mutex (m_Mutex);

    Message* msg = m_Head;
    while (msg)
    {
        Message* next = msg->m_Next;
        delete msg;
        msg = next;
    }

    m_Head = 0;
    m_Tail = 0;
    m_Count = 0;
    m_Total = 0;

    m_Append.Increment();
    m_Append.Reset();
}

uint32_t MessageQueue::Count()
{
    return m_Count;
}

uint32_t MessageQueue::Total()
{
    return m_Total;
}

void MessageQueue::Wait()
{
    // this will send the calling thread to sleep, and when it returns the semaphore value will be decremented for *this* thread
    m_Append.Decrement();

    // this will be zero if the queue was reset by another thread
    if ( m_Total )
    {
        // do an increment to preserve the semaphore's value over the call to this function
        m_Append.Increment();
    }
}

namespace ProtocolMessageIDs
{
    enum ProtocolMessageID
    {
        Disconnect,
    };
}

// Debug printing
//#define IPC_CONNECTION_DEBUG

static const tchar_t* ConnectionStateNames[] = 
{
    TXT( "Waiting" ),
    TXT( "Active" ),
    TXT( "Closed" ),
    TXT( "Failed" ),
};

HELIUM_COMPILE_ASSERT( ConnectionStates::Count == (sizeof(ConnectionStateNames) / sizeof(const tchar_t*)) );
Localization::StringTable Connection::s_StringTable( "Helium::IPC::Connection" );
bool Connection::s_RegisteredStringTable = false;

Connection::Connection()
: m_Server (false)
, m_Terminating (false)
, m_Terminate (true, false)
, m_State (ConnectionStates::Closed)
, m_ConnectCount (0)
, m_RemotePlatform ((Helium::Platform::Type)-1)
, m_NextTransaction (0)
{
    SetState(ConnectionStates::Closed);

    if ( !s_RegisteredStringTable )
    {
        s_StringTable.AddString( "en", "Connected", TXT( "<NAME>: Connected\n" ) );
        s_StringTable.AddString( "en", "Disconnected", TXT( "<NAME>: Disonnected\n" ) );
        s_StringTable.AddString( "en", "Waiting", TXT( "<NAME>: Waiting for connection\n" ) );
        s_StringTable.AddString( "en", "MessageCreateFailed", TXT( "<NAME>: Failed to create message ( ID: <ID>, TRN: <TRANS>, Size: <SIZE> )\n" ) );
        s_StringTable.AddString( "en", "RemotePlatform", TXT( "<NAME>: Remote platform is '<PLATFORM>'\n" ) );
        s_StringTable.AddString( "en", "RemotePlatformTypeReadFailed", TXT( "<NAME>: Failed to read remote platform type!\n" ) );
        s_StringTable.AddString( "en", "RemotePlatformTypeWriteFailed", TXT( "<NAME>: Failed to write remote Platform type!\n" ) );
  
        Localization::GlobalLocalizer().RegisterTable( &s_StringTable );
        s_RegisteredStringTable = true;
    }
}

Connection::~Connection()
{
}

bool Connection::Initialize(bool server, const tchar_t* name)
{
    CopyString(m_Name, name);
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

        m_ConnectThread.Join();

        m_ReadQueue.Clear();
        m_WriteQueue.Clear();

        SetState(ConnectionStates::Closed);

        m_Terminating = false;
        m_Terminate.Reset();
    }
}

void Connection::SetState(ConnectionState state)
{
    Helium::MutexScopeLock mutex (m_Mutex);

    if (m_State != state)
    {
        // report status change
#ifdef IPC_CONNECTION_DEBUG
        Helium::Print("%s: State changing from '%s' to '%s'\n", m_Name, ConnectionStateNames[m_State], ConnectionStateNames[state]);
#else
        if (m_State != ConnectionStates::Active && state == ConnectionStates::Active)
        {
            Localization::Statement stmt( "Helium::IPC::Connection", "Connected" );
            stmt.ReplaceKey( TXT( "NAME" ), m_Name );
            Helium::Print( stmt.Get().c_str() );
        }

        if (m_State == ConnectionStates::Active && state != ConnectionStates::Active)
        {
            Localization::Statement stmt( "Helium::IPC::Connection", "Disconnected" );
            stmt.ReplaceKey( TXT( "NAME" ), m_Name );
            Helium::Print( stmt.Get().c_str() );
        }

        if (m_State == ConnectionStates::Active && state == ConnectionStates::Waiting)
        {
            Localization::Statement stmt( "Helium::IPC::Connection", "Waiting" );
            stmt.ReplaceKey( TXT( "NAME" ), m_Name );
            Helium::Print( stmt.Get().c_str() );
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

Message* Connection::CreateMessage(uint32_t id, uint32_t size, int32_t trans, MessageType type)
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
        Localization::Statement stmt( "Helium::IPC::Connection", "MessageCreateFailed" );
        stmt.ReplaceKey( TXT( "NAME" ), m_Name );
        stmt.ReplaceKey( TXT( "ID" ), id );
        stmt.ReplaceKey( TXT( "TRANS" ), trans );
        stmt.ReplaceKey( TXT( "SIZE" ), size );

        Helium::Print( stmt.Get().c_str() );
    }

    return msg;
}

bool Connection::CreatedMessage(int32_t transaction)
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
        Helium::Print("%s: Got message %d, id '%d', transaction '%d', size '%d'\n", m_Name, msg->GetNumber(), msg->GetID(), msg->GetTransaction(), msg->GetSize());
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
        Helium::Print("%s: Put message %d, id '%d', transaction '%d', size '%d'\n", m_Name, msg->GetNumber(), msg->GetID(), msg->GetTransaction(), msg->GetSize());
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
    
    Localization::Statement stmt( "Helium::IPC::Connection", "RemotePlatform" );
    stmt.ReplaceKey( TXT( "NAME" ), m_Name );
    stmt.ReplaceKey( TXT( "PLATFORM" ), Helium::Platform::GetTypeName(m_RemotePlatform) );

    Helium::Print( stmt.Get().c_str() );

    // start read thread
    Helium::CallbackThread::Entry readEntry = &Helium::CallbackThread::EntryHelper<Connection, &Connection::ReadThread>;
    if (!m_ReadThread.Create( readEntry, this, TXT("IPC Read Thread")))
    {
        HELIUM_BREAK();
        return;
    }

    // start write thread
    Helium::CallbackThread::Entry writeEntry = &Helium::CallbackThread::EntryHelper<Connection, &Connection::WriteThread>;
    if (!m_WriteThread.Create( writeEntry, this, TXT("IPC Write Thread")))
    {
        HELIUM_BREAK();
        return;
    }

    // wait for the read thread to quit
    m_ReadThread.Join();

    // wake up any reader blocking waiting for messages
    m_ReadQueue.Add(NULL);

    // wake up the writer thread so that it detects the disconnect
    m_WriteQueue.Add(NULL);

    // wait for the write thread to quit
    m_WriteThread.Join();

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

void Connection::SendProtocolMessage( uint32_t message )
{
    Message* msg = CreateMessage( message, 0, 0, MessageTypes::Protocol );
    if (msg)
    {
        WriteMessage(msg);
        delete msg;
    }
}

bool Connection::ReadHostType()
{
    uint8_t byte;

    if (!Read(&byte, sizeof(byte)))
    {
        Localization::Statement stmt( "Helium::IPC::Connection", "RemotePlatformTypeReadFailed" );
        stmt.ReplaceKey( TXT( "NAME" ), m_Name );

        Helium::Print( stmt.Get().c_str() );
        return false;
    }

    m_RemotePlatform = (Helium::Platform::Type)byte;

    return true;
}

bool Connection::WriteHostType()
{
    uint8_t byte = (uint8_t)Helium::Platform::GetType();
    if (!Write(&byte, sizeof(byte)))
    {
        Localization::Statement stmt( "Helium::IPC::Connection", "RemotePlatformTypeWriteFailed" );
        stmt.ReplaceKey( TXT( "NAME" ), m_Name );

        Helium::Print( stmt.Get().c_str() );
        return false;
    }

    return true;
}

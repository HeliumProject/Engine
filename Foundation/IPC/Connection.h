#pragma once

#include "Message.h"

#include "Platform/Event.h"
#include "Platform/Thread.h"
#include "Platform/Platform.h"

namespace IPC
{
    namespace ConnectionStates
    {
        enum ConnectionState
        {
            Waiting,
            Active,
            Closed,
            Failed,
            Count,
        };
    }
    typedef ConnectionStates::ConnectionState ConnectionState;

    namespace MessageTypes
    {
        enum MessageType
        {
            Protocol = 0,
            User,
        };
    }
    typedef MessageTypes::MessageType MessageType;

    class FOUNDATION_API Connection
    {
    protected:
        tchar                   m_Name[256];          // friendly name for this connection
        bool                    m_Server;             // are we the server side or the client side
        bool                    m_Terminating;        // used by the closedown code to signal it wants the threads to terminate
        Helium::Event           m_Terminate;          // used to wake up sleeping threads for when we want to terminate

        ConnectionState         m_State;              // current status, do not change outside of m_Mutex 
        u32                     m_ConnectCount;       // track the number of connection that have occured
        Helium::Platform::Type  m_RemotePlatform;     // the platform of the end point on the other side
        i32                     m_NextTransaction;    // next transaction id for this connection endpoint

        Helium::Mutex           m_Mutex;              // mutex to protect access to this class
        MessageQueue            m_ReadQueue;          // incoming messages
        MessageQueue            m_WriteQueue;         // outgoing messages

        Helium::Thread          m_ConnectThread;      // handle of the core thread that manages the connection, once
        Helium::Thread          m_ReadThread;         // handle of the thread reads from the pipe (incomming)
        Helium::Thread          m_WriteThread;        // handle of the thread that writes to the pipe (outgoing)

        MessageHeader           m_ReadHeader;
        MessageHeader           m_WriteHeader;

    public:
        Connection();
        virtual ~Connection();

    protected:
        bool Initialize(bool server, const tchar* name);

    public:
        void Cleanup();


        //
        // State
        //

    public:
        ConnectionState GetState()
        {
            return m_State;
        }

    protected:
        void SetState(ConnectionState state);


        //
        // Access
        //

    public:
        Helium::Platform::Type GetRemotePlatform()
        {
            return m_RemotePlatform;
        }

        u32 GetConnectCount()
        {
            return m_ConnectCount;
        }


        //
        // Message interface
        //  To keep transaction numbers under control, all message creation (and querying) is done here.
        //

    public:
        // create a message to reply to a transaction
        Message* CreateMessage(u32 id, u32 size, i32 trans = 0, u32 type = MessageTypes::User);

        // did this endpoint create the specified transaction
        bool CreatedMessage(i32 transaction);

        // wait in the calling thread for a message
        void Wait();


        //
        //  Send
        //
        //  Adds a message to the send queue and returns immdiately to the caller. Once a message has
        //  been added to the queue the caller no longer owns it and must not delete it. the message will
        //  be sent to the other side of the connection in order with respect to other sent messages.
        //  If the return code from Send() is not ConnectionStates::Active then the message was not
        //  accepted and was not added to the queue, in this situation the caller still owns the message
        //  and is free to do what it wishes, it can try again or simply delete the message should it
        //  wish.
        //
        virtual ConnectionState Send(Message* msg);


        //
        //  Receive
        //
        //  Gets a message from the input queue, the pointer is filled in with the address and once
        //  you have it you own it and must delete it when you are finished with it. If there are no
        //  messages in the queue or the connection is not active the pointer is set to NULL.
        //
        virtual ConnectionState Receive(Message** msg, bool wait = false);


        //
        // Read/Write data
        //

    protected:
        // Do any necessary cleanup on the current thread
        virtual void CleanupThread();

        // These synchronously perform a single message read or write operation
        virtual bool ReadMessage(Message** msg) = 0;
        virtual bool WriteMessage(Message* msg) = 0;

        // These synchronously read or write data through the connection
        virtual bool Read(void* buffer, u32 bytes) = 0;
        virtual bool Write(void* buffer, u32 bytes) = 0;

        // ReadPump blocks on incoming data for message creation
        bool ReadPump();

        // WritePump blocks on messages being appended to the write queue
        bool WritePump();

        // ReadThread and WriteThread run the read and write pumps until the
        //  connection fails or our connection object is destructed
        void ReadThread();
        void WriteThread();

        // ConnectThread is the body of the connection thread for clients and servers.
        //  It exchanges the platform information and waits for the connection to break,
        //  then returns.
        void ConnectThread();

        // Processes a protocol message (disconnect/handshake/etc.)
        void ProcessProtocolMessage(Message* msg);

        // Send the disconnect message
        void SendProtocolMessage(u32 message);

        // Send host type message
        bool WriteHostType();

        // Receive host type message
        bool ReadHostType();
    };
}

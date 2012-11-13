#pragma once

#include "Platform/Condition.h"
#include "Platform/Locks.h"
#include "Platform/Runtime.h"
#include "Platform/Semaphore.h"
#include "Platform/Thread.h"

#include "Foundation/IPC.h"
#include "Foundation/Localization.h"
#include "Foundation/Profile.h"

//#define IPC_PROFILE

#if defined(PROFILE_INSTRUMENT_ALL) || defined(IPC_PROFILE)
# define IPC_SCOPE_TIMER(x) PROFILE_SCOPE_TIMER(x)
#else
# define IPC_SCOPE_TIMER(x)
#endif

namespace Helium
{
	namespace IPC
	{
		namespace MessageTypes
		{
			enum MessageType
			{
				Protocol = 0,
				User,
			};
		}
		typedef MessageTypes::MessageType MessageType;

		class HELIUM_FOUNDATION_API MessageHeader
		{
		public:
			uint32_t m_ID;
			int32_t m_TRN;
			uint32_t m_Size;
			MessageType m_Type;

			MessageHeader()
				: m_ID( 0 )
				, m_TRN( 0 )
				, m_Size( 0 )
				, m_Type( MessageTypes::User )
			{
			}

			MessageHeader(uint32_t id, int32_t trn, uint32_t size, MessageType type = MessageTypes::User )
				: m_ID( id )
				, m_TRN( trn )
				, m_Size( size )
				, m_Type( type )
			{
			}
		};

		class HELIUM_FOUNDATION_API Message : private MessageHeader
		{
			friend class Connection;
			friend class MessageQueue;

		private:
			Message*	m_Next;
			uint32_t	m_Number;
			uint8_t*	m_Data;

		private:
			Message(uint32_t id, int32_t trans, uint32_t size, MessageType type = MessageTypes::User);

		public:
			~Message();

			uint32_t GetNumber() const
			{
				return m_Number;
			}

			void SetNumber(uint32_t n)
			{
				m_Number = n;
			}

			uint32_t GetID() const
			{
				return m_ID;
			}

			int32_t GetTransaction() const
			{
				return m_TRN;
			}

			uint32_t GetSize() const
			{
				return m_Size;
			}

			MessageType GetType() const
			{
				return m_Type;
			}

			uint8_t* GetData() const
			{
				return m_Data;
			}

			uint8_t* TakeData()
			{
				uint8_t* data = m_Data;
				m_Data = NULL;
				return data;
			}
		};

		class HELIUM_FOUNDATION_API MessageQueue
		{
		private:
			Message* m_Head;			// pointer to head message 
			Message* m_Tail;			// pointer to tail message 
			uint32_t m_Count;			// number of messages in queue
			uint32_t m_Total;			// number of messages that have passed through the queue since clear

			Helium::Mutex m_Mutex;      // mutex to control access to the queue
			Helium::Semaphore m_Append; // semaphore that increments on add, decrements on remove

		public:
			MessageQueue();
			~MessageQueue();

			void Add(Message*);
			Message* Remove();
			void Clear();
			uint32_t Count();
			uint32_t Total();
			void Wait();
		};
		
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

		class HELIUM_FOUNDATION_API Connection
		{
		protected:
			static Localization::StringTable s_StringTable;
			static bool                      s_RegisteredStringTable;

			tchar_t					m_Name[256];          // friendly name for this connection
			bool					m_Server;             // are we the server side or the client side
			bool                    m_Terminating;        // used by the closedown code to signal it wants the threads to terminate
			Helium::Condition       m_Terminate;          // used to wake up sleeping threads for when we want to terminate

			ConnectionState         m_State;              // current status, do not change outside of m_Mutex 
			uint32_t                m_ConnectCount;       // track the number of connection that have occured
			Helium::Platform::Type  m_RemotePlatform;     // the platform of the end point on the other side
			int32_t                 m_NextTransaction;    // next transaction id for this connection endpoint

			Helium::Mutex           m_Mutex;              // mutex to protect access to this class
			MessageQueue            m_ReadQueue;          // incoming messages
			MessageQueue            m_WriteQueue;         // outgoing messages

			Helium::CallbackThread  m_ConnectThread;      // handle of the core thread that manages the connection, once
			Helium::CallbackThread  m_ReadThread;         // handle of the thread reads from the pipe (incomming)
			Helium::CallbackThread  m_WriteThread;        // handle of the thread that writes to the pipe (outgoing)

			MessageHeader           m_ReadHeader;
			MessageHeader           m_WriteHeader;

		public:
			Connection();
			virtual ~Connection();

		protected:
			bool Initialize(bool server, const tchar_t* name);

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

			uint32_t GetConnectCount()
			{
				return m_ConnectCount;
			}


			//
			// Message interface
			//  To keep transaction numbers under control, all message creation (and querying) is done here.
			//

		public:
			// create a message to reply to a transaction
			Message* CreateMessage(uint32_t id, uint32_t size, int32_t trans = 0, MessageType type = MessageTypes::User);

			// did this endpoint create the specified transaction
			bool CreatedMessage(int32_t transaction);

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
			virtual bool Read(void* buffer, uint32_t bytes) = 0;
			virtual bool Write(void* buffer, uint32_t bytes) = 0;

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
			void SendProtocolMessage(uint32_t message);

			// Send host type message
			bool WriteHostType();

			// Receive host type message
			bool ReadHostType();
		};
	}
}
#pragma once

#include "API.h"
#include "Swizzle.h"
#include "Platform/Platform.h"

#include "Common/Types.h"
#include "Common/Memory/Endian.h"
#include "Common/Automation/Event.h"

namespace IPC
{
  class Message;
  class Connection;
}

namespace RPC
{
  class Invoker;
  class Interface;
  class Host;

  const u32 MAX_STACK = 64;
  const u32 MAX_INVOKERS = 32;
  const u32 MAX_INTERFACES = 32;
  const i32 TIMEOUT_DEFAULT = 1000;
  const i32 TIMEOUT_FOREVER = -1;


  //
  // Args structure is the base class for all args
  //

  namespace Flags
  {
    enum Flag
    {
      NonBlocking         = 1 << 0, // do no block waiting for a reply
      ReplyWithArgs       = 1 << 1, // args data should be returned (potentially changed)
      ReplyWithPayload    = 1 << 2, // payload data should be returned (potentially changed)
    };
  }

  struct RPC_API Args
  {
    Host* m_Host;
    u32   m_Flags;

    void* m_Payload;
    u32   m_PayloadSize;
  };
  typedef Nocturnal::Signature<void, RPC::Args&>::Delegate ArgsDelegate;


  //
  // Invoker:
  //  - packages an invocation for dispatch to a remote implementation
  //  - performs invocation on locally defined virtual implementation
  //

  class Invoker : public Nocturnal::RefCountBase< Invoker >
  {
  public:
    Invoker (Interface* interface, SwizzleFunc swizzler)
      : m_Interface (interface)
      , m_Swizzler (swizzler)
    {
      NOC_ASSERT( interface && swizzler );
    }

    virtual ~Invoker()
    {

    }

    virtual u32 GetArgsSize()
    {
      return 0;
    }

    virtual void Invoke(u8* data, u32 size) = 0;

    const char* GetName()
    {
      return m_Name;
    }

    Interface* GetInterface()
    {
      return m_Interface;
    }

    void Swizzle( void* data )
    {
      m_Swizzler( data );
    }

  protected:
    const char*   m_Name;
    Interface*    m_Interface;
    SwizzleFunc   m_Swizzler;
  };

  typedef Nocturnal::SmartPtr< Invoker > InvokerPtr;


  //
  // Interface is a named group of invokers
  //

  class RPC_API Interface
  {
  public:
    Interface(const char* name);

    Host* GetHost()
    {
      return m_Host;
    }
    void SetHost(Host* host)
    {
      m_Host = host;
    }

    const char* GetName()
    {
      return m_Name;
    }

    void AddInvoker(InvokerPtr invoker);
    Invoker* GetInvoker(const char* name);

  protected:
    const char*   m_Name;
    Host*         m_Host;
    InvokerPtr    m_Invokers[MAX_INVOKERS];
    u32           m_InvokerCount;
  };


  //
  // Host is the endpoint for communication and local store of interfaces
  //

  class RPC_API Host
  {
  public:
    Host();
    ~Host();

    void Reset();

    //
    // Interface managment
    //

    // set/query local implementations
    void AddInterface(RPC::Interface* interface);
    Interface* GetInterface(const char* name);

    //
    // IPC connection settings
    //

    // set the communication connection to use
    void SetConnection(IPC::Connection* con);

    // set the timeout for blocking operations
    void SetTimeout(i32 timeout);

    //
    // Invoker dispatching
    //

    // create message to send
    IPC::Message* Create(Invoker* invoker, u32 size, i32 transaction = 0);

    // helper function to send a single data block
    void Emit(Invoker* invoker, Args* args = NULL, u32 size = 0, SwizzleFunc swizzler = NULL);

    // process data from the other side
    bool Invoke(IPC::Message* msg);

    // take the message data (call from within an interface function)
    u8* TakeData();

    // Are we connected and ready to do work?
    bool Connected();

    // Should we swizzle?
    bool Swizzle();

    // Call this periodically in your to dispatch (invoke) rpc functions within the calling thread and return
    bool Dispatch();

  private:
    // Call this to process all messages in the calling thread, pass true to sleep until the connection breaks
    bool Process(bool wait);

  private:
    struct Frame
    {
      IPC::Message*     m_Message;
      bool              m_MessageTaken;

      bool              m_Replied;
      u32               m_ReplyID;
      u8*               m_ReplyData;
      u32               m_ReplySize;
      i32               m_ReplyTransaction;
    };

    class RPC_API Stack
    {
    public:
      Stack();
      void Reset();
      i32 Size();
      Frame* Push();
      Frame* Top();
      void Pop();

    private:
      Frame             m_Frames[MAX_STACK];
      u32               m_Size;
    };

    IPC::Connection*    m_Connection;
    u32                 m_ConnectionCount;
    i32                 m_Timeout;
    Stack               m_Stack;
    Interface*          m_Interfaces[MAX_INTERFACES];
    u32                 m_InterfaceCount;
  };

  template<class ArgsType>
  class InvokerTemplate : public Invoker
  {
  public:
    typedef Nocturnal::Signature<void, ArgsType&> InvokerSignature;
    typedef typename InvokerSignature::Delegate InvokerDelegate;

    InvokerTemplate(Interface* interface, InvokerDelegate delegate)
      : Invoker (interface, GetSwizzleFunc<ArgsType>())
      , m_Delegate (delegate)
    {

    }

    virtual u32 GetArgsSize()
    {
      return sizeof(ArgsType);
    }

    virtual void Invoke(u8* data, u32 size)
    {
      if (size)
      {
        if (m_Interface->GetHost()->Swizzle())
        {
          m_Swizzler(data);
        }

        ArgsType* args = (ArgsType*)data;

        if ( sizeof(ArgsType) > size )
        {
          args->m_Payload = data + sizeof(ArgsType);
          args->m_PayloadSize = size - sizeof(ArgsType);
        }

        m_Delegate.Invoke( *args );
      }
    }

    void Emit(ArgsType* args, void* payload = NULL, u32 size = 0)
    {
      args->m_Payload = payload;
      args->m_PayloadSize = size;
      m_Interface->m_Host->Emit(this, args, sizeof(ArgsType), m_Swizzler);
    }

  private:
    InvokerDelegate m_Delegate;
  };
}

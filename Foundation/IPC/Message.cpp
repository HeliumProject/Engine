#include "Platform/API.h"
#include "Message.h"
#include "Platform/Assert.h"

using namespace Helium::IPC;

Message::Message(u32 id, i32 trn, u32 size, u32 type)
: MessageHeader( id, trn, size, type )
, m_Next (NULL)
, m_Number (0)
{
    if (size)
    {
        m_Data = new u8[size];
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
        Helium::TakeMutex mutex (m_Mutex);

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

    Helium::TakeMutex mutex (m_Mutex);

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

    Helium::TakeMutex mutex (m_Mutex);

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

u32 MessageQueue::Count()
{
    return m_Count;
}

u32 MessageQueue::Total()
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

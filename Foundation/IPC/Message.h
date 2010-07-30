#pragma once

#include "Platform/Mutex.h"
#include "Platform/Semaphore.h"
#include "Foundation/API.h"
#include "IPC.h"

namespace IPC
{
    class FOUNDATION_API MessageHeader
    {
    public:
        u32 m_ID;
        i32 m_TRN;
        u32 m_Size;
        u32 m_Type;

        MessageHeader()
        {
            if (this)
            {
                m_ID = 0;
                m_TRN = 0;
                m_Size = 0;
                m_Type = 0;
            }
        }

        MessageHeader(u32 id, i32 trn, u32 size, u32 type)
        {
            if (this)
            {
                m_ID = id;
                m_TRN = trn;
                m_Size = size;
                m_Type = type;
            }
        }
    };

    class FOUNDATION_API Message : private MessageHeader
    {
        friend class Connection;
        friend class MessageQueue;

    private:
        Message*  m_Next;
        u32       m_Number;
        u8*       m_Data;

    private:
        Message(u32 id, i32 trans, u32 size, u32 type);

    public:
        ~Message();

        u32 GetNumber() const
        {
            return m_Number;
        }

        void SetNumber(u32 n)
        {
            m_Number = n;
        }

        u32 GetID() const
        {
            return m_ID;
        }

        i32 GetTransaction() const
        {
            return m_TRN;
        }

        u32 GetSize() const
        {
            return m_Size;
        }

        u32 GetType() const
        {
            return m_Type;
        }

        u8* GetData() const
        {
            return m_Data;
        }

        u8* TakeData()
        {
            u8* data = m_Data;
            m_Data = NULL;
            return data;
        }
    };

    class FOUNDATION_API MessageQueue
    {
    private:
        Message* m_Head;    // pointer to head message 
        Message* m_Tail;    // pointer to tail message 
        u32 m_Count;        // number of messages in queue
        u32 m_Total;        // number of messages that have passed through the queue since clear

        Helium::Mutex m_Mutex;      // mutex to control access to the queue
        Helium::Semaphore m_Append; // semaphore that increments on add, decrements on remove

    public:
        MessageQueue();
        ~MessageQueue();

        void Add(Message*);
        Message* Remove();
        void Clear();
        u32 Count();
        u32 Total();
        void Wait();
    };
}

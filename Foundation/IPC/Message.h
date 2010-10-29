#pragma once

#include "Platform/Mutex.h"
#include "Platform/Semaphore.h"
#include "Foundation/API.h"
#include "IPC.h"

namespace Helium
{
    namespace IPC
    {
        class FOUNDATION_API MessageHeader
        {
        public:
            uint32_t m_ID;
            int32_t m_TRN;
            uint32_t m_Size;
            uint32_t m_Type;

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

            MessageHeader(uint32_t id, int32_t trn, uint32_t size, uint32_t type)
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
            uint32_t       m_Number;
            uint8_t*       m_Data;

        private:
            Message(uint32_t id, int32_t trans, uint32_t size, uint32_t type);

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

            uint32_t GetType() const
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

        class FOUNDATION_API MessageQueue
        {
        private:
            Message* m_Head;    // pointer to head message 
            Message* m_Tail;    // pointer to tail message 
            uint32_t m_Count;        // number of messages in queue
            uint32_t m_Total;        // number of messages that have passed through the queue since clear

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
    }
}

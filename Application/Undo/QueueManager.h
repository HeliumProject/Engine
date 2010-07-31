#pragma once

#include <deque>

#include "Queue.h"

namespace Helium
{
    namespace Undo
    {
        typedef std::deque< Queue* > D_QueueDumbPtr;

        ///////////////////////////////////////////////////////////////////////////
        // Manages multiple undo queues.  Push each one into the manager when a
        // command is pushed into a queue.  Then use this class to undo/redo across
        // all the queues in order.
        // 
        class APPLICATION_API QueueManager 
        {
        private:
            size_t m_MaxLength;
            D_QueueDumbPtr m_Undo;
            D_QueueDumbPtr m_Redo;

        public:
            QueueManager();
            virtual ~QueueManager();

            void Reset();
            size_t GetLength() const;
            void SetMaxLength( size_t length );
            size_t GetMaxLenth() const;
            bool CanUndo() const;
            bool CanRedo() const;
            void Push( Queue* queue );
            void Undo();
            void Redo();

        private:
            void Resize();

        private:
            void QueueDestroyed( const QueueChangeArgs& args );
        };
    }
}
#pragma once

#include "Foundation/Automation/Event.h"
#include "BatchCommand.h"

namespace Helium
{
    namespace Undo
    {
        class Queue;
        struct APPLICATION_API QueueChangeArgs
        {
            Queue* m_Queue;
            const Command* m_Command;

            QueueChangeArgs( Queue* queue, const Command* command )
                : m_Queue( queue )
                , m_Command( command )
            {

            }
        };
        typedef Helium::Signature< void, const QueueChangeArgs& > QueueChangeSignature;
        typedef Helium::Signature< bool, const QueueChangeArgs& > QueueChangingSignature;

        class APPLICATION_API Queue
        {
            //
            // Members
            //

        private:
            // The undo and redo stacks
            V_CommandSmartPtr m_Undo;
            V_CommandSmartPtr m_Redo;

            // is the queue active, we don't want to modify the queue while we are commiting a change
            bool m_Active;

            // max allowed length of the queue
            int m_MaxLength;

            // the batch state
            int m_BatchState;

            // the batch
            BatchCommandPtr m_Batch;


            //
            // Constructor
            //

        public:
            Queue();
            ~Queue();

            void Reset();

            void Print() const;


            //
            // Accessors
            //

            bool IsActive() const;

            int GetLength() const;

            int GetMaxLength() const;

            void SetMaxLength(int value);


            //
            // Auto-Batching
            //

            // are we batching?
            bool IsBatching() const;

            // increments batch state
            void BeginBatch();

            // decrements batch state and pushes batch
            void EndBatch();


            //
            // Main program interfaces
            //

            void Push(const CommandPtr& c);

            bool CanUndo() const;

            bool CanRedo() const;

            void Undo();

            void Redo();


            // 
            // Events
            //

        private:
            QueueChangeSignature::Event m_CommandPushed;
        public:
            void AddCommandPushedListener( const QueueChangeSignature::Delegate& listener )
            {
                m_CommandPushed.Add( listener );
            }
            void RemoveCommandPushedListener( const QueueChangeSignature::Delegate& listener )
            {
                m_CommandPushed.Remove( listener );
            }

        private:
            QueueChangeSignature::Event m_Reset;
        public:
            void AddResetListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Reset.Add( listener );
            }
            void RemoveResetListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Reset.Remove( listener );
            }

        private:
            QueueChangeSignature::Event m_Destroyed;
        public:
            void AddDestroyListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Destroyed.Add( listener );
            }
            void RemoveDestroyListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Destroyed.Remove( listener );
            }

        private:
            QueueChangingSignature::Event m_Undoing;
        public:
            void AddUndoingListener( const QueueChangingSignature::Delegate& listener )
            {
                m_Undoing.Add( listener );
            }
            void RemoveUndoingListener( const QueueChangingSignature::Delegate& listener )
            {
                m_Undoing.Remove( listener );
            }

        private:
            QueueChangingSignature::Event m_Redoing;
        public:
            void AddRedoingListener( const QueueChangingSignature::Delegate& listener )
            {
                m_Redoing.Add( listener );
            }
            void RemoveRedoingListener( const QueueChangingSignature::Delegate& listener )
            {
                m_Redoing.Remove( listener );
            }

        private:
            QueueChangeSignature::Event m_Undone;
        public:
            void AddUndoneListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Undone.Add( listener );
            }
            void RemoveUndoneListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Undone.Remove( listener );
            }

        private:
            QueueChangeSignature::Event m_Redone;
        public:
            void AddRedoneListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Redone.Add( listener );
            }
            void RemoveRedoneListener( const QueueChangeSignature::Delegate& listener )
            {
                m_Redone.Remove( listener );
            }
        };
    }
}
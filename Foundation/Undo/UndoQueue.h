#pragma once

#include "Foundation/Undo/UndoCommand.h"

namespace Helium
{
    class UndoQueue;

    struct UndoQueueChangeArgs
    {
        UndoQueueChangeArgs( UndoQueue* queue, const UndoCommand* command )
            : m_UndoQueue( queue )
            , m_UndoCommand( command )
        {

        }

        UndoQueue*          m_UndoQueue;
        const UndoCommand*  m_UndoCommand;
    };

    struct UndoQueueChangingArgs : public UndoQueueChangeArgs
    {
        UndoQueueChangingArgs( UndoQueue* queue, const UndoCommand* command )
            : UndoQueueChangeArgs( queue, command )
            , m_Veto( false )
        {

        }

        mutable bool    m_Veto;
    };

    typedef Helium::Signature< const UndoQueueChangingArgs& > UndoQueueChangingSignature;
    typedef Helium::Signature< const UndoQueueChangeArgs& > UndoQueueChangeSignature;

    class HELIUM_FOUNDATION_API UndoQueue
    {
        //
        // Members
        //

    private:
        // The undo and redo stacks
        std::vector<UndoCommandPtr> m_Undo;
        std::vector<UndoCommandPtr> m_Redo;

        // is the queue active, we don't want to modify the queue while we are commiting a change
        bool m_Active;

        // max allowed length of the queue
        int m_MaxLength;

        // the batch state
        int m_BatchState;

        // the batch
        BatchUndoCommandPtr m_Batch;


        //
        // Constructor
        //

    public:
        UndoQueue();
        ~UndoQueue();

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

        void Push(const UndoCommandPtr& c);

        bool CanUndo() const;

        bool CanRedo() const;

        void Undo();

        void Redo();


        // 
        // Events
        //

    private:
        UndoQueueChangeSignature::Event m_UndoCommandPushed;
    public:
        void AddUndoCommandPushedListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_UndoCommandPushed.Add( listener );
        }
        void RemoveUndoCommandPushedListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_UndoCommandPushed.Remove( listener );
        }

    private:
        UndoQueueChangeSignature::Event m_Reset;
    public:
        void AddResetListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Reset.Add( listener );
        }
        void RemoveResetListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Reset.Remove( listener );
        }

    private:
        UndoQueueChangeSignature::Event m_Destroyed;
    public:
        void AddDestroyListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Destroyed.Add( listener );
        }
        void RemoveDestroyListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Destroyed.Remove( listener );
        }

    private:
        UndoQueueChangingSignature::Event m_Undoing;
    public:
        void AddUndoingListener( const UndoQueueChangingSignature::Delegate& listener )
        {
            m_Undoing.Add( listener );
        }
        void RemoveUndoingListener( const UndoQueueChangingSignature::Delegate& listener )
        {
            m_Undoing.Remove( listener );
        }

    private:
        UndoQueueChangingSignature::Event m_Redoing;
    public:
        void AddRedoingListener( const UndoQueueChangingSignature::Delegate& listener )
        {
            m_Redoing.Add( listener );
        }
        void RemoveRedoingListener( const UndoQueueChangingSignature::Delegate& listener )
        {
            m_Redoing.Remove( listener );
        }

    private:
        UndoQueueChangeSignature::Event m_Undone;
    public:
        void AddUndoneListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Undone.Add( listener );
        }
        void RemoveUndoneListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Undone.Remove( listener );
        }

    private:
        UndoQueueChangeSignature::Event m_Redone;
    public:
        void AddRedoneListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Redone.Add( listener );
        }
        void RemoveRedoneListener( const UndoQueueChangeSignature::Delegate& listener )
        {
            m_Redone.Remove( listener );
        }
    };
}
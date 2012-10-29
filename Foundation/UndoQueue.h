#pragma once

#include "Foundation/API.h"
#include "Foundation/Event.h"
#include "Foundation/Property.h"
#include "Foundation/Memory/SmartPtr.h"

#include <vector>

namespace Helium
{
    //
    // This is a basic undoable command object, interface for the queue
    //

    class HELIUM_FOUNDATION_API UndoCommand : public Helium::RefCountBase<UndoCommand>
    {
    public:
        UndoCommand();
        virtual ~UndoCommand();

        virtual void Undo() = 0;
        virtual void Redo() = 0;

        //
        // A significant command is one that changes data in such a way that a save is required.
        //  By default, all commands are significant.  Override this in a derived class if you do not 
        //  want your command to cause the application to think that a save is required.  It is up
        //  to the application code to check this value and behave appropriately.
        //

        virtual bool IsSignificant() const
        {
            return true;
        }
    };

    typedef Helium::SmartPtr<UndoCommand> UndoCommandPtr;

    //
    // This is a tuple of commands for making multiple changes to multiple objects in a single command
    //

    class HELIUM_FOUNDATION_API BatchUndoCommand : public UndoCommand
    {
    protected:
        std::vector<UndoCommandPtr>   m_Commands;
        bool                    m_IsSignificant;

    public:
        BatchUndoCommand();
        BatchUndoCommand(const std::vector<UndoCommandPtr>& objects);
            
        void Set(const std::vector<UndoCommandPtr>& commands);
        void Push(const UndoCommandPtr& command);

        virtual void Undo() HELIUM_OVERRIDE;
        virtual void Redo() HELIUM_OVERRIDE;

        virtual bool IsSignificant() const HELIUM_OVERRIDE;
        virtual bool IsEmpty() const;
    };

    typedef Helium::SmartPtr<BatchUndoCommand> BatchUndoCommandPtr;

    //
    // UndoCommand template for get/set property data
    //

    template <class V>
    class PropertyUndoCommand : public UndoCommand
    {
    private:
        // the property object we will get/set through
        Helium::SmartPtr< Helium::Property<V> > m_Property;

        // the latent data value
        V m_Value;

        bool m_Significant; 

    public:
        PropertyUndoCommand(const Helium::SmartPtr< Helium::Property<V> >& property)
            : m_Property (property)
            , m_Significant( true )
        {
            m_Value = m_Property->Get();
        }

        PropertyUndoCommand(const Helium::SmartPtr< Helium::Property<V> >& property, const V& val)
            : m_Property (property)
            , m_Value (val)
            , m_Significant( true )
        {
            Swap();
        }

        void SetSignificant(bool significant)
        {
            m_Significant = significant; 
        }

        virtual bool IsSignificant() const
        {
            return m_Significant; 
        }

        virtual void Undo() HELIUM_OVERRIDE
        {
            Swap();
        }

        virtual void Redo() HELIUM_OVERRIDE
        {
            Swap();
        }

        void Swap()
        {
            // read the existing value
            V old = m_Property->Get();

            // set the stored value
            m_Property->Set(m_Value);

            // save the previous one
            m_Value = old;
        }
    };

    //
    // ExistenceUndoCommand helps store some state for add/remove with undo/redo support using delegates
    //

    namespace ExistenceActions
    {
        enum ExistenceAction
        {
            Add,
            Remove
        };
    }
    typedef ExistenceActions::ExistenceAction ExistenceAction;

    template< class V >
    class ExistenceUndoCommand : public UndoCommand
    {
    private:
        ExistenceAction     m_Action;
        Delegate< V >       m_Add;
        Delegate< V >       m_Remove;
        V                   m_Value;

    public:
        ExistenceUndoCommand( ExistenceAction action, const V& value, Delegate< V > add, Delegate< V > remove, bool redo )
            : m_Action( action )
            , m_Value( value )
            , m_Add( add )
            , m_Remove( remove )
        {
            if ( redo )
            {
                Redo();
            }
        }

        void Undo() HELIUM_OVERRIDE
        {
            switch ( m_Action )
            {
            case ExistenceActions::Add:
                m_Remove.Invoke( m_Value );
                break;

            case ExistenceActions::Remove:
                m_Add.Invoke( m_Value );
                break;
            }
        }

        void Redo() HELIUM_OVERRIDE
        {
            switch ( m_Action )
            {
            case ExistenceActions::Add:
                m_Add.Invoke( m_Value );
                break;

            case ExistenceActions::Remove:
                m_Remove.Invoke( m_Value );
                break;
            }
        }
    };

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
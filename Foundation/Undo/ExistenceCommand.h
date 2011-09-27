#pragma once

#include "Foundation/Undo/UndoCommand.h"

namespace Helium
{
    namespace ExistenceActions
    {
        enum ExistenceAction
        {
            Add,
            Remove
        };
    }
    typedef ExistenceActions::ExistenceAction ExistenceAction;

    class HELIUM_FOUNDATION_API ExistenceCommand : public UndoCommand
    {
    private:
        ExistenceAction     m_Action;
        FunctionCallerPtr   m_Add;
        FunctionCallerPtr   m_Remove;

    public:
        ExistenceCommand( ExistenceAction action, FunctionCallerPtr add, FunctionCallerPtr remove, bool redo = true );
        ~ExistenceCommand();

        virtual void Undo() HELIUM_OVERRIDE;
        virtual void Redo() HELIUM_OVERRIDE;
    };
    typedef Helium::SmartPtr< ExistenceCommand > ExistenceCommandPtr;
}
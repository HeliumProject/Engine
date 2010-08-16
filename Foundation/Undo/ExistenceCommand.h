#pragma once

#include "Foundation/API.h"
#include "Command.h"
#include "FunctionCaller.h"

namespace Helium
{
    namespace Undo
    {
        /////////////////////////////////////////////////////////////////////////////
        // Defines whether you want to Add or Remove an item.
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


        /////////////////////////////////////////////////////////////////////////////
        // Undoable command for adding or removing an item.
        //
        class FOUNDATION_API ExistenceCommand : public Command
        {
        private:
            ExistenceAction m_Action;
            FunctionCallerPtr m_Add;
            FunctionCallerPtr m_Remove;

        public:
            // Constructor
            ExistenceCommand( ExistenceAction action, FunctionCallerPtr add, FunctionCallerPtr remove, bool redo = true );
            virtual ~ExistenceCommand();
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;
        };
        typedef Helium::SmartPtr< ExistenceCommand > ExistenceCommandPtr;
    }
}
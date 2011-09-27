#pragma once

#include "Foundation/Undo/UndoCommand.h"

namespace Helium
{
    //
    // This is a tuple of commands for making multiple changes to multiple objects in a single command
    //

    class HELIUM_FOUNDATION_API BatchCommand : public UndoCommand
    {
    protected:
        V_UndoCommandSmartPtr   m_Commands;
        bool                    m_IsSignificant;

    public:
        BatchCommand();
        BatchCommand(const V_UndoCommandSmartPtr& objects);
        virtual ~BatchCommand();
            
        void Set(const V_UndoCommandSmartPtr& commands);
        void Push(const UndoCommandPtr& command);

        virtual void Undo() HELIUM_OVERRIDE;
        virtual void Redo() HELIUM_OVERRIDE;

        virtual bool IsSignificant() const HELIUM_OVERRIDE;
        virtual bool IsEmpty() const;
    };

    typedef Helium::SmartPtr<BatchCommand> BatchCommandPtr;
}
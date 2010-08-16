#pragma once

#include "Foundation/API.h"

#include "Command.h"

namespace Helium
{
    namespace Undo
    {
        //
        // This is a tuple of commands for making multiple changes to multiple objects in a single command
        //

        class FOUNDATION_API BatchCommand : public Command
        {
        protected:
            bool m_IsSignificant;
            V_CommandSmartPtr m_Commands;

        public:
            BatchCommand();
            BatchCommand(const V_CommandSmartPtr& objects);
            virtual ~BatchCommand();
            void Set(const V_CommandSmartPtr& commands);
            void Push(const CommandPtr& command);
            virtual void Undo() HELIUM_OVERRIDE;
            virtual void Redo() HELIUM_OVERRIDE;
            virtual bool IsSignificant() const HELIUM_OVERRIDE;
            virtual bool IsEmpty() const;
        };

        typedef Helium::SmartPtr<BatchCommand> BatchCommandPtr;
    }
}
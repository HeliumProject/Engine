#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectFieldInterpreter.h"

#include "Foundation/FileDialog.h"

namespace Helium
{
    namespace Inspect
    {
        class FOUNDATION_API PathInterpreter : public ReflectFieldInterpreter
        {
        public:
            PathInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent);

            FileDialogSignature::Delegate d_FindMissingFile;

        private:

            // callbacks
            void DataChanging( const DataChangingArgs& args );
            void Edit( const ButtonClickedArgs& args );

        protected:
            tstring m_FileFilter;

        private:
            Inspect::Value* m_Value;
        };

        typedef Helium::SmartPtr<PathInterpreter> PathInterpreterPtr;
        typedef std::vector< PathInterpreterPtr > V_PathInterpreter;
    }
}
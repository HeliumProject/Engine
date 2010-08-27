#pragma once

#include "Application/API.h"
#include "Application/Inspect/Interpreters/Reflect/ReflectFieldInterpreter.h"

#include "Foundation/FileDialog.h"

#ifdef INSPECT_REFACTOR
#include "Application/Inspect/DragDrop/FilteredDropTarget.h"
#endif

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API FileInterpreter : public ReflectFieldInterpreter
        {
        public:
            FileInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent);

            FileDialogSignature::Delegate d_FindMissingFile;

        private:

            // callbacks
            bool DataChanging( DataChangingArgs& args );
            void Edit( const ButtonClickedArgs& args );

#ifdef INSPECT_REFACTOR
            void OnDrop( const Inspect::FilteredDropTargetArgs& args );
#endif


        protected:
            tstring m_FileFilter;

        private:
            Inspect::Value* m_Value;
        };

        typedef Helium::SmartPtr<FileInterpreter> FileInterpreterPtr;
        typedef std::vector< FileInterpreterPtr > V_FileInterpreter;
    }
}
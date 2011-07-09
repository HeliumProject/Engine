#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Interpreters/Reflect/ReflectFieldInterpreter.h"
#include "Foundation/Reflect/Data/StlVectorData.h"
#include "Foundation/Reflect/Data/StlSetData.h"

#include "Foundation/FileDialog.h"

namespace Helium
{
    namespace Inspect
    {
        // Forwards
        class Button;
        class List;
        struct FilteredDropTargetArgs;

        class HELIUM_FOUNDATION_API PathContainerInterpreter : public ReflectFieldInterpreter
        {
        public:
            PathContainerInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent);

        private:
            // callbacks
            void OnAdd( const ButtonClickedArgs& args );
            void OnAddFile( const ButtonClickedArgs& args );
            void OnFindFile( const ButtonClickedArgs& args );
            void OnEdit( const ButtonClickedArgs& args );
            void OnRemove( const ButtonClickedArgs& args );
            void OnMoveUp( const ButtonClickedArgs& args );
            void OnMoveDown( const ButtonClickedArgs& args );

            FileDialogSignature::Delegate d_OpenFileDialog;
            
        private:
            Helium::SmartPtr< MultiStringFormatter< Reflect::Data > > m_DataPtr;
            Reflect::PathStlVectorData* m_PathVector;
            Reflect::PathStlSetData* m_PathSet;
            Inspect::List* m_List;
        };

        typedef Helium::SmartPtr< PathContainerInterpreter > PathContainerInterpreterPtr;
        typedef std::vector< PathContainerInterpreterPtr > V_PathContainerInterpreterSmartPtr;
    }
}
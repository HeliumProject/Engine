#pragma once

#include "DropTarget.h"

#include "Foundation/SmartPtr.h"

namespace Helium
{
    namespace Editor
    {
        struct FilteredDropTargetArgs
        {
            std::vector< std::string > m_Paths;

            FilteredDropTargetArgs( const std::vector< std::string >& paths )
                : m_Paths( paths )
            {
            }
        };
        typedef Helium::Signature< const FilteredDropTargetArgs&, Helium::AtomicRefCountBase > FilteredDropTargetSignature;

        class FilteredDropTarget : public DropTarget
        {
        public:
            FilteredDropTarget( const std::string& filter = TXT( "" ) );
            virtual ~FilteredDropTarget();

            bool ValidateDrag( const Editor::DragArgs& args );
            void DragOver( const Editor::DragArgs& args );
            void Drop( const Editor::DragArgs& args );  

            void AddDroppedListener( const FilteredDropTargetSignature::Delegate& d );
            void RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d );

        protected:
            std::string m_FileFilter;

        private:
            FilteredDropTargetSignature::Event m_Dropped;
        };
    }
}
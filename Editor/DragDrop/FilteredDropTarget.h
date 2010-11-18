#pragma once

#include "DropTarget.h"

#include "Foundation/Atomic.h"

namespace Helium
{
    namespace Editor
    {
        struct FilteredDropTargetArgs
        {
            std::vector< tstring > m_Paths;

            FilteredDropTargetArgs( const std::vector< tstring >& paths )
                : m_Paths( paths )
            {
            }
        };
        typedef Helium::Signature< const FilteredDropTargetArgs&, Helium::AtomicRefCountBase > FilteredDropTargetSignature;

        class FilteredDropTarget : public DropTarget
        {
        public:
            FilteredDropTarget( const tstring& filter = TXT( "" ) );
            virtual ~FilteredDropTarget();

            bool ValidateDrag( const Editor::DragArgs& args );
            void DragOver( const Editor::DragArgs& args );
            void Drop( const Editor::DragArgs& args );  

            void AddDroppedListener( const FilteredDropTargetSignature::Delegate& d );
            void RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d );

        protected:
            tstring m_FileFilter;

        private:
            FilteredDropTargetSignature::Event m_Dropped;
        };
    }
}
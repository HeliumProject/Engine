#pragma once

#include "DropTarget.h"

#include "Foundation/Atomic.h"

namespace Finder
{
    class FinderSpec;
}

namespace Inspect
{
    struct FilteredDropTargetArgs
    {
        std::vector< tstring > m_Paths;

        FilteredDropTargetArgs( const std::vector< tstring >& paths )
            : m_Paths( paths )
        {
        }
    };
    typedef Helium::Signature<void, const FilteredDropTargetArgs&, Foundation::AtomicRefCountBase> FilteredDropTargetSignature;

    class APPLICATION_API FilteredDropTarget : public DropTarget
    {
    public:
        FilteredDropTarget( const tstring& filter = TXT( "" ) );
        virtual ~FilteredDropTarget();

        bool ValidateDrag( const Inspect::DragArgs& args );
        wxDragResult DragOver( const Inspect::DragArgs& args );
        wxDragResult Drop( const Inspect::DragArgs& args );  

        void AddDroppedListener( const FilteredDropTargetSignature::Delegate& d );
        void RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d );

    protected:
        tstring m_FileFilter;

    private:
        FilteredDropTargetSignature::Event m_Dropped;
    };
}
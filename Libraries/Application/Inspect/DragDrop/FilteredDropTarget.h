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
        V_string m_Paths;

        FilteredDropTargetArgs( const V_string& paths )
            : m_Paths( paths )
        {
        }
    };
    typedef Nocturnal::Signature<void, const FilteredDropTargetArgs&, Foundation::AtomicRefCountBase> FilteredDropTargetSignature;

    class APPLICATION_API FilteredDropTarget : public DropTarget
    {
    public:
        FilteredDropTarget( const std::string& filter = "" );
        virtual ~FilteredDropTarget();

        bool ValidateDrag( const Inspect::DragArgs& args );
        wxDragResult DragOver( const Inspect::DragArgs& args );
        wxDragResult Drop( const Inspect::DragArgs& args );  

        void AddDroppedListener( const FilteredDropTargetSignature::Delegate& d );
        void RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d );

    protected:
        std::string m_FileFilter;

    private:
        FilteredDropTargetSignature::Event m_Dropped;
    };
}
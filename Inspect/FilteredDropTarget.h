#pragma once

#include "DropTarget.h"

#include "Windows/Atomic.h"

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
  typedef Nocturnal::Signature<void, const FilteredDropTargetArgs&, ::Windows::AtomicRefCountBase> FilteredDropTargetSignature;

  class INSPECT_API FilteredDropTarget : public DropTarget
  {
  public:
    FilteredDropTarget( const Finder::FinderSpec* finderSpec = NULL );
    virtual ~FilteredDropTarget();

    bool ValidateDrag( const Inspect::DragArgs& args );
    wxDragResult DragOver( const Inspect::DragArgs& args );
    wxDragResult Drop( const Inspect::DragArgs& args );  
  
    void AddDroppedListener( const FilteredDropTargetSignature::Delegate& d );
    void RemoveDroppedListener( const FilteredDropTargetSignature::Delegate& d );

  protected:
    const Finder::FinderSpec* m_FinderSpec;
 
   private:
    FilteredDropTargetSignature::Event m_Dropped;
 };
}
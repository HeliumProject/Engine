#pragma once

#include "Common/Memory/SmartPtr.h"

namespace Luna
{
  class AssetManager;

  /////////////////////////////////////////////////////////////////////////////
  // Friend class of Luna::AssetManager.  Raises hierarchy change messages in its
  // constructor and destructor.  The main consumer of this class is the tree
  // control which freezes and thaws drawing based upon the lifetime of these
  // objects.  Therefore, it is important to note that as long as you hold a 
  // pointer to one of these objects, the tree control will not draw.
  // 
  class HierarchyChangeToken : public Nocturnal::RefCountBase< HierarchyChangeToken >
  {
  private:
    Luna::AssetManager* m_AssetManager;

  public:
    HierarchyChangeToken( Luna::AssetManager* assetManager );
    ~HierarchyChangeToken();
  };
  typedef Nocturnal::SmartPtr< HierarchyChangeToken > LHierarchyChangeTokenPtr;
}

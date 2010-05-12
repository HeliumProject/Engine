#pragma once

#include "API.h"

#include "TUID/TUID.h"
#include "UIToolKit/ThreadMechanism.h"

namespace Luna
{
  class DependencyCollection;

  ///////////////////////////////////////////////////////////////////////////////
  /// class DependencyLoader
  ///////////////////////////////////////////////////////////////////////////////
  class DependencyLoader : public UIToolKit::ThreadMechanism
  {
  public:
    DependencyLoader( DependencyCollection* collection );
    virtual ~DependencyLoader();

  protected:
    virtual void InitData() NOC_OVERRIDE;
    virtual void ThreadProc( i32 threadID ) NOC_OVERRIDE;

    void OnEndThread( const UIToolKit::ThreadProcArgs& args );

  private:
    DependencyCollection* m_Collection;
    S_tuid m_AssetIDs;
  };
  
}

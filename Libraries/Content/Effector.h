#pragma once

#include "API.h"

#include "PivotTransform.h"

namespace Content
{
  class CONTENT_API Effector : public PivotTransform
  {
  public:
    
    i32 m_EffectorIndex;

    Effector () 
      : m_EffectorIndex( -1 )
    { }

    Effector (const Nocturnal::TUID& id)
      : PivotTransform (id) 
      , m_EffectorIndex( -1 ) 
    { }

    REFLECT_DECLARE_CLASS(Effector, PivotTransform);

    static void EnumerateClass( Reflect::Compositor<Effector>& comp );
  };

  typedef Nocturnal::SmartPtr<Effector> EffectorPtr;
  typedef std::vector<EffectorPtr> V_Effector;
}
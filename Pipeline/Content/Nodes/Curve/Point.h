#pragma once

#include "Pipeline/Content/Nodes/HierarchyNode.h"

namespace Content
{
  class PIPELINE_API Point : public HierarchyNode
  {
  public:
    Math::Vector3 m_Position;

    Point( const Math::Vector3& pos = Math::Vector3() )
      : m_Position( pos )
    {

    }

    Point( const Nocturnal::TUID& id, const Math::Vector3& pos = Math::Vector3() )
      : HierarchyNode( id )
      , m_Position( pos )
    {

    }

    REFLECT_DECLARE_CLASS( Point, HierarchyNode );

    static void EnumerateClass( Reflect::Compositor<Point>& comp );
  };

  typedef Nocturnal::SmartPtr<Point> PointPtr;
}
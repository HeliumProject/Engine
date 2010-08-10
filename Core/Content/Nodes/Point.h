#pragma once

#include "Core/Content/Nodes/HierarchyNode.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API Point : public HierarchyNode
        {
        public:
            Math::Vector3 m_Position;

            Point( const Math::Vector3& pos = Math::Vector3() )
                : m_Position( pos )
            {

            }

            Point( const Helium::TUID& id, const Math::Vector3& pos = Math::Vector3() )
                : HierarchyNode( id )
                , m_Position( pos )
            {

            }

            REFLECT_DECLARE_CLASS( Point, HierarchyNode );

            static void EnumerateClass( Reflect::Compositor<Point>& comp );
        };

        typedef Helium::SmartPtr<Point> PointPtr;
    }
}
#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/SceneNode.h"
#include "Foundation/Container/BitArray.h"
#include "Foundation/TUID.h"

namespace Content
{
    //
    // This is the base class of all nodes in the XML hierarchy.
    //  It encapsulates the ID (TUID/Uuid) of each HierarchyNode.
    //

    class PIPELINE_API HierarchyNode : public SceneNode
    {
    public:
        // The ID of the parent node
        Helium::TUID m_ParentID;

        // The hidden state
        bool m_Hidden;

        // The live state
        bool m_Live;

        HierarchyNode()
            : m_ParentID( Helium::TUID::Null )
            , m_Hidden( false )
            , m_Live( false )
       {

        }

        HierarchyNode( const Helium::TUID& id )
            : SceneNode( id )
            , m_ParentID( Helium::TUID::Null )
            , m_Hidden( false )
            , m_Live( false )
        {

        }

        REFLECT_DECLARE_ABSTRACT(HierarchyNode, SceneNode);

        static void EnumerateClass( Reflect::Compositor<HierarchyNode>& comp );

        virtual bool ProcessComponent(Reflect::ElementPtr element, const tstring& memberName) HELIUM_OVERRIDE;
    };

    typedef Helium::SmartPtr<HierarchyNode> HierarchyNodePtr;
    typedef std::vector<HierarchyNodePtr> V_HierarchyNode;
}
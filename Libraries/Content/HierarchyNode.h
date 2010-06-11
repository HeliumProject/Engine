#pragma once

#include "API.h"
#include "SceneNode.h"
#include "Foundation/Container/BitArray.h"
#include "Content/ContentTypes.h"
#include "UID/TUID.h"

namespace Content
{
    //
    // This is the base class of all nodes in the XML hierarchy.
    //  It encapsulates the ID (UID::TUID/Uuid) of each HierarchyNode.
    //

    class CONTENT_API HierarchyNode : public SceneNode
    {
    public:
        // The ID of the parent node
        Nocturnal::UID::TUID m_ParentID;

        // The hidden state
        bool m_Hidden;

        // The live state
        bool m_Live;

        BitArray m_ExportTypes;

        HierarchyNode()
            : m_ParentID( Nocturnal::UID::TUID::Null )
            , m_Hidden( false )
            , m_Live( false )
            , m_ExportTypes( Content::ContentTypes::NumContentTypes )
        {

        }

        HierarchyNode( const Nocturnal::UID::TUID& id )
            : SceneNode( id )
            , m_ParentID( Nocturnal::UID::TUID::Null )
            , m_Hidden( false )
            , m_Live( false )
            , m_ExportTypes( Content::ContentTypes::NumContentTypes )
        {

        }

        REFLECT_DECLARE_ABSTRACT(HierarchyNode, SceneNode);

        static void EnumerateClass( Reflect::Compositor<HierarchyNode>& comp );

        virtual bool ProcessComponent(Reflect::ElementPtr element, const std::string& memberName) NOC_OVERRIDE;
    };

    typedef Nocturnal::SmartPtr<HierarchyNode> HierarchyNodePtr;
    typedef std::vector<HierarchyNodePtr> V_HierarchyNode;
}
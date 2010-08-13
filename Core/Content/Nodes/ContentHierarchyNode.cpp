#include "ContentHierarchyNode.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_ABSTRACT(HierarchyNode);

void HierarchyNode::EnumerateClass( Reflect::Compositor<HierarchyNode>& comp )
{
    Reflect::Field* fieldParentID = comp.AddField( &HierarchyNode::m_ParentID, "m_ParentID" );
    Reflect::Field* fieldHidden = comp.AddField( &HierarchyNode::m_Hidden, "m_Hidden" );
    Reflect::Field* fieldLive = comp.AddField( &HierarchyNode::m_Live, "m_Live" );
}


bool HierarchyNode::ProcessComponent( Reflect::ElementPtr element, const tstring& memberName)
{
    if (memberName == TXT( "m_Visible" ) )
    {
        if ( Reflect::Serializer::GetValue( Reflect::AssertCast< Reflect::Serializer >(element), m_Hidden ) )
        {
            m_Hidden = !m_Hidden;
        }

        return true;
    }

    return __super::ProcessComponent(element, memberName);
}
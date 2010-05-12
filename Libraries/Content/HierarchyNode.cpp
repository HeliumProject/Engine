#include "HierarchyNode.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_ABSTRACT(HierarchyNode);

void HierarchyNode::EnumerateClass( Reflect::Compositor<HierarchyNode>& comp )
{
  Reflect::Field* fieldParentID = comp.AddField( &HierarchyNode::m_ParentID, "m_ParentID" );
  Reflect::Field* fieldHidden = comp.AddField( &HierarchyNode::m_Hidden, "m_Hidden" );
  Reflect::Field* fieldLive = comp.AddField( &HierarchyNode::m_Live, "m_Live" );
}


bool HierarchyNode::ProcessComponent(ElementPtr element, const std::string& memberName)
{
  if (memberName == "m_Visible")
  {
    if ( Serializer::GetValue( AssertCast<Serializer>(element), m_Hidden ) )
    {
      m_Hidden = !m_Hidden;
    }

    return true;
  }

  return __super::ProcessComponent(element, memberName);
}
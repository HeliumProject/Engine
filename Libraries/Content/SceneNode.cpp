#include "SceneNode.h"
#include "ContentVisitor.h"

using namespace Reflect;
using namespace Attribute;
using namespace Content;

REFLECT_DEFINE_ABSTRACT(SceneNode);

void SceneNode::EnumerateClass( Reflect::Compositor<SceneNode>& comp )
{
  Reflect::Field* fieldID = comp.AddField( &SceneNode::m_ID, "m_ID", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldDefaultName = comp.AddField( &SceneNode::m_DefaultName, "m_DefaultName", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldGivenName = comp.AddField( &SceneNode::m_GivenName, "m_GivenName", Reflect::FieldFlags::Hide );
  Reflect::Field* fieldUseGivenName = comp.AddField( &SceneNode::m_UseGivenName, "m_UseGivenName", Reflect::FieldFlags::Hide );
}


void SceneNode::Host(ContentVisitor* visitor)
{
  visitor->VisitSceneNode(this); 
}

void SceneNode::PostLoad( Reflect::V_Element& elements )
{
  // Override if your node needs to do something
}

bool SceneNode::ProcessComponent(ElementPtr element, const std::string& fieldName)
{
  if ( fieldName == "m_Name" )
  {
    Serializer::GetValue( Reflect::TryCast<Serializer>(element), m_DefaultName );
    Serializer::GetValue( Reflect::TryCast<Serializer>(element), m_GivenName );
    m_UseGivenName = true;
    return true;
  }

  return __super::ProcessComponent( element, fieldName );
}

bool SceneNode::ValidateCompatible( const AttributePtr& attr, std::string& error ) const
{
  if ( attr->GetAttributeUsage() == AttributeUsages::Class )
  {
    error = attr->GetClass()->m_UIName + " is a class attribute, so it cannot be added to an instance.";
    return false;
  }

  return __super::ValidateCompatible( attr, error );
}
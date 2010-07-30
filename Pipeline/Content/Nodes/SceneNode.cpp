#include "Pipeline/Content/Nodes/SceneNode.h"
#include "Pipeline/Content/ContentVisitor.h"

using namespace Helium;
using namespace Helium::Component;
using namespace Helium::Content;

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

bool SceneNode::ProcessComponent( Reflect::ElementPtr element, const tstring& fieldName )
{
    if ( fieldName == TXT( "m_Name" ) )
    {
        Reflect::Serializer::GetValue( Reflect::TryCast< Reflect::Serializer >( element ), m_DefaultName );
        Reflect::Serializer::GetValue( Reflect::TryCast< Reflect::Serializer >( element ), m_GivenName );
        m_UseGivenName = true;
        return true;
    }

    return __super::ProcessComponent( element, fieldName );
}

bool SceneNode::ValidateCompatible( const ComponentPtr& component, tstring& error ) const
{
    if ( component->GetComponentUsage() == ComponentUsages::Class )
    {
        error = component->GetClass()->m_UIName + TXT( " is a class component, so it cannot be added to an instance." );
        return false;
    }

    return __super::ValidateCompatible( component, error );
}
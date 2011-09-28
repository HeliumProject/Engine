#include "SceneGraphPch.h"
#include "JointTransform.h"

#include "Math/FpuEulerAngles.h"
#include "Foundation/Undo/UndoCommand.h"

#include "SceneGraph/Color.h"
#include "SceneGraph/PrimitiveAxes.h"
#include "SceneGraph/PrimitiveRings.h"
#include "SceneGraph/Scene.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_OBJECT( JointTransform );

void JointTransform::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &JointTransform::m_SegmentScaleCompensate, TXT( "m_SegmentScaleCompensate" ) );
}

void JointTransform::InitializeType()
{
    Reflect::RegisterClassType< JointTransform >( TXT( "SceneGraph::JointTransform" ) );
}

void JointTransform::CleanupType()
{
    Reflect::UnregisterClassType< JointTransform >();
}

JointTransform::JointTransform()
: m_SegmentScaleCompensate( false )
{
}

JointTransform::~JointTransform()
{
}

void JointTransform::Initialize()
{
    Base::Initialize();

    SceneGraph::PrimitiveRings* rings = static_cast< SceneGraph::PrimitiveRings* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings ) );
    m_ObjectBounds.minimum = Vector3(-rings->m_Radius, -rings->m_Radius, -rings->m_Radius);
    m_ObjectBounds.maximum = Vector3(rings->m_Radius, rings->m_Radius, rings->m_Radius);
}

void JointTransform::Render( RenderVisitor* render )
{
#ifdef VIEWPORT_REFACTOR
    RenderEntry* entry = render->Allocate(this);

    entry->m_Location = render->State().m_Matrix.Normalized();
    entry->m_Center = m_ObjectBounds.Center();

    if (IsSelected() && m_Owner->IsFocused() )
    {
        entry->m_Draw = &JointTransform::DrawSelected;
    }
    else
    {
        entry->m_Draw = &JointTransform::DrawNormal;
    }
#endif

    // don't call Base here, it will draw big ass axes
    SceneGraph::HierarchyNode::Render( render );
}

#ifdef VIEWPORT_REFACTOR

void JointTransform::DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const SceneGraph::HierarchyNode* node = Reflect::AssertCast<SceneGraph::HierarchyNode>( object );

    const JointTransform* joint = Reflect::AssertCast<JointTransform>( node );

    joint->SetMaterial( g_JointTransformMaterial );

    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

void JointTransform::DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object )
{
    const SceneGraph::HierarchyNode* node = Reflect::AssertCast<SceneGraph::HierarchyNode>( object );

    const JointTransform* joint = Reflect::AssertCast<JointTransform>( node );

    joint->SetMaterial( g_JointTransformMaterial );

    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes )->Draw( args );
    node->GetOwner()->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointRings )->Draw( args );
}

#endif

bool JointTransform::Pick( PickVisitor* pick )
{
    pick->SetCurrentObject (this, pick->State().m_Matrix.Normalized());

    return pick->PickPoint(Vector3::Zero, static_cast< SceneGraph::PrimitiveAxes* >( m_Owner->GetViewport()->GetGlobalPrimitive( GlobalPrimitives::JointAxes ) )->m_Length);
}
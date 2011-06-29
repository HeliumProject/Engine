#include "PipelinePch.h"
#include "InstanceType.h"

#include "Foundation/Container/Insert.h" 

#include "Pipeline/SceneGraph/Instance.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/PrimitivePointer.h"

using Helium::Insert; 

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::InstanceType);

void InstanceType::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::InstanceType >( TXT( "SceneGraph::InstanceType" ) );
}

void InstanceType::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::InstanceType >();
}

InstanceType::InstanceType( SceneGraph::Scene* scene, const Reflect::Class* instanceClass )
: SceneGraph::HierarchyNodeType( scene, instanceClass )
, m_Pointer( NULL )
{
    m_Pointer = new SceneGraph::PrimitivePointer();
    m_Pointer->Update();
}

InstanceType::~InstanceType()
{
    delete m_Pointer;
}

void InstanceType::Create()
{
    Base::Create();

    m_Pointer->Create();
}

void InstanceType::Delete()
{
    Base::Delete();

    m_Pointer->Delete();
}

bool InstanceType::IsTransparent()
{
    return false;
}

const D3DMATERIAL9& InstanceType::GetMaterial() const
{
    return m_Material;
}

void InstanceType::AddSet(SceneGraph::InstanceSet* set)
{
    StdInsert<M_InstanceSetSmartPtr>::Result inserted = m_Sets.insert( M_InstanceSetSmartPtr::value_type( set->GetName(), set ) );

    if ( inserted.second )
    {
        m_SetAdded.Raise( InstanceTypeChangeArgs( this, set ) ); 
    }
}

void InstanceType::RemoveSet(SceneGraph::InstanceSet* set)
{
    Helium::StrongPtr< SceneGraph::InstanceSet > keepAlive = set;

    if ( m_Sets.erase(set->GetName()) > 0 )
    {
        m_SetRemoved.Raise( InstanceTypeChangeArgs( this, set ) );
    }
}

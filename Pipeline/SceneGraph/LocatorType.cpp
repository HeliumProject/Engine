/*#include "Precompile.h"*/
#include "LocatorType.h"

#include "Pipeline/SceneGraph/Locator.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/Color.h"
#include "Pipeline/SceneGraph/PrimitiveLocator.h"
#include "Pipeline/SceneGraph/PrimitiveCube.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(LocatorType);

void LocatorType::InitializeType()
{
    Reflect::RegisterClassType< LocatorType >( TXT( "SceneGraph::LocatorType" ) );
}

void LocatorType::CleanupType()
{
    Reflect::UnregisterClassType< LocatorType >();
}

LocatorType::LocatorType( Scene* scene, int32_t instanceType )
: InstanceType( scene, instanceType )
{
    m_Locator = new PrimitiveLocator( scene->GetViewport()->GetResources() );
    m_Locator->Update();

    m_Cube = new PrimitiveCube( scene->GetViewport()->GetResources() );
    m_Cube->Update();
}

LocatorType::~LocatorType()
{
    delete m_Locator;
    delete m_Cube;
}

void LocatorType::Create()
{
    __super::Create();

    m_Locator->Create();
    m_Cube->Create();
}

void LocatorType::Delete()
{
    __super::Delete();

    m_Locator->Delete();
    m_Cube->Delete();
}

const Primitive* LocatorType::GetShape( LocatorShape shape ) const
{
    switch (shape)
    {
    case LocatorShape::Cross:
        {
            return m_Locator;
        }

    case LocatorShape::Cube:
        {
            return m_Cube;
        }
    }

    return NULL;
}
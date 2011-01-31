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

LocatorType::LocatorType( Scene* scene, const Reflect::Class* instanceClass )
: InstanceType( scene, instanceClass )
{
    m_Locator = new PrimitiveLocator;
    m_Locator->Update();

    m_Cube = new PrimitiveCube;
    m_Cube->Update();
}

LocatorType::~LocatorType()
{
    delete m_Locator;
    delete m_Cube;
}

void LocatorType::Create()
{
    Base::Create();

    m_Locator->Create();
    m_Cube->Create();
}

void LocatorType::Delete()
{
    Base::Delete();

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
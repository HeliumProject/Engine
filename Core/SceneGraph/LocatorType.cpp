/*#include "Precompile.h"*/
#include "LocatorType.h"

#include "Core/SceneGraph/Locator.h"
#include "Core/SceneGraph/Scene.h"
#include "Core/SceneGraph/Viewport.h"
#include "Core/SceneGraph/Color.h"
#include "Core/SceneGraph/PrimitiveLocator.h"
#include "Core/SceneGraph/PrimitiveCube.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(LocatorType);

void LocatorType::InitializeType()
{
    Reflect::RegisterClassType< LocatorType >( TXT( "LocatorType" ) );
}

void LocatorType::CleanupType()
{
    Reflect::UnregisterClassType< LocatorType >();
}

LocatorType::LocatorType( Scene* scene, i32 instanceType )
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
    case LocatorShapes::Cross:
        {
            return m_Locator;
        }

    case LocatorShapes::Cube:
        {
            return m_Cube;
        }
    }

    return NULL;
}
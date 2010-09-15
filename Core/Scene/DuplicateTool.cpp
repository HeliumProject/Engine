/*#include "Precompile.h"*/
#include "DuplicateTool.h"

#include "Core/Scene/Mesh.h"
#include "Core/Scene/Scene.h"
#include "Volume.h"
#include "VolumeType.h"

#include "Core/Scene/Pick.h"

using namespace Helium;
using namespace Helium::Core;

REFLECT_DEFINE_ABSTRACT(Core::DuplicateTool);

void DuplicateTool::InitializeType()
{
    Reflect::RegisterClassType< Core::DuplicateTool >( TXT( "Core::DuplicateTool" ) );
}

void DuplicateTool::CleanupType()
{
    Reflect::UnregisterClassType< Core::DuplicateTool >();
}

DuplicateTool::DuplicateTool(Core::Scene* scene, PropertiesGenerator* generator)
: Core::CreateTool (scene, generator)
, m_Source (NULL)
{
    if (!m_Scene->GetSelection().GetItems().Empty())
    {
        m_Source = Reflect::ObjectCast<Core::Transform>( *m_Scene->GetSelection().GetItems().Begin() );
    }
}

DuplicateTool::~DuplicateTool()
{

}

Core::TransformPtr DuplicateTool::CreateNode()
{
    if (m_Source)
    {
        return Reflect::ObjectCast<Core::Transform>( m_Source->Duplicate() );
    }
    else
    {
        return NULL;
    }
}
#include "SceneGraphPch.h"
#include "Primitive.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::Primitive);

void Primitive::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::Primitive >( TXT( "SceneGraph::Primitive" ) );
}

void Primitive::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::Primitive >();
}

Primitive::Primitive()
: m_IsSolid (false)
, m_IsTransparent (false)
{
    SetPopulator( PopulateSignature::Delegate( this, &Primitive::Populate ) );
}

Primitive::~Primitive()
{
    Delete();
}

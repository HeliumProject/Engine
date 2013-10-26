#include "SceneGraphPch.h"
#include "Primitive.h"

HELIUM_DEFINE_ABSTRACT( Helium::SceneGraph::Primitive );

using namespace Helium;
using namespace Helium::SceneGraph;

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

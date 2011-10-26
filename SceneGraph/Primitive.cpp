#include "SceneGraphPch.h"
#include "Primitive.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_ABSTRACT(SceneGraph::Primitive);

void Primitive::InitializeType()
{

}

void Primitive::CleanupType()
{

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

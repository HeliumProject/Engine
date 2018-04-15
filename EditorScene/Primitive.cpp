#include "Precompile.h"
#include "Primitive.h"

HELIUM_DEFINE_ABSTRACT( Helium::Editor::Primitive );

using namespace Helium;
using namespace Helium::Editor;

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

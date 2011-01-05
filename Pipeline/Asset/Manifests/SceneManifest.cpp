#include "SceneManifest.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_CLASS(SceneManifest);

void SceneManifest::AcceptCompositeVisitor( Reflect::Composite& comp )
{
    Reflect::Field* fieldAssets = comp.AddField( &SceneManifest::m_Assets, "m_Assets" );
}
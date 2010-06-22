#include "SceneManifest.h"

using namespace Reflect;
using namespace Asset;

REFLECT_DEFINE_CLASS(SceneManifest);

void SceneManifest::EnumerateClass( Reflect::Compositor<SceneManifest>& comp )
{
    Reflect::Field* fieldAssets = comp.AddField( &SceneManifest::m_Assets, "m_Assets" );
}
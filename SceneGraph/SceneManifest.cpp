#include "SceneGraphPch.h"
#include "SceneManifest.h"

using namespace Helium;
using namespace Helium::SceneGraph;

REFLECT_DEFINE_OBJECT(SceneManifest);

void SceneManifest::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &SceneManifest::m_BoundingBoxMin, TXT( "m_BoundingBoxMin" ) );
    comp.AddField( &SceneManifest::m_BoundingBoxMax, TXT( "m_BoundingBoxMax" ) );
    comp.AddField( &SceneManifest::m_Assets, TXT( "m_Assets" ) );
}
#include "SceneGraphPch.h"
#include "SceneManifest.h"

#include "Reflect/TranslatorDeduction.h"

REFLECT_DEFINE_OBJECT( Helium::SceneGraph::SceneManifest );

using namespace Helium;
using namespace Helium::SceneGraph;

void SceneManifest::PopulateStructure( Reflect::Structure& comp )
{
    comp.AddField( &SceneManifest::m_BoundingBoxMin, TXT( "m_BoundingBoxMin" ) );
    comp.AddField( &SceneManifest::m_BoundingBoxMax, TXT( "m_BoundingBoxMax" ) );
    comp.AddField( &SceneManifest::m_Assets, TXT( "m_Assets" ) );
}
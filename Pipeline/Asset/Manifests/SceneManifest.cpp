#include "PipelinePch.h"
#include "SceneManifest.h"

using namespace Helium;
using namespace Helium::Asset;

REFLECT_DEFINE_OBJECT(SceneManifest);

void SceneManifest::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &SceneManifest::m_Assets, TXT( "m_Assets" ) );
}
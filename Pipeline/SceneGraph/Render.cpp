/*#include "Precompile.h"*/
#include "Render.h"

#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/Camera.h"
#include "Pipeline/SceneGraph/SceneNode.h"

#include "Foundation/Log.h"
#include "Foundation/String/Wildcard.h"
#include "Foundation/String/Tokenize.h"

using namespace Helium;
using namespace Helium::SceneGraph;

RenderVisitor::RenderVisitor()
    : m_Args( NULL )
    , m_View( NULL )
    , m_DrawInterface( NULL )
    , m_StartTime( 0x0 )
    , m_CompareTime( 0x0 )
{

}

void RenderVisitor::Reset( DrawArgs* args, const SceneGraph::Viewport* view, Lunar::BufferedDrawer* drawInterface )
{
    m_Args = args;
    m_View = view;
    m_DrawInterface = drawInterface;

    // if you hit this then you are leaking entries in the state stack, BAD :P
    HELIUM_ASSERT( m_States.size() == 1 );
    m_States.clear();
    m_States.resize( 1 );

    m_StartTime = Helium::TimerGetClock();
}

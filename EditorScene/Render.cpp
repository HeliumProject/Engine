#include "Precompile.h"
#include "Render.h"

#include "EditorScene/Viewport.h"
#include "EditorScene/Camera.h"
#include "EditorScene/SceneNode.h"

#include "Foundation/Log.h"
#include "Foundation/Wildcard.h"
#include "Foundation/Tokenize.h"

using namespace Helium;
using namespace Helium::Editor;

RenderVisitor::RenderVisitor()
	: m_View( NULL )
	, m_DrawInterface( NULL )
{

}

void RenderVisitor::Reset( const Editor::Viewport* view, BufferedDrawer* drawInterface )
{
	m_View = view;
	m_DrawInterface = drawInterface;

	// if you hit this then you are leaking entries in the state stack, BAD :P
	HELIUM_ASSERT( m_States.size() == 1 );
	m_States.clear();
	m_States.resize( 1 );
}

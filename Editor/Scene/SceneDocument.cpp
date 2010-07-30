#include "Precompile.h"
#include "SceneDocument.h"

using namespace Helium;
using namespace Helium::Editor;

// 
// RTTI
// 
EDITOR_DEFINE_TYPE( SceneDocument );

void SceneDocument::InitializeType()
{
  Reflect::RegisterClass< SceneDocument >( TXT( "SceneDocument" ) );
}

void SceneDocument::CleanupType()
{
  Reflect::UnregisterClass< SceneDocument >();
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneDocument::SceneDocument( const tstring& file, const tstring& name )
: Document( file, name )
, m_Scene( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SceneDocument::~SceneDocument()
{
}

///////////////////////////////////////////////////////////////////////////////
// Sets the scene that this file is associated with.
// 
void SceneDocument::SetScene( Editor::Scene* scene )
{
  HELIUM_ASSERT( m_Scene == NULL ); // Only call this function once
  m_Scene = scene;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the scene that this file is associated with.
// 
Editor::Scene* SceneDocument::GetScene() const
{
  HELIUM_ASSERT( m_Scene != NULL );
  return m_Scene;
}

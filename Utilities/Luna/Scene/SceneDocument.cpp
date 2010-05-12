#include "Precompile.h"
#include "SceneDocument.h"

using namespace Luna;

// 
// RTTI
// 
LUNA_DEFINE_TYPE( SceneDocument );

void SceneDocument::InitializeType()
{
  Reflect::RegisterClass< SceneDocument >( "SceneDocument" );
}

void SceneDocument::CleanupType()
{
  Reflect::UnregisterClass< SceneDocument >();
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneDocument::SceneDocument( const std::string& file, const std::string& name )
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
void SceneDocument::SetScene( Luna::Scene* scene )
{
  NOC_ASSERT( m_Scene == NULL ); // Only call this function once
  m_Scene = scene;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the scene that this file is associated with.
// 
Luna::Scene* SceneDocument::GetScene() const
{
  NOC_ASSERT( m_Scene != NULL );
  return m_Scene;
}

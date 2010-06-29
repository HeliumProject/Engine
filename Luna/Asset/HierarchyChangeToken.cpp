#include "Precompile.h"
#include "HierarchyChangeToken.h"
#include "AssetManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor - raises "hierarchy change starting" event.
// 
HierarchyChangeToken::HierarchyChangeToken( Luna::AssetManager* assetManager )
: m_AssetManager( assetManager )
{
  m_AssetManager->m_HierarchyChangeStarting.Raise( HierarchyChangeArgs() );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor - raises "hierarchy change finished" event.
// 
HierarchyChangeToken::~HierarchyChangeToken()
{
  m_AssetManager->m_HierarchyChangeFinished.Raise( HierarchyChangeArgs() );
}

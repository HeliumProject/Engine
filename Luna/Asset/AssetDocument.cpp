#include "Precompile.h"
#include "AssetDocument.h"
#include "AssetClass.h"

using namespace Luna;

// 
// RTTI
// 
LUNA_DEFINE_TYPE( AssetDocument );

void AssetDocument::InitializeType()
{
  Reflect::RegisterClass<AssetDocument>( TXT( "AssetDocument" ) );
}

void AssetDocument::CleanupType()
{
  Reflect::UnregisterClass<AssetDocument>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetDocument::AssetDocument( Luna::AssetClass* asset )
: Document( asset->GetFilePath() )
, m_AssetClass( asset )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetDocument::~AssetDocument()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the asset class that is associated with this file.
// 
Luna::AssetClass* AssetDocument::GetAssetClass() const
{
  return m_AssetClass;
}

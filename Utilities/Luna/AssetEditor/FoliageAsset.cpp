#include "Precompile.h"
#include "FoliageAsset.h"

#include "AssetEditor.h"
#include "AssetManager.h"
#include "PersistentDataFactory.h"
#include "RemoteAsset.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::FoliageAsset );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::FoliageAsset types.
// 
void FoliageAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::FoliageAsset>( "Luna::FoliageAsset" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::FoliageAsset >(), &Luna::FoliageAsset::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::FoliageAsset types.
// 
void FoliageAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::FoliageAsset>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function.
// 
Luna::PersistentDataPtr FoliageAsset::Create( Reflect::Element* foliageClass, Luna::AssetManager* manager )
{
  return new Luna::FoliageAsset( Reflect::AssertCast< Asset::FoliageAsset >( foliageClass ), manager );
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FoliageAsset::FoliageAsset( Asset::FoliageAsset* foliageClass, Luna::AssetManager* manager )
: Luna::AssetClass( foliageClass, manager )
{
  foliageClass->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FoliageAsset::OnPackageDataChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FoliageAsset::~FoliageAsset()
{
  GetPackage()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FoliageAsset::OnPackageDataChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the package data has changed.  Handles real-time update
// with the devkit.
// 
void FoliageAsset::OnPackageDataChanged( const Reflect::ElementChangeArgs& args )
{
  RemoteAsset* remote = m_AssetManager->GetAssetEditor()->GetRemoteInterface();
  Asset::FoliageAsset* foliage = GetPackage< Asset::FoliageAsset >();
  remote->TweakFoliageLod( foliage );
}

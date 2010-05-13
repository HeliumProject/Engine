#include "Precompile.h" 
#include "LevelAsset.h" 

#include "AssetManager.h"
#include "AttributeExistenceCommand.h"
#include "PersistentDataFactory.h"

#include "Console/Console.h"
#include "Inspect/Control.h"
#include "Inspect/Container.h"

#include "Attribute/AttributeHandle.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::LevelAsset ); 

//-------------------------------------------------------------------
// static functions for registration and factory implementation
//

void LevelAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::LevelAsset>( "Luna::LevelAsset" );
  PersistentDataFactory::GetInstance()->Register(Reflect::GetType<Asset::LevelAsset>(), &Luna::LevelAsset::Create);
}

void LevelAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::LevelAsset>();  
}

Luna::PersistentDataPtr LevelAsset::Create( Reflect::Element* level, Luna::AssetManager* manager )
{
  return new Luna::LevelAsset( Reflect::AssertCast< Asset::LevelAsset >( level ), manager ); 
}

//-------------------------------------------------------------------
// class member implementation
// 

LevelAsset::LevelAsset( Asset::LevelAsset* level, Luna::AssetManager* manager ) 
: Luna::AssetClass( level, manager )
{
  // comes from Reflect::Element
  level->AddChangedListener( Reflect::ElementChangeSignature::Delegate (this, &LevelAsset::OnElementChanged)); 

  level->AddAttributeAddedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &LevelAsset::OnAttributeAdded)); 
  level->AddAttributeRemovedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &LevelAsset::OnAttributeRemoved)); 
}

LevelAsset::~LevelAsset()
{
  const Asset::LevelAssetPtr& level = GetPackage< Asset::LevelAsset >(); 
  level->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate (this, &LevelAsset::OnElementChanged)); 

  //Listen for add/remove attribute
  level->RemoveAttributeAddedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &LevelAsset::OnAttributeAdded)); 
  level->RemoveAttributeRemovedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &LevelAsset::OnAttributeRemoved)); 
}

void LevelAsset::OnElementChanged(const Reflect::ElementChangeArgs& args)
{
  Asset::LevelAsset* level = GetPackage< Asset::LevelAsset >(); 
  NOC_ASSERT(args.m_Element == level); 

  // right now we broadcast just that this level asset has changed
  // when hooked for liveupdate, the devkit has logic that avoids redundant reloads
  // 
  LevelChangedArgs notifyArgs; 
  notifyArgs.m_LevelClass = level; 

  m_LevelChanged.Raise(notifyArgs); 
}

void LevelAsset::WeatherAttributeChanged(const Reflect::ElementChangeArgs& args)
{
  LevelWeatherAttributesChangedArgs notifyArgs; 
  notifyArgs.m_Flags        = Luna::WEATHER_OP_NONE;
  notifyArgs.m_LevelClass   = GetPackage< Asset::LevelAsset >(); 

  if(args.m_Field != NULL)
  {
    if(args.m_Field->m_UIName == "Texture")
    {
      notifyArgs.m_Flags   |= Luna::WEATHER_OP_PARTICLE_TEXTURE_REBUILD;
    }

    if(args.m_Field->m_UIName == "Detail Normal Map Override")
    {
      notifyArgs.m_Flags   |= Luna::WEATHER_OP_DETAIL_NORMALMAP_REBUILD;
    }

    if(args.m_Field->m_UIName == "Enabled")
    {
      notifyArgs.m_Flags   |= Luna::WEATHER_OP_TEXTURES2D_RELOAD;
    }

  }

  m_LevelWeatherAttributesChanged.Raise(notifyArgs);
}

void LevelAsset::OnAttributeAdded(const Attribute::AttributeCollectionChanged& args)
{
}

void LevelAsset::OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args)
{
}

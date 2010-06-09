#include "Precompile.h" 
#include "SceneAsset.h" 

#include "AssetManager.h"
#include "AttributeExistenceCommand.h"
#include "PersistentDataFactory.h"

#include "Console/Console.h"
#include "Inspect/Control.h"
#include "Inspect/Container.h"

#include "Attribute/AttributeHandle.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::SceneAsset ); 

//-------------------------------------------------------------------
// static functions for registration and factory implementation
//

void SceneAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::SceneAsset>( "Luna::SceneAsset" );
  PersistentDataFactory::GetInstance()->Register(Reflect::GetType<Asset::SceneAsset>(), &Luna::SceneAsset::Create);
}

void SceneAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::SceneAsset>();  
}

Luna::PersistentDataPtr SceneAsset::Create( Reflect::Element* level, Luna::AssetManager* manager )
{
  return new Luna::SceneAsset( Reflect::AssertCast< Asset::SceneAsset >( level ), manager ); 
}

//-------------------------------------------------------------------
// class member implementation
// 

SceneAsset::SceneAsset( Asset::SceneAsset* level, Luna::AssetManager* manager ) 
: Luna::AssetClass( level, manager )
{
  // comes from Reflect::Element
  level->AddChangedListener( Reflect::ElementChangeSignature::Delegate (this, &SceneAsset::OnElementChanged)); 

  level->AddAttributeAddedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &SceneAsset::OnAttributeAdded)); 
  level->AddAttributeRemovedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &SceneAsset::OnAttributeRemoved)); 
}

SceneAsset::~SceneAsset()
{
  const Asset::SceneAssetPtr& level = GetPackage< Asset::SceneAsset >(); 
  level->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate (this, &SceneAsset::OnElementChanged)); 

  //Listen for add/remove attribute
  level->RemoveAttributeAddedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &SceneAsset::OnAttributeAdded)); 
  level->RemoveAttributeRemovedListener( Attribute::AttributeCollectionChangedSignature::Delegate(this, &SceneAsset::OnAttributeRemoved)); 
}

void SceneAsset::OnElementChanged(const Reflect::ElementChangeArgs& args)
{
  Asset::SceneAsset* level = GetPackage< Asset::SceneAsset >(); 
  NOC_ASSERT(args.m_Element == level); 

  // right now we broadcast just that this level asset has changed
  // when hooked for liveupdate, the devkit has logic that avoids redundant reloads
  // 
  LevelChangedArgs notifyArgs; 
  notifyArgs.m_LevelClass = level; 

  m_LevelChanged.Raise(notifyArgs); 
}

void SceneAsset::WeatherAttributeChanged(const Reflect::ElementChangeArgs& args)
{
  LevelWeatherAttributesChangedArgs notifyArgs; 
  notifyArgs.m_Flags        = Luna::WEATHER_OP_NONE;
  notifyArgs.m_LevelClass   = GetPackage< Asset::SceneAsset >(); 

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

void SceneAsset::OnAttributeAdded(const Attribute::AttributeCollectionChanged& args)
{
}

void SceneAsset::OnAttributeRemoved(const Attribute::AttributeCollectionChanged& args)
{
}

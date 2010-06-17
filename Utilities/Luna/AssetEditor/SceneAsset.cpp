#include "Precompile.h" 
#include "SceneAsset.h" 

#include "AssetManager.h"
#include "ComponentExistenceCommand.h"
#include "PersistentDataFactory.h"

#include "Foundation/Log.h"
#include "Application/Inspect/Widgets/Control.h"
#include "Application/Inspect/Widgets/Container.h"

#include "Pipeline/Component/ComponentHandle.h"

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

  level->AddComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate(this, &SceneAsset::OnComponentAdded)); 
  level->AddComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate(this, &SceneAsset::OnComponentRemoved)); 
}

SceneAsset::~SceneAsset()
{
  const Asset::SceneAssetPtr& level = GetPackage< Asset::SceneAsset >(); 
  level->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate (this, &SceneAsset::OnElementChanged)); 

  //Listen for add/remove attribute
  level->RemoveComponentAddedListener( Component::ComponentCollectionChangedSignature::Delegate(this, &SceneAsset::OnComponentAdded)); 
  level->RemoveComponentRemovedListener( Component::ComponentCollectionChangedSignature::Delegate(this, &SceneAsset::OnComponentRemoved)); 
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

void SceneAsset::WeatherComponentChanged(const Reflect::ElementChangeArgs& args)
{
  LevelWeatherComponentsChangedArgs notifyArgs; 
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

  m_LevelWeatherComponentsChanged.Raise(notifyArgs);
}

void SceneAsset::OnComponentAdded(const Component::ComponentCollectionChanged& args)
{
}

void SceneAsset::OnComponentRemoved(const Component::ComponentCollectionChanged& args)
{
}

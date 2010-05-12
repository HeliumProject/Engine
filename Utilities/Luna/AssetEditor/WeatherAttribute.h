#pragma once
#include "AttributeWrapper.h" 
#include "AssetNode.h"
#include "Asset/WeatherAttribute.h"

namespace Luna
{
  class WeatherAttribute : public Luna::AttributeWrapper
  {
  public: 
    LUNA_DECLARE_TYPE( Luna::WeatherAttribute, Luna::AttributeWrapper ); 

    static void InitializeType(); 
    static void CleanupType(); 

    static Luna::PersistentDataPtr Create(Reflect::Element* attribute, Luna::AssetManager* manager); 

  protected: 
    WeatherAttribute(Asset::WeatherAttribute* attribute, Luna::AssetManager* manager); 
  public: 
    virtual ~WeatherAttribute(); 
    virtual void PopulateContextMenu( ContextMenuItemSet& menu ) NOC_OVERRIDE;

  private: 
    void OnGenerateScript( const ContextMenuArgsPtr& args );

  }; 
}

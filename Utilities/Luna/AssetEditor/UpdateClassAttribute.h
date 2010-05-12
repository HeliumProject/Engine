#pragma once
#include "AttributeWrapper.h" 
#include "AssetNode.h"
#include "Asset/UpdateClassAttribute.h"

namespace Luna
{
  class UpdateClassAttribute : public Luna::AttributeWrapper
  {
  public: 
    LUNA_DECLARE_TYPE( Luna::UpdateClassAttribute, Luna::AttributeWrapper ); 

    static void InitializeType(); 
    static void CleanupType(); 

    static Luna::PersistentDataPtr Create(Reflect::Element* attribute, Luna::AssetManager* manager); 

  protected: 
    UpdateClassAttribute(Asset::UpdateClassAttribute* attribute, Luna::AssetManager* manager); 
  public: 
    virtual ~UpdateClassAttribute(); 
    virtual void CreateChildren( Luna::AssetNode* parentNode );
    virtual void ConnectProperties( EnumerateElementArgs& args ); 
    virtual void PopulateContextMenu( ContextMenuItemSet& menu ) NOC_OVERRIDE; 

  private: 
    static void OnAddClass(const ContextMenuArgsPtr& args); 
    void OnRemoveClass(const ContextMenuArgsPtr& args); 

    // this is not really my parent node. it is "my node" 
    // 
    Luna::AssetNode* m_TreeNode; 
  }; 
}

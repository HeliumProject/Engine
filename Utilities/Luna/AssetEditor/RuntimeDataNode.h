#pragma once

#include "PersistentNode.h"
#include "Core/PropertiesManager.h"
#include "Content/RuntimeData.h"

namespace Luna
{
  class RuntimeDataNode : public Luna::AssetNode
  {
  public: 
    LUNA_DECLARE_TYPE( Luna::RuntimeDataNode, Luna::AssetNode ); 
    static void InitializeType(); 
    static void CleanupType(); 

    RuntimeDataNode(const Content::RuntimeDataPtr& runtimeData, Luna::AssetManager* assetManager); 
    virtual ~RuntimeDataNode(); 

    virtual void ConnectProperties( EnumerateElementArgs& args ); 

    Content::RuntimeDataPtr m_RuntimeData; 

  }; 

}

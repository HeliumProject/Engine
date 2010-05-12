#include "Precompile.h"
#include "RuntimeDataNode.h"

using namespace Luna; 

LUNA_DEFINE_TYPE( Luna::RuntimeDataNode ); 

void RuntimeDataNode::InitializeType()
{
  Reflect::RegisterClass<Luna::RuntimeDataNode>( "Luna::RuntimeDataNode" );
}

void RuntimeDataNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::RuntimeDataNode>();
}

RuntimeDataNode::RuntimeDataNode(const Content::RuntimeDataPtr& runtimeData, Luna::AssetManager* assetManager)
  : Luna::AssetNode(assetManager)
  , m_RuntimeData(runtimeData)
{
  SetIcon("header_16.png"); 
}

RuntimeDataNode::~RuntimeDataNode()
{

}

void RuntimeDataNode::ConnectProperties( EnumerateElementArgs& args )
{
  // we shouldn't usually have a null runtime instance
  // however, in the case where the symbol is gone and we're converting from long ago data
  // we might be. 
  // 
  if(m_RuntimeData->GetRuntimeInstance())
  {
    args.EnumerateElement(m_RuntimeData->GetRuntimeInstance() ); 
  }
}


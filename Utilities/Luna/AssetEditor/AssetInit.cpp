#include "Precompile.h"
#include "AssetInit.h"
#include "ArrayItemNode.h"
#include "ArrayNode.h"
#include "AssetEditor.h"
#include "AssetDocument.h"
#include "AssetClass.h"
#include "AssetPreferences.h"
#include "AssetReferenceNode.h"
#include "AssetNode.h"
#include "AttributeWrapper.h"
#include "AttributeContainer.h"
#include "AttributeNode.h"
#include "Application/Inspect/DragDrop/ClipboardDataWrapper.h"
#include "Application/Inspect/DragDrop/ClipboardElementArray.h"
#include "Application/Inspect/DragDrop/ClipboardFileList.h"
#include "ContainerItemNode.h"
#include "ContainerNode.h"
#include "ElementArrayNode.h"
#include "FieldFileReference.h"
#include "FieldNode.h"
#include "FileArrayNode.h"
#include "FileBackedAttribute.h"
#include "PersistentData.h"
#include "PersistentNode.h" 
#include "Application/Inspect/DragDrop/ReflectClipboardData.h"
#include "ShaderAsset.h"
#include "SimpleFieldNode.h"
#include "SceneAsset.h"
#include "TextureMapAttribute.h"

#include "Foundation/InitializerStack.h"
#include "Asset/AssetInit.h"
#include "Asset/AssetVersion.h"
#include "Pipeline/Content/ContentInit.h"
#include "Foundation/Log.h"
#include "Reflect/Registry.h"
#include "Application/UI/ImageManager.h"

// Using
using namespace Luna;


namespace Luna
{
  namespace LunaAsset
  {
    i32 g_InitCount = 0;
    Nocturnal::InitializerStack g_InitializerStack;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Prepares this module for first use.  Only the first call to this function
// does something.  All subsequent calls just increment an internal counter and
// must be matched by a call to CleanupModule.
// 
void LunaAsset::InitializeModule()
{
  if ( ++g_InitCount == 1 )
  {
    g_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );
    g_InitializerStack.Push( Asset::Initialize, Asset::Cleanup );
    g_InitializerStack.Push( Content::Initialize, Content::Cleanup );
    g_InitializerStack.Push( AssetPreferences::InitializeType, AssetPreferences::CleanupType );
    g_InitializerStack.Push( Luna::AssetManagerClientData::InitializeType, Luna::AssetManagerClientData::CleanupType );
    g_InitializerStack.Push( Luna::AssetNode::InitializeType, Luna::AssetNode::CleanupType );
    g_InitializerStack.Push( Luna::PersistentNode::InitializeType, Luna::PersistentNode::CleanupType );
    g_InitializerStack.Push( Luna::FieldNode::InitializeType, Luna::FieldNode::CleanupType );
    g_InitializerStack.Push( Luna::FieldFileReference::InitializeType, Luna::FieldFileReference::CleanupType );
    g_InitializerStack.Push( Luna::ContainerItemNode::InitializeType, Luna::ContainerItemNode::CleanupType );
    g_InitializerStack.Push( Luna::ContainerNode::InitializeType, Luna::ContainerNode::CleanupType );
    g_InitializerStack.Push( Luna::ArrayItemNode::InitializeType, Luna::ArrayItemNode::CleanupType );
    g_InitializerStack.Push( Luna::ArrayNode::InitializeType, Luna::ArrayNode::CleanupType );
    g_InitializerStack.Push( Luna::ElementArrayNode::InitializeType, Luna::ElementArrayNode::CleanupType );
    g_InitializerStack.Push( Luna::FileArrayNode::InitializeType, Luna::FileArrayNode::CleanupType );
    g_InitializerStack.Push( Luna::SimpleFieldNode::InitializeType, Luna::SimpleFieldNode::CleanupType );
    g_InitializerStack.Push( Luna::PersistentData::InitializeType, Luna::PersistentData::CleanupType );
    g_InitializerStack.Push( Luna::AssetClass::InitializeType, Luna::AssetClass::CleanupType );
    g_InitializerStack.Push( Luna::AssetReferenceNode::InitializeType, Luna::AssetReferenceNode::CleanupType );
    g_InitializerStack.Push( Luna::SceneAsset::InitializeType, Luna::SceneAsset::CleanupType ); 
    g_InitializerStack.Push( Luna::ShaderAsset::InitializeType, Luna::ShaderAsset::CleanupType );
    g_InitializerStack.Push( Luna::AttributeWrapper::InitializeType, Luna::AttributeWrapper::CleanupType );
    g_InitializerStack.Push( Luna::FileBackedAttribute::InitializeType, Luna::FileBackedAttribute::CleanupType );
    g_InitializerStack.Push( Luna::TextureMapAttribute::InitializeType, Luna::TextureMapAttribute::CleanupType );
    g_InitializerStack.Push( Luna::AttributeContainer::InitializeType, Luna::AttributeContainer::CleanupType );
    g_InitializerStack.Push( Luna::AttributeNode::InitializeType, Luna::AttributeNode::CleanupType );
    g_InitializerStack.Push( AssetDocument::InitializeType, AssetDocument::CleanupType );

    Nocturnal::ImageManagerInit( "", "" );

    Nocturnal::GlobalImageManager().LoadGuiArt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Decrements an internal counter.  Once the counter reaches zero, the module
// is unloaded.
// 
void LunaAsset::CleanupModule()
{
  if ( --g_InitCount == 0 )
  {
    g_InitializerStack.Cleanup();

    Nocturnal::ImageManagerCleanup();
  }
}

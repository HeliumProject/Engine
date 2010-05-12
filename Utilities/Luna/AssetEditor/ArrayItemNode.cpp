#include "Precompile.h"
#include "ArrayItemNode.h"
#include "ArrayNode.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::ArrayItemNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ArrayItemNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ArrayItemNode>( "Luna::ArrayItemNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ArrayItemNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::ArrayItemNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ArrayItemNode::ArrayItemNode( Luna::AssetManager* assetManager, Luna::ArrayNode* container, const std::string& name )
: Luna::ContainerItemNode( assetManager, container, name )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ArrayItemNode::~ArrayItemNode()
{
}

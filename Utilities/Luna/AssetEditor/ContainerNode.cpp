#include "Precompile.h"
#include "ContainerNode.h"

#include "ArrayNode.h"
#include "ContainerItemNode.h"
#include "ElementArrayNode.h"

using namespace Luna;


// Definition
LUNA_DEFINE_TYPE( Luna::ContainerNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ContainerNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ContainerNode>( "Luna::ContainerNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ContainerNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::ContainerNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ContainerNode::ContainerNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::FieldNode( assetManager, element, field )
{
  SetIcon( "folder_blue_16.png" );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ContainerNode::~ContainerNode()
{
}

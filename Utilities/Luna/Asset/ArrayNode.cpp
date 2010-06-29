#include "Precompile.h"
#include "ArrayNode.h"
#include "ArrayItemNode.h"

#include "Foundation/String/Tokenize.h"
#include "Foundation/Log.h"
#include "Foundation/TUID.h"

using namespace Luna;


// Definition
LUNA_DEFINE_TYPE( Luna::ArrayNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ArrayNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ArrayNode>( TXT( "Luna::ArrayNode" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void ArrayNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::ArrayNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ArrayNode::ArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::ContainerNode( assetManager, element, field )
{
  //if ( HasFlag( Reflect::FieldFlags::FileID ) )
  //{
  //  m_Serializer->SetTranslateInputListener( Reflect::TranslateInputSignature::Delegate ( this, &ArrayNode::TranslateInputTUIDArray ) );
  //  m_Serializer->SetTranslateOutputListener( Reflect::TranslateOutputSignature::Delegate ( this, &ArrayNode::TranslateOutputTUIDArray ) );
  //}
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ArrayNode::~ArrayNode()
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates all the children of this node.
// 
void ArrayNode::CreateChildren()
{
  tstring value = GetValue();
  std::vector< tstring > tokens;
  ::Tokenize( value, tokens, Reflect::s_ContainerItemDelimiter );
  for each ( const tstring& str in tokens )
  {
    AddChild( new Luna::ArrayItemNode( GetAssetManager(), this, str ) );
  }
}


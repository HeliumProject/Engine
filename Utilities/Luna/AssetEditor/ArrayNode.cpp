#include "Precompile.h"
#include "ArrayNode.h"
#include "ArrayItemNode.h"

#include "Common/String/Tokenize.h"
#include "Console/Console.h"
#include "UID/TUID.h"

using namespace Luna;


// Definition
LUNA_DEFINE_TYPE( Luna::ArrayNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void ArrayNode::InitializeType()
{
  Reflect::RegisterClass<Luna::ArrayNode>( "Luna::ArrayNode" );
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
  std::string value = GetValue();
  V_string tokens;
  ::Tokenize( value, tokens, Reflect::s_ContainerItemDelimiter );
  for each ( const std::string& str in tokens )
  {
    AddChild( new Luna::ArrayItemNode( GetAssetManager(), this, str ) );
  }
}


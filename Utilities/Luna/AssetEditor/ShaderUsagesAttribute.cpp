#include "Precompile.h"
#include "ShaderUsagesAttribute.h"

#include "AssetNode.h"
#include "PersistentDataFactory.h"
#include "PersistentNode.h"

#include "UIToolKit/ImageManager.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::ShaderUsagesAttribute );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::ShaderUsagesAttribute types.
// 
void ShaderUsagesAttribute::InitializeType()
{
  Reflect::RegisterClass<Luna::ShaderUsagesAttribute>( "Luna::ShaderUsagesAttribute" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::ShaderUsagesAttribute >(), &ShaderUsagesAttribute::CreateShaderUsagesAttribute );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::ShaderUsagesAttribute types.
// 
void ShaderUsagesAttribute::CleanupType()
{
  Reflect::UnregisterClass<Luna::ShaderUsagesAttribute>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr ShaderUsagesAttribute::CreateShaderUsagesAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new Luna::ShaderUsagesAttribute( Reflect::AssertCast< Asset::ShaderUsagesAttribute >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
ShaderUsagesAttribute::ShaderUsagesAttribute( Asset::ShaderUsagesAttribute* attribute, Luna::AssetManager* assetManager )
: Luna::AttributeWrapper( attribute, assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
ShaderUsagesAttribute::~ShaderUsagesAttribute()
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates all the child nodes associated with this attribute, underneath the
// specified parent.
// 
void ShaderUsagesAttribute::CreateChildren( Luna::AssetNode* parentNode )
{
  Asset::ShaderUsagesAttribute* pkg = GetPackage< Asset::ShaderUsagesAttribute >();
  Asset::V_ShaderUsage::const_iterator itr = pkg->m_ShaderUsages.begin();
  Asset::V_ShaderUsage::const_iterator end = pkg->m_ShaderUsages.end();
  for ( ; itr != end; ++itr )
  {
    const Asset::ShaderUsagePtr& shaderUsage = *itr;
    Luna::PersistentNodePtr node = new Luna::PersistentNode( shaderUsage, GetAssetManager() );
    node->SetName( shaderUsage->GetTitle() );
    node->CreateChildren();
    parentNode->AddChild( node );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to check for any changes that might invalidate children in the
// hierarchy.
// 
void ShaderUsagesAttribute::PackageChanged( const Reflect::ElementChangeArgs& args )
{
  __super::PackageChanged( args );

  const Asset::ShaderUsagesAttribute* pkg = GetPackage< Asset::ShaderUsagesAttribute >();
  if ( pkg == args.m_Element )
  {
    const Reflect::Field* field = pkg->GetClass()->FindField( &Asset::ShaderUsagesAttribute::m_ShaderUsages );
    if ( args.m_Field == field )
    {
      // The shader usages have changed so we need to recreate the children of this node.
      m_ChildrenRefresh.Raise( AttributeChangeArgs( this ) );
    }
  }
}

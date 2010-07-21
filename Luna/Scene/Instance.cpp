#include "Precompile.h"
#include "Instance.h"
#include "InstanceType.h"
#include "InstancePanel.h"

#include "Scene.h"

#include "Application/UI/ArtProvider.h"

#include "Foundation/Log.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::Instance);

void Instance::InitializeType()
{
  Reflect::RegisterClass< Luna::Instance >( TXT( "Luna::Instance" ) );
  PropertiesGenerator::InitializePanel( TXT( "Instance" ), CreatePanelSignature::Delegate( &Instance::CreatePanel ) );
}

void Instance::CleanupType()
{
  Reflect::UnregisterClass< Luna::Instance >();
}

Instance::Instance(Luna::Scene* scene, Content::Instance* instance)
: Luna::PivotTransform ( scene, instance )
{
}

Instance::~Instance()
{
}

void Instance::Pack()
{
  __super::Pack();
}

void Instance::Unpack()
{
  __super::Unpack();
}

i32 Instance::GetImageIndex() const
{
  i32 image = Nocturnal::GlobalFileIconsTable().GetIconID( TXT( "null" ) );

  if ( GetNodeType() )
  {
    image = m_NodeType->GetImageIndex();
  }

  return image;
}

SceneNodeTypePtr Instance::CreateNodeType( Luna::Scene* scene ) const
{
  // Overridden to create an light-specific type
  Luna::InstanceType* nodeType = new Luna::InstanceType( scene, GetType() );

  // Set the image index (usually this is handled by the base class, but we aren't calling the base)
  nodeType->SetImageIndex( GetImageIndex() );

  return nodeType;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Instance.
// 
bool Instance::ValidatePanel(const tstring& name)
{
  if ( name == TXT( "Instance" ) )
    return true;

  return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit Luna::Instance.
// 
void Instance::CreatePanel( CreatePanelArgs& args )
{
  InstancePanel* panel = new InstancePanel ( args.m_Generator, args.m_Selection );

  args.m_Generator->Push( panel );
  {
    panel->SetCanvas( args.m_Generator->GetContainer()->GetCanvas() );
    panel->Create();
  }
  args.m_Generator->Pop();
}

bool Instance::GetSolid() const
{
  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

  return GetPackage< Content::Instance >()->m_SolidOverride;
}
void Instance::SetSolid( bool b )
{
  GetPackage< Content::Instance >()->m_Solid = b;
}

bool Instance::GetSolidOverride() const
{
  return GetPackage< Content::Instance >()->m_SolidOverride;
}
void Instance::SetSolidOverride( bool b )
{
  GetPackage< Content::Instance >()->m_SolidOverride = b;
}

bool Instance::GetTransparent() const
{
  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

  return GetPackage< Content::Instance >()->m_TransparentOverride;
}
void Instance::SetTransparent( bool b )
{
  GetPackage< Content::Instance >()->m_Transparent = b;
}

bool Instance::GetTransparentOverride() const
{
  return GetPackage< Content::Instance >()->m_TransparentOverride;
}
void Instance::SetTransparentOverride( bool b )
{
  GetPackage< Content::Instance >()->m_TransparentOverride = b;
}


#include "Precompile.h"
#include "Instance.h"
#include "InstanceType.h"
#include "InstancePanel.h"

#include "Scene.h"

#include "UIToolKit/ImageManager.h"

#include "Console/Console.h"
#include "Finder/ContentSpecs.h"
#include "File/Manager.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::Instance);

void Instance::InitializeType()
{
  Reflect::RegisterClass< Luna::Instance >( "Luna::Instance" );
  Enumerator::InitializePanel( "Instance", CreatePanelSignature::Delegate( &Instance::CreatePanel ) );
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
  i32 image = UIToolKit::GlobalImageManager().GetImageIndex( "null_16.png" );

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

Luna::SceneNodeType* Instance::DeduceNodeType()
{
  // result
  SceneNodeTypePtr type;

  // best match config
  TypeConfigPtr config;

  // check for a pre-existing configured type
  const std::string& configured = GetPackage<Content::Instance>()->m_ConfiguredType;

  // iterate and score configs
  V_TypeConfigSmartPtr::const_iterator itr = m_Scene->GetTypeConfigs().begin();
  V_TypeConfigSmartPtr::const_iterator end = m_Scene->GetTypeConfigs().end();
  for ( int score = 0; itr != end; ++itr )
  {
    int s = (*itr)->Validate(this);

    // if we found our configured type, just use it
    if (s > 0 && !configured.empty() && (*itr)->m_Name == configured)
    {
      config = *itr;
      score = s;
      break;
    }

    if (s > score)
    {
      config = *itr;
      score = s;
    }
  }

  if (config)
  {
    HM_StrToSceneNodeTypeSmartPtr::const_iterator found = m_Scene->GetNodeTypesByName().find( config->m_Name );

    if (found == m_Scene->GetNodeTypesByName().end())
    {
      type = CreateNodeType( m_Scene );

      Luna::InstanceType* instanceType = Reflect::AssertCast<Luna::InstanceType>(type);

      instanceType->SetConfiguration( config );

      m_Scene->AddNodeType( instanceType );
    }
    else
    {
      type = found->second;
    }
  }

  if (type.ReferencesObject())
  {
    // we earned fancy points
    return type;
  }
  else
  {
    // no fancy points accumulated this round
    return __super::DeduceNodeType();
  }
}

void Instance::CheckNodeType()
{
  __super::CheckNodeType();
}

S_string Instance::GetValidConfiguredTypeNames()
{
  S_string names;

  // iterate and score configs
  V_TypeConfigSmartPtr::const_iterator itr = m_Scene->GetTypeConfigs().begin();
  V_TypeConfigSmartPtr::const_iterator end = m_Scene->GetTypeConfigs().end();
  for ( int score = 0; itr != end; ++itr )
  {
    int s = (*itr)->Validate(this);
    if (s > 0)
    {
      names.insert( (*itr)->m_Name );
    }
  }

  return names;
}

std::string Instance::GetConfiguredTypeName() const
{
  return GetPackage<Content::Instance>()->m_ConfiguredType;
}

void Instance::SetConfiguredTypeName( const std::string& type )
{
  std::string&                  oldType   = GetPackage<Content::Instance>()->m_ConfiguredType;
  InstancePropertiesChangeArgs  args(this, oldType, type);

  oldType = type;
  CheckNodeType();
  m_Changed.Raise( args );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified panel is supported by Instance.
// 
bool Instance::ValidatePanel(const std::string& name)
{
  if ( name == "Instance" )
    return true;

  return __super::ValidatePanel( name );
}

///////////////////////////////////////////////////////////////////////////////
// Static function for creating the UI panel that allows users to edit Luna::Instance.
// 
void Instance::CreatePanel( CreatePanelArgs& args )
{
  InstancePanel* panel = new InstancePanel ( args.m_Enumerator, args.m_Selection );

  args.m_Enumerator->Push( panel );
  {
    panel->SetCanvas( args.m_Enumerator->GetContainer()->GetCanvas() );
    panel->Create();
  }
  args.m_Enumerator->Pop();
}

bool Instance::GetSolid() const
{
  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

  return (GetPackage< Content::Instance >()->m_SolidOverride || !type->GetConfiguration().ReferencesObject()) ? GetPackage< Content::Instance >()->m_Solid : type->GetConfiguration()->m_Solid;
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

  return (GetPackage< Content::Instance >()->m_TransparentOverride || !type->GetConfiguration().ReferencesObject()) ? GetPackage< Content::Instance >()->m_Transparent : type->GetConfiguration()->m_Transparent;
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


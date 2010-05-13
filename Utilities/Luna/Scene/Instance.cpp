#include "Precompile.h"
#include "Instance.h"
#include "InstanceType.h"
#include "InstancePanel.h"
#include "InstanceCodeSet.h"

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
, m_CodeSet (NULL)
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
  CheckSets();

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

  CheckSets();
}

std::string Instance::GenerateName() const
{
  std::string name = GetRuntimeClassName();

  if ( name.empty() )
  {
    name = __super::GenerateName();
  }
  else
  {
    name += "1";
  }

  return name;
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

Luna::InstanceCodeSet* Instance::GetCodeSet()
{
  return m_CodeSet;
}

const Luna::InstanceCodeSet* Instance::GetCodeSet() const
{
  return m_CodeSet;
}

void Instance::SetCodeSet(Luna::InstanceCodeSet* codeClass)
{
  m_CodeSet = codeClass;
}

void Instance::CheckSets()
{
  Content::InstancePtr instance = GetPackage< Content::Instance >();

  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>( m_NodeType );
  if (type)
  {
    // find the set
    M_InstanceSetSmartPtr::const_iterator found = type->GetSets().find(currentCodeSet);
    
    // if we found it, and it contains us, and we are using it
    if (found != type->GetSets().end() && found->second->ContainsInstance(this) && m_CodeSet == found->second.Ptr())
    {
      // we are GTG
      return;
    }

    // the set we are entering
    Luna::InstanceCodeSet* newCodeSet = NULL;

    // create new class object if it does not already exist
    if (found == type->GetSets().end())
    {
      // create
      newCodeSet = new Luna::InstanceCodeSet (type, currentCodeSet);

      // save
      type->AddSet( newCodeSet );
    }
    else
    {
      // existing
      newCodeSet = Reflect::AssertCast<Luna::InstanceCodeSet>( found->second );
    }

    // check previous membership
    if (m_CodeSet)
    {
      m_CodeSet->RemoveInstance(this);
    }

    // add to the new class collection
    newCodeSet->AddInstance(this);
  }
}

void Instance::ReleaseSets()
{
  m_CodeSet->RemoveInstance( this );
}

void Instance::FindSimilar(V_HierarchyNodeDumbPtr& similar) const
{
  if (m_CodeSet)
  {
    S_InstanceDumbPtr::const_iterator itr = m_CodeSet->GetInstances().begin();
    S_InstanceDumbPtr::const_iterator end = m_CodeSet->GetInstances().end();
    for ( ; itr != end; ++itr )
    {
      similar.push_back( *itr );
    }
  }
}

bool Instance::IsSimilar(const HierarchyNodePtr& node) const
{
  const Luna::Instance* instance = Reflect::ObjectCast<Luna::Instance>( node );
  if ( instance && ( instance->m_CodeSet == m_CodeSet ) )
  {
    return true;
  }

  return false;
}

std::string Instance::GetDescription() const
{
  if (m_CodeSet)
  {
    return m_CodeSet->GetName();
  }
  else
  {
    return __super::GetDescription();
  }
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

const bool Instance::GetIsWeatherBlocker() const
{
  return GetPackage< Content::Instance >()->GetIsWeatherBlocker();
}

float Instance::GetBorderSize() const
{
  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

  return GetPackage< Content::Instance >()->m_BorderSize;
}

void Instance::SetBorderSize( float s )
{
  GetPackage< Content::Instance >()->m_BorderSize = s;
  std::string&                  type   = GetPackage<Content::Instance>()->m_ConfiguredType;
  InstancePropertiesChangeArgs  args(this, type, type);
  m_Changed.Raise( args );
}

bool Instance::GetSkipParticles() const
{
  Luna::InstanceType* type = Reflect::AssertCast<Luna::InstanceType>(m_NodeType);

  return GetPackage< Content::Instance >()->m_SkipParticles;
}

void Instance::SetSkipParticles( bool s )
{
  GetPackage< Content::Instance >()->m_SkipParticles = s;
  std::string&                  type   = GetPackage<Content::Instance>()->m_ConfiguredType;
  InstancePropertiesChangeArgs  args(this, type, type);
  m_Changed.Raise( args );
}

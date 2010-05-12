#include "Precompile.h"

#include "AnimationConfigAsset.h"
#include "PersistentDataFactory.h"
#include "AssetDocument.h"
#include "AssetEditor.h"
#include "AssetNode.h"

#include "AnimToolLib/AnimConfig.h"
#include "FileSystem/FileSystem.h"
#include "Editor/SessionManager.h"
#include "Core/Enumerator.h"
#include "InspectReflect/InspectReflectInit.h"
#include "Windows/Process.h"

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AnimationConfigAsset );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AnimationConfigAsset types.
// 
void AnimationConfigAsset::InitializeType()
{
  Reflect::RegisterClass<Luna::AnimationConfigAsset>( "Luna::AnimationConfigAsset" );
  Enumerator::InitializePanel( "Animation Slots", CreatePanelSignature::Delegate( &Luna::AnimationConfigAsset::CreatePanel ) );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AnimationConfigAsset >(), &Luna::AnimationConfigAsset::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AnimationConfigAsset types.
// 
void AnimationConfigAsset::CleanupType()
{
  Reflect::UnregisterClass<Luna::AnimationConfigAsset>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function.
// 
Luna::PersistentDataPtr AnimationConfigAsset::Create( Reflect::Element* animConfig, Luna::AssetManager* manager )
{
  return new Luna::AnimationConfigAsset( Reflect::AssertCast< Asset::AnimationConfigAsset >( animConfig ), manager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AnimationConfigAsset::AnimationConfigAsset( Asset::AnimationConfigAsset* animConfig, Luna::AssetManager* manager )
: Luna::AssetClass( animConfig, manager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AnimationConfigAsset::~AnimationConfigAsset()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of animation slots.
// 
const M_strbool& AnimationConfigAsset::GetCategories() const
{
  const Asset::AnimationConfigAsset* pkg = GetPackage< Asset::AnimationConfigAsset >();
  return pkg->m_Categories;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the list of animation slots.
// 
void AnimationConfigAsset::SetCategories( const M_strbool& slots )
{
  Asset::AnimationConfigAsset* pkg = GetPackage< Asset::AnimationConfigAsset >();
  pkg->m_Categories = slots;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of animation slots.
// 
const M_strbool& AnimationConfigAsset::GetModifiers() const
{
  const Asset::AnimationConfigAsset* pkg = GetPackage< Asset::AnimationConfigAsset >();
  return pkg->m_Modifiers;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the list of animation slots.
// 
void AnimationConfigAsset::SetModifiers( const M_strbool& slots )
{
  Asset::AnimationConfigAsset* pkg = GetPackage< Asset::AnimationConfigAsset >();
  pkg->m_Modifiers = slots;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to the configuration file for animation slots.
// 
const std::string& AnimationConfigAsset::GetConfigurationFile() const
{
  return AnimToolLib::AnimConfig::GetInstance()->GetConfigFile();
}

///////////////////////////////////////////////////////////////////////////////
// Enables the static "Asset" panel for asset classes.
// 
bool AnimationConfigAsset::ValidatePanel( const std::string& name )
{
  if ( name == "Animation Slots" )
  {
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Creates the static panel for this asset.
// 
void AnimationConfigAsset::CreatePanel( CreatePanelArgs& args )
{
  // Create the normal Luna::AssetClass panel
#pragma TODO( "Fix panel" )
  //Luna::AssetClass::CreatePanel( args );

  Inspect::Action* reloadButton = NULL;

  // Create the panels specific to this class.
  args.m_Enumerator->PushPanel( "Animation Slots", true );
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "Configuration File" );

      Inspect::Action* editButton = args.m_Enumerator->AddAction( Inspect::ActionSignature::Delegate ( &AnimationConfigAsset::OnEditButton ) );
      editButton->SetText( "Edit" );
      editButton->SetToolTip( "Edit the configuration file with Notepad." );

      reloadButton = args.m_Enumerator->AddAction( Inspect::ActionSignature::Delegate ( &AnimationConfigAsset::OnReloadButton ) );
      reloadButton->SetText( "Reload" );
      reloadButton->SetToolTip( "Reload the configuration file." );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      const std::string& (AnimToolLib::AnimConfig::*getConfigFileFunc)() const = &AnimToolLib::AnimConfig::GetConfigFile;
      void (AnimToolLib::AnimConfig::*setConfigFileFunc)( const std::string& ) = NULL;
      typedef Nocturnal::MemberProperty< AnimToolLib::AnimConfig, std::string > Property;
      typedef Nocturnal::SmartPtr< Property > PropertyPtr;
      PropertyPtr prop = new Property( AnimToolLib::AnimConfig::GetInstance(), getConfigFileFunc, setConfigFileFunc );
      Inspect::Value* config = args.m_Enumerator->AddValue< std::string >( prop.Ptr() );
      config->SetReadOnly( true );
    }
    args.m_Enumerator->Pop();

    // No support for multiselection for now
    if ( args.m_Selection.Size() == 1 )
    {
      Luna::AssetNode* node = Reflect::AssertCast< Luna::AssetNode >( args.m_Selection.Front() );
      Luna::AnimationConfigAsset* configClass = Reflect::AssertCast< Luna::AnimationConfigAsset >( node->GetAssetClass() );
      args.m_Enumerator->PushPanel( "Categories", true );
      {
        Inspect::CheckList* checkList = args.m_Enumerator->AddCheckList< M_strbool >( new Nocturnal::MemberProperty< Luna::AnimationConfigAsset, M_strbool >( configClass, &AnimationConfigAsset::GetCategories, &AnimationConfigAsset::SetCategories ) );
        reloadButton->SetClientData( new Inspect::ClientDataControl( checkList ) );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushPanel( "Modifiers", true );
      {
        Inspect::CheckList* checkList = args.m_Enumerator->AddCheckList< M_strbool >( new Nocturnal::MemberProperty< Luna::AnimationConfigAsset, M_strbool >( configClass, &AnimationConfigAsset::GetModifiers, &AnimationConfigAsset::SetModifiers ) );
        reloadButton->SetClientData( new Inspect::ClientDataControl( checkList ) );
      }
      args.m_Enumerator->Pop();
    }
  }
  args.m_Enumerator->Pop();
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the Edit button is clicked.  Launches the Animation Config file
// in Notepad.
// 
void AnimationConfigAsset::OnEditButton( Inspect::Button* button )
{
  std::string configFile = AnimToolLib::AnimConfig::GetInstance()->GetConfigFile();
  DocumentManager docManager( SessionManager::GetInstance()->LaunchEditor( EditorTypes::Asset ) );
  DocumentPtr doc = new Document( configFile );
  if ( docManager.QueryCheckOut( doc ) )
  {
    FileSystem::Win32Name( configFile );
    std::string command( "cmd /c np \"" );
    command += configFile + "\""; 
    Windows::Execute( command, true );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when the Reload button is clicked.  Validates the animation slots
// on all open assets of type Luna::AnimationConfigAsset.
// 
void AnimationConfigAsset::OnReloadButton( Inspect::Button* button )
{
  if ( AnimToolLib::AnimConfig::GetInstance()->Reload() )
  {
    Reflect::Object* clientData = button->GetClientData();
    if ( clientData && clientData->HasType( Reflect::GetType< Inspect::ClientDataControl >() ) )
    {
      Inspect::Control* list = ( static_cast< Inspect::ClientDataControl* >( clientData ) )->m_Control;
      list->Read();
    }
  }
}

#include "Precompile.h"
#include "AnimationHelper.h"

#include "AnimGroupDlg.h"
#include "AnimationGroup.h"
#include "AnimationSetAsset.h"
#include "AssetEditor.h"
#include "PersistentNode.h"
#include "PromptNewExistingDlg.h"

#include "Asset/AnimationConfigAsset.h"
#include "Asset/AnimationGroup.h"
#include "Asset/AnimationChain.h"
#include "Asset/AnimationSetAsset.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ExtensionSpecs.h"
#include "Finder/LunaSpecs.h"
#include "UIToolKit/ListDialog.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Local helper function to prompt for a Maya file to use in animations.
// 
static inline bool PromptMayaFile( wxWindow* window, const Luna::AssetClass* asset, tuid& fileID )
{
  const std::string title = "Choose Maya File";
  const std::string desc = "Specify where the Maya file containing the animation data for this clip is located.";
  const std::string createLabel = "Create a new Maya file (default location provided)";
  const std::string existingLabel = "Use an existing Maya file";

  std::string newFileLoc = asset->GetFilePath();
  FileSystem::StripLeaf( newFileLoc );
  FileSystem::AppendPath( newFileLoc, "animations/" );
  FileSystem::AppendPath( newFileLoc, asset->GetName() );
  FinderSpecs::Extension::MAYA_BINARY.Modify( newFileLoc );

  std::string existingFileLoc = asset->GetFilePath();
  FileSystem::StripLeaf( existingFileLoc );

  PromptNewExistingDlg dlg( window, &AnimationHelper::CreateDefaultMayaFile, title, desc, createLabel, existingLabel );
  dlg.SetFinderSpec( &FinderSpecs::Extension::MAYA_BINARY );
  dlg.SetNewFile( newFileLoc );
  dlg.SetExistingFile( existingFileLoc );
  dlg.SetRequiresTuid( true );
  if ( dlg.ShowModal() == wxID_OK )
  {
    fileID = dlg.GetFileID();
    return true;
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Creates a Maya file at the specified location.  Returns true if successful.
// If this function returns false, an error message will be provided.
// 
bool AnimationHelper::CreateDefaultMayaFile( const std::string& path, std::string& error )
{
  tuid fileID = TUID::Null;
  try
  {
    FileSystem::MakePath( path, true );
    FileSystem::Copy( FinderSpecs::Luna::EMPTY_MAYA_FILE.GetFile( FinderSpecs::Luna::CONFIG_FOLDER ), path, false );
    fileID = File::GlobalManager().Open( path );
  }
  catch ( const Nocturnal::Exception& e )
  {
    error = "Failed to copy Maya template file to " + path + "." + "\n" + "Reason: " + e.Get();
  }
  return fileID != TUID::Null;
}

///////////////////////////////////////////////////////////////////////////////
// Handles all the UI for adding a new animation group to the currently selected
// animation sets.
// 
void AnimationHelper::AddNewAnimationGroup( AssetEditor* assetEditor )
{
  // Get all the selected animation sets
  Luna::AssetManager* assetManager = assetEditor->GetAssetManager();
  std::string errorMsg;
  S_tuid animConfigFiles;
  S_AnimationSetAssetDumbPtr animSets;
  OS_SelectableDumbPtr::Iterator selItr = assetManager->GetSelection().GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = assetManager->GetSelection().GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( *selItr );
    if ( node )
    {
      Luna::AnimationSetAsset* animSet = Reflect::ObjectCast< Luna::AnimationSetAsset >( node->GetAssetClass() );
      if ( animSet )
      {
        tuid configFile = animSet->GetPackage< Asset::AnimationSetAsset >()->m_AnimationConfig;
        animConfigFiles.insert( configFile );
        
        if ( animConfigFiles.size() > 1 )
        {
          errorMsg = "Unable to create an Animation Group because the selected Animation Sets do not all have the same AnimationConfig setting.  Please make sure that the AnimationConfig setting is the same on all the selected Animation Sets.";
          break;
        }

        animSets.insert( animSet );
      }
    }
  }

  if ( errorMsg.empty() && animSets.empty() )
  {
    errorMsg = "You must select an Animation Set in order to create an Animation Group.";
  }

  // Report any errors discovered while traversing the selection
  if ( !errorMsg.empty() )
  {
    wxMessageBox( errorMsg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Show the dialog to get the settings for the new animation group
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();
  V_string errors;

  Asset::AnimationGroupPtr animGroup;
  tuid animConfigFile = *animConfigFiles.begin();
  if ( animConfigFile == TUID::Null )
  {
    if ( wxMessageBox( "The selected Animation Set does not have an Animation Configuration file specified.  Would you like to create a default Animation Group anyway?", "Create default Animation Group?", wxCENTER | wxICON_QUESTION | wxYES_NO, assetEditor ) == wxYES )
    {
      animGroup = new Asset::AnimationGroup();
    }
  }
  else
  {
    Asset::AnimationConfigAssetPtr animConfig;
    try
    {
      animConfig = Asset::AssetClass::GetCachedAssetClass< Asset::AnimationConfigAsset >( *animConfigFiles.begin() );
    }
    catch ( const Nocturnal::Exception& e )
    {
      errorMsg = "Unable to load Animation Config from the selected Animation Set.\nReason: " + e.Get();
      wxMessageBox( errorMsg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
      return;
    }

    AnimGroupDlg dlg( assetEditor, animConfig, "New Animation Group" );
    if ( dlg.ShowModal() == wxID_OK )
    {
      animGroup = new Asset::AnimationGroup();
      animGroup->m_Category = dlg.GetCategory();
      dlg.GetModifiers( animGroup->m_Modifiers );
    }
  }

  if ( animGroup.ReferencesObject() )
  {
    // If the user clicks OK, iterate over the animation sets and create the new group
    for each ( Luna::AnimationSetAsset* animSet in animSets )
    {
      if ( animSet->ContainsMatchingGroup( animGroup ) )
      {
        errors.push_back( "There is already an Animation Group with these settings on " + animSet->GetFilePath() + "." );
        continue;
      }
      if ( !assetManager->IsEditable( animSet ) )
      {
        continue;
      }

      // Create an undoable command for adding the animation group to the animation set.
      Asset::AnimationGroupPtr clone = Reflect::AssertCast< Asset::AnimationGroup >( animGroup->Clone() );
      batch->Push( new AnimGroupExistenceCommand( Undo::ExistenceActions::Add, animSet, clone ) );
    }
  }

  if ( !batch->IsEmpty() )
  {
    assetManager->Push( batch );
  }

  if ( !errors.empty() )
  {
    UIToolKit::ListDialog dlg( assetEditor, "Error", "The following errors were encountered while creating Animation Groups:", errors );
    dlg.ShowModal();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Opens the dialog to allow editing a single Animation Group.  This operation
// is undoable.
// 
void AnimationHelper::EditAnimationGroup( AssetEditor* assetEditor )
{
  Selection& selection = assetEditor->GetAssetManager()->GetSelection();

  // Error checking
  if ( selection.GetItems().Empty() )
  {
    wxMessageBox( "You must select an Animation Group to edit.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }
  else if ( selection.GetItems().Size() > 1 )
  {
    wxMessageBox( "You must select only one Animation Group to edit at a time.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Make sure that the selection is an animation group
  Luna::PersistentNode* persistentNode = Reflect::ObjectCast< Luna::PersistentNode >( *( selection.GetItems().Begin() ) );
  if ( !persistentNode || !persistentNode->GetPersistentData< Luna::PersistentData >()->HasType( Reflect::GetType< Luna::AnimationGroup >() ) )
  {
    wxMessageBox( "The selected item is not an Animation Group. Please select an Animation Group to edit.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Make sure that the file can be edited.
  Luna::AnimationGroup* animGroupWrapper = persistentNode->GetPersistentData< Luna::AnimationGroup >();
  Luna::AnimationSetAsset* animSetWrapper = Reflect::ObjectCast< Luna::AnimationSetAsset >( persistentNode->GetAssetClass() );
  if ( !animSetWrapper || !assetEditor->GetAssetManager()->IsEditable( animSetWrapper ) )
  {
    return;
  }

  Asset::AnimationSetAsset* animSet = animSetWrapper->GetPackage< Asset::AnimationSetAsset >();
  tuid configFile = animSet->m_AnimationConfig;

  // Make sure that there is an animation config file
  if ( configFile == TUID::Null )
  {
    std::string msg( "No Animation Config was found. Please verify that the Animation Config setting on the Animation Set named " );
    msg += animSet->GetFullName() + " is valid.";
    wxMessageBox( msg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Get the animation config
  Asset::AnimationConfigAssetPtr animConfig;
  try
  {
    animConfig = Asset::AssetClass::GetCachedAssetClass< Asset::AnimationConfigAsset >( configFile );
  }
  catch ( const Nocturnal::Exception& e )
  {
    std::string msg = "Unable to load Animation Config from the selected Animation Set.\nReason: " + e.Get();
    wxMessageBox( msg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Run the dialog for editing the animation group
  AnimGroupDlg dlg( assetEditor, animConfig, "Edit Animation Group" );
  dlg.SetCategory( animGroupWrapper->GetCategory() );
  dlg.SetModifiers( animGroupWrapper->GetModifiers() );
  if ( dlg.ShowModal() == wxID_OK )
  {
    M_string modifiers;
    dlg.GetModifiers( modifiers );

    const bool isCategoryDifferent = animGroupWrapper->GetCategory() != dlg.GetCategory();
    const bool isModifierDifferent = animGroupWrapper->GetModifiers() != modifiers;

    if ( !( isCategoryDifferent || isModifierDifferent ) )
    {
      // Nothing to do, bail
      return;
    }

    // Make sure that the settings do not match an existing group on this animation set.
    Asset::AnimationGroupPtr temp = new Asset::AnimationGroup();
    temp->m_Category = dlg.GetCategory();
    dlg.GetModifiers( temp->m_Modifiers );
    if ( animSetWrapper->ContainsMatchingGroup( temp ) )
    {
      wxMessageBox( "There is already an Animation Group with these settings on this Animation Set.  Operation aborted!", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
      return;
    }

    // Create an undoable command to make the change
    Undo::BatchCommandPtr batch = new Undo::BatchCommand();

    if ( isCategoryDifferent )
    {
      batch->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::AnimationGroup, std::string >( animGroupWrapper, &Luna::AnimationGroup::GetCategory, &Luna::AnimationGroup::SetCategory ), dlg.GetCategory() ) );
    }

    if ( isModifierDifferent )
    {
      batch->Push( new Undo::PropertyCommand< M_string >( new Nocturnal::MemberProperty< Luna::AnimationGroup, M_string >( animGroupWrapper, &Luna::AnimationGroup::GetModifiers, &Luna::AnimationGroup::SetModifiers ), modifiers ) );
    }

    if ( !batch->IsEmpty() )
    {
      assetEditor->GetAssetManager()->Push( batch );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Creates a new Animation Chain and prompts the user for a Maya file to add
// to the chain.
// 
void AnimationHelper::AddClipToNewChain( AssetEditor* assetEditor )
{
#pragma TODO( "Make this work with undo/redo" )
  Selection& selection = assetEditor->GetAssetManager()->GetSelection();
  if ( selection.GetItems().Empty() )
  {
    wxMessageBox( "You must select an Animation Group to add a new Animation Clip.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Build a list of animation groups that we wish to add clips to.
  typedef Nocturnal::OrderedSet< Luna::AssetClass* > OS_AssetClassDumbPtr;
  typedef std::set< Asset::AnimationGroup* > S_AnimationGroupDumbPtr;
  S_AnimationGroupDumbPtr animGroups;
  OS_AssetClassDumbPtr assets;
  OS_SelectableDumbPtr::Iterator selItr = selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* assetNode = Reflect::AssertCast< Luna::AssetNode >( *selItr );
    Luna::PersistentNode* persistNode = Reflect::ObjectCast< Luna::PersistentNode >( assetNode );
    if ( persistNode && persistNode->GetPersistentData< Luna::PersistentData >()->GetPackage< Reflect::Element >()->HasType( Reflect::GetType< Asset::AnimationGroup >() ) )
    {
      if ( persistNode->GetAssetManager()->IsEditable( persistNode->GetAssetClass() ) )
      {
        assets.Append( persistNode->GetAssetClass() );
        animGroups.insert( persistNode->GetPersistentData< Luna::PersistentData >()->GetPackage< Asset::AnimationGroup >() );
      }
    }
  }

  if ( animGroups.empty() )
  {
    wxMessageBox( "None of the selected items are Animation Groups (or they could not be checked out).  Please select at least one Animation Group that you would like to add an Animation Clip to.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Prompt for the Maya file to create the clips from (use the last selected asset class
  // to build the default Maya file locations).
  tuid mayaFile = TUID::Null;
  if ( PromptMayaFile( assetEditor, assets.Back(), mayaFile ) )
  {
    // Add an animation clip to each animation group.
    for each ( Asset::AnimationGroup* animGroup in animGroups )
    {
      Asset::AnimationChainPtr animChain = new Asset::AnimationChain();
      Asset::AnimationClipDataPtr animClip = new Asset::AnimationClipData();
      animClip->m_ArtFile = mayaFile;
      animChain->m_AnimationClips.push_back( animClip );
      animGroup->m_AnimationChains.push_back( animChain );
      animGroup->RaiseChanged( animGroup->GetClass()->FindField( &Asset::AnimationGroup::m_AnimationChains ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Creates a new animation clip (prompting the user for the Maya file to use)
// and adds it to the selected chain.
// 
void AnimationHelper::AddClipToExistingChain( AssetEditor* assetEditor )
{
#pragma TODO( "Make this work with undo/redo" )
  Selection& selection = assetEditor->GetAssetManager()->GetSelection();
  if ( selection.GetItems().Empty() )
  {
    wxMessageBox( "You must select an Animation Chain to add a new Animation Clip.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Build a list of the animation chains that we are going to add clips to.
  typedef Nocturnal::OrderedSet< Luna::AssetClass* > OS_AssetClassDumbPtr;
  typedef std::set< Asset::AnimationChain* > S_AnimationChain;
  S_AnimationChain animChains;
  OS_AssetClassDumbPtr assets;
  OS_SelectableDumbPtr::Iterator selItr = selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* assetNode = Reflect::AssertCast< Luna::AssetNode >( *selItr );
    Luna::PersistentNode* persistNode = Reflect::ObjectCast< Luna::PersistentNode >( assetNode );
    if ( persistNode && persistNode->GetPersistentData< Luna::PersistentData >()->GetPackage< Reflect::Element >()->HasType( Reflect::GetType< Asset::AnimationChain >() ) )
    {
      if ( persistNode->GetAssetManager()->IsEditable( persistNode->GetAssetClass() ) )
      {
        assets.Append( persistNode->GetAssetClass() );
        animChains.insert( persistNode->GetPersistentData< Luna::PersistentData >()->GetPackage< Asset::AnimationChain >() );
      }
    }
  }

  if ( animChains.empty() )
  {
    wxMessageBox( "None of the selected items are Animation Chains (or they could not be checked out).  Please select at least one Animation Chain that you would like to add an Animation Clip to.", "Error", wxCENTER | wxICON_ERROR | wxOK, assetEditor );
    return;
  }

  // Prompt for the Maya file.  Use the last selected asset class as the basis for the
  // default paths.
  tuid mayaFile = TUID::Null;
  if ( PromptMayaFile( assetEditor, assets.Back(), mayaFile ) )
  {
    // Add an animation clip to each animation chain.
    for each ( Asset::AnimationChain* animChain in animChains )
    {
      Asset::AnimationClipDataPtr animClip = new Asset::AnimationClipData();
      animClip->m_ArtFile = mayaFile;
      animChain->m_AnimationClips.push_back( animClip );
      animChain->RaiseChanged( animChain->GetClass()->FindField( &Asset::AnimationChain::m_AnimationClips ) );
    }
  }
}

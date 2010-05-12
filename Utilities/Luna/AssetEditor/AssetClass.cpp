#include "Precompile.h"
#include "AssetClass.h"

#include "AnimationAttribute.h"
#include "AssetEditor.h"
#include "AssetDocument.h"
#include "AssetManager.h"
#include "AttributeContainer.h"
#include "AttributeExistenceCommand.h"
#include "AttributeNode.h"
#include "ContextMenuCallbacks.h"
#include "FieldFactory.h"
#include "PersistentDataFactory.h"
#include "ShaderGroupDlg.h"

#include "Asset/AnimationSetAsset.h"
#include "Asset/ArtFileAttribute.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/Exceptions.h"
#include "Asset/ShaderGroupAttribute.h"
#include "Asset/ShaderUsagesAttribute.h"
#include "Asset/StandardShaderAsset.h"
#include "AssetManager/AssetManager.h"
#include "Common/CommandLine.h"
#include "Common/Container/Insert.h" 
#include "Common/String/Natural.h"
#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ContentSpecs.h"
#include "Console/Console.h"
#include "Inspect/ClipboardElementArray.h"
#include "Core/Enumerator.h"
#include "Asset/EntityManifest.h"
#include "UIToolKit/ImageManager.h"

#include <algorithm>

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AssetClass );


///////////////////////////////////////////////////////////////////////////////
// Sorting algorithm for shader usages.  Does a string comparison on the title
// of each shader usage.
// 
struct SortShaderUsages
{
  bool operator()( const Asset::ShaderUsagePtr& lhs, const Asset::ShaderUsagePtr& rhs )
  {
    return strinatcmp( lhs->GetTitle().c_str(), rhs->GetTitle().c_str() ) < 0;
  }
};

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AssetClass types.
// 
void AssetClass::InitializeType()
{
  Reflect::RegisterClass<Luna::AssetClass>( "Luna::AssetClass" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AssetClass >(), &Luna::AssetClass::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AssetClass types.
// 
void AssetClass::CleanupType()
{
  Reflect::UnregisterClass<Luna::AssetClass>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function for making an instance of this class.
// 
Luna::PersistentDataPtr AssetClass::Create( Reflect::Element* assetClass, Luna::AssetManager* manager )
{
  return new Luna::AssetClass( Reflect::AssertCast< Asset::AssetClass >( assetClass ), manager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetClass::AssetClass( Asset::AssetClass* assetClass, Luna::AssetManager* manager )
: Luna::PersistentData( assetClass, manager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetClass::~AssetClass()
{
}

///////////////////////////////////////////////////////////////////////////////
// Loads the persistent data into this asset class.
// 
void AssetClass::Pack()
{
  M_AttributeSmartPtr::const_iterator attrItr = m_Attributes.begin();
  M_AttributeSmartPtr::const_iterator attrEnd = m_Attributes.end();
  for ( ; attrItr != attrEnd; ++attrItr )
  {
    const Luna::AttributeWrapperPtr& attrib = attrItr->second;
    attrib->Pack();
  }

  __super::Pack();

  std::string error;
  if ( !GetPackage< Asset::AssetClass >()->ValidateClass( error ) )
  {
    std::string msg = "The following error was encountered while packing '" + GetName() + "'.\n\n";
    msg += error;
    wxMessageBox( msg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetAssetManager()->GetAssetEditor() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Loads the persistent data into this asset class.
// 
void AssetClass::Unpack()
{
  __super::Unpack();

  Asset::AssetClass* package = GetPackage< Asset::AssetClass >();

  NOC_ASSERT( package );
  if ( !m_Attributes.empty() )
  {
    NOC_ASSERT( package->GetAttributes().empty() );
  }

  Attribute::M_Attribute::const_iterator attrItr = package->GetAttributes().begin();
  Attribute::M_Attribute::const_iterator attrEnd = package->GetAttributes().end();
  for ( ; attrItr != attrEnd; ++attrItr )
  {
    const Attribute::AttributePtr& attribute = attrItr->second;
    Luna::AttributeWrapperPtr attributeWrapper = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( attribute, GetAssetManager() );
    if ( !attributeWrapper.ReferencesObject() )
    {
      throw ( Nocturnal::Exception( "Internal error - Unable to create attribute in Luna::AssetClass::Unpack" ) );
    }
    AddAttribute( attributeWrapper, false );
  }

  std::string error;
  if ( !package->ValidateClass( error ) )
  {
    std::string msg = "The following error was encountered while unpacking '" + GetName() + "'.\n\n";
    msg += error;
    wxMessageBox( msg.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetAssetManager()->GetAssetEditor() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Saves this asset class to disk.  Returns true on success.
// 
bool AssetClass::Save( std::string& error )
{
  Pack();

  Asset::AssetClass* assetClass = GetPackage< Asset::AssetClass >();
  tuid fileID = assetClass->m_AssetClassID;

  std::string path = GetFilePath();
  if ( path.empty() )
  {
    Console::Error( "No file path for asset.\n" );
    return false;
  }

  bool saved = false;
  try
  {
    Reflect::ArchiveBinary::ToFile( assetClass, path, new Asset::AssetVersion(), NULL );
    Asset::AssetClass::InvalidateCache( assetClass->m_AssetClassID );
    saved = true;
  }
  catch ( const Nocturnal::Exception& e )
  {
    error = e.Get();
    saved = false;
  }

  return saved;
}


///////////////////////////////////////////////////////////////////////////////
// Returns true if this asset is able to be built.
// 
bool AssetClass::IsBuildable() const
{
  return GetPackage< Asset::AssetClass >()->IsBuildable();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this asset is able to be viewed.
// 
bool AssetClass::IsViewable() const
{
  return GetPackage< Asset::AssetClass >()->IsViewable();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this asset has art data that can be exported.
// 
bool AssetClass::IsExportable() const
{
  Attribute::AttributeViewer< Asset::ArtFileAttribute > artViewer( GetPackage< Asset::AssetClass >() );
  return artViewer.Valid();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this asset, determined by the file name on disk where
// this asset is located.
// 
const std::string& AssetClass::GetName() const
{
  if ( m_Name.empty() )
  {
    const Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      m_Name = package->GetShortName();
    }
    catch ( Nocturnal::Exception& e )
    {
      Console::Error( "Unable to get asset name: %s\n", e.what() );
      NOC_BREAK();
    }
  }

  return m_Name;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to this asset's location on disk.
// 
const std::string& AssetClass::GetFilePath() const
{
  if ( m_FilePath.empty() )
  {
    const Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      m_FilePath = package->GetFilePath();
    }
    catch ( const Nocturnal::Exception& e )
    {
      Console::Error( "Unable to get asset path: %s\n", e.what() );
      NOC_BREAK();
    }
  }

  return m_FilePath;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the file ID tuid for this asset.
// 
tuid AssetClass::GetFileID() const
{
  const Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
  return package->GetFileID();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the icon to use for this asset.
// 
std::string AssetClass::GetIcon() const
{
  // get the engine type icon
  std::string icon = Asset::AssetClass::GetEngineTypeIcon( GetPackage<Asset::AssetClass>()->GetEngineType() );
  
  if ( icon.empty() )
  {
    icon = "null_16.png"; 
  }

  return icon;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of attributes belonging to this asset.
// 
const M_AttributeSmartPtr& AssetClass::GetAttributes()
{
  return m_Attributes;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the attribute in the specified slot.
// 
Luna::AttributeWrapper* AssetClass::FindAttribute( i32 slot )
{
  M_AttributeSmartPtr::const_iterator found = m_Attributes.find( slot );
  if ( found != m_Attributes.end() )
  {
    return found->second;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the specified menu set with the items appropriate to editing any
// asset class.
// 
void AssetClass::PopulateContextMenu( ContextMenuItemSet& menu )
{
  Asset::AssetClass* assetPkg = GetPackage< Asset::AssetClass >();
  Luna::ObjectPtr clientData = new Luna::AssetManagerClientData( m_AssetManager );

  const size_t numSelected = m_AssetManager->GetSelection().GetItems().Size();
  const bool isAnimationSet = assetPkg->HasType( Reflect::GetType< Asset::AnimationSetAsset >() );
  const bool isShader = assetPkg->HasType( Reflect::GetType< Asset::ShaderAsset >() );
  const bool isLevel = assetPkg->GetEngineType() == Asset::EngineTypes::Level;

  const bool isUserAssetAdmin = Nocturnal::GetCmdLineFlag( "asset_admin" );

  bool areAssetsLocked = true;
  const char* networkLockAssetsPath = getenv( NOCTURNAL_STUDIO_PREFIX "NETWORK_LOCK_ASSETS_FILE" );
  if ( networkLockAssetsPath != NULL )
  {
    // Faster than FileSystem::Exists
    WIN32_FILE_ATTRIBUTE_DATA lockFileStats;
    if ( ::GetFileAttributesEx( networkLockAssetsPath, GetFileExInfoStandard, &lockFileStats ) )
    {
      areAssetsLocked = true;
    }
    else
    {
      areAssetsLocked = false;
    }
  }

  V_string staticContentFiles;
  ::AssetManager::GetStaticContentFiles( GetFileID(), staticContentFiles );

  const bool canPreview = staticContentFiles.size() ? true : false;
  std::string previewTip = canPreview
    ? "Preview the asset in the preview panel."
    : "This asset cannot be previewed in the preview panel.";

  const bool canDuplicate = ( numSelected == 1 ) && ( isUserAssetAdmin || ( !areAssetsLocked && ( isShader || isAnimationSet ) ) );
  std::string duplicateToolTip = canDuplicate 
    ? "Create a copy of this asset using the Asset Manager wizard."
    : "Duplicating assets is not allowed or not available at this time.";

  const bool canRename = ( numSelected == 1 ) && ( isUserAssetAdmin || ( !isLevel && !areAssetsLocked ) );
  std::string renameToolTip = canRename 
    ? "Rename or Move this asset using the Asset Manager wizard."
    : "Renaming assets is not allowed or not available at this time.";

  const bool canDelete = ( numSelected == 1 ) && ( isUserAssetAdmin || ( !isLevel && !areAssetsLocked ) );
  std::string deleteToolTip = canDelete 
    ? "Delete this asset using the Asset Manager wizard."
    : "Deleting assets is not allowed or not available at this time.";

  ContextMenuItemPtr menuItem = new ContextMenuItem( "Preview" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::OnAssetPreview ) );
  menuItem->Enable( canPreview );
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( "Add Attribute" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::PromptAddAttributes ) );
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( "Duplicate", duplicateToolTip.c_str() );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnDuplicateAsset ), new Luna::AssetManagerClientData( m_AssetManager ) );
  menuItem->Enable( canDuplicate );
  menu.AppendItem( menuItem );

  menuItem = new ContextMenuItem( "Rename/Move", renameToolTip.c_str() );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnRenameAsset ), clientData );
  menuItem->Enable( canRename );
  menu.AppendItem( menuItem );

  menuItem = new ContextMenuItem( "Delete", deleteToolTip.c_str() );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnDeleteAsset ), clientData );
  menuItem->Enable( canDelete );
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( "Save", "Save selected Asset(s)", UIToolKit::GlobalImageManager().GetBitmap( "save_16.png" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnSaveSelectedAssets ), clientData );
  menu.AppendItem( menuItem );
  menu.AppendSeparator();

  SubMenuPtr rcsSubMenu = new SubMenu( "Perforce" );

  menuItem = new ContextMenuItem( "Check Out" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnCheckOutSelectedAssets ), clientData );
  rcsSubMenu->AppendItem( menuItem );

  menuItem = new ContextMenuItem( "Revision History" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnRevisionHistorySelectedAssets ), clientData );
  rcsSubMenu->AppendItem( menuItem );

  menu.AppendItem( rcsSubMenu );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( "Close" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnCloseSelectedAssets ), clientData );
  menu.AppendItem( menuItem );

  std::string unused;
  if (
    assetPkg->GetAttribute( Reflect::GetType< Asset::ShaderGroupAttribute >() ).ReferencesObject() ||
    assetPkg->ValidateAttribute( Reflect::AssertCast< Attribute::AttributeBase >( Reflect::Registry::GetInstance()->CreateInstance( Reflect::GetType< Asset::ShaderGroupAttribute >() ) ), unused )
    )
  {
    bool enable = false;
    S_AssetClassDumbPtr selection;
    if ( m_AssetManager->GetSelectedAssets( selection ) == 1 )
    {
      enable = *( selection.begin() ) == this;
    }

    menu.AppendSeparator();
    menuItem = new ContextMenuItem( "Manage Shader Groups" );
    menuItem->Enable( enable );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AssetClass::ManageShaderGroups ) );
    menu.AppendItem( menuItem );
  }

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( "Expand" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::OnExpandSelectedAssets ) );
  menu.AppendItem( menuItem );

  menuItem = new ContextMenuItem( "Collapse" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::OnCollapseSelectedAssets ) );
  menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Registers the specified node as a reference of this asset class.
// 
void AssetClass::RegisterAssetReferenceNode( Luna::AssetReferenceNode* node )
{
  // Sanity check: node better NOT be in the list
  NOC_ASSERT( m_References.find( node ) == m_References.end() );
  m_References.insert( node );
}

///////////////////////////////////////////////////////////////////////////////
// Unregisters the specifed node as a reference of this asset class.
// 
void AssetClass::UnregisterAssetReferenceNode( Luna::AssetReferenceNode* node )
{
  // Sanity check: node better be in the list
  NOC_ASSERT( m_References.find( node ) != m_References.end() );
  m_References.erase( node );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of nodes that are references of this asset class.
// 
const S_AssetReferenceNodeDumbPtr& AssetClass::GetAssetReferenceNodes() const
{
  return m_References;
}

///////////////////////////////////////////////////////////////////////////////
// Displays a dialog for editing the shader groups of the first selected asset.
// 
void AssetClass::ManageShaderGroups( const ContextMenuArgsPtr& args )
{
  if ( m_AssetManager->IsEditable( this ) )
  {
    UpdateShaderUsages();

    ShaderGroupDlg dlg( m_AssetManager->GetAssetEditor(), this );
    if ( dlg.ShowModal() == wxID_OK )
    {
      Asset::ShaderGroupAttributePtr swapShadersAttrib = dlg.GetShaderGroupAttribute();

      Luna::AttributeWrapper* oldAttribute = FindAttribute( Reflect::GetType< Asset::ShaderGroupAttribute >() );
      if ( oldAttribute )
      {
        args->GetBatch()->Push( new AttributeExistenceCommand( Undo::ExistenceActions::Remove, this, oldAttribute ) );
      }

      if ( swapShadersAttrib->m_ShaderGroups.size() != 0 )
      {
        Luna::AttributeWrapperPtr newAttribute = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( swapShadersAttrib, m_AssetManager );

        args->GetBatch()->Push( new AttributeExistenceCommand( Undo::ExistenceActions::Add, this, newAttribute ) );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Updates the shader usages for this asset (assuming this asset can have a shader
// usages attribute).
// 
void AssetClass::UpdateShaderUsages()
{
  // Determine which shaders this asset is currently using
  typedef std::map< tuid, std::string > M_TuidToStr;
  M_TuidToStr currentShaderIds; // maps shader ID to name of the shader group it belongs to (if any)

  Attribute::AttributeViewer< Asset::ArtFileAttribute > model ( GetPackage< Asset::AssetClass >() );
  if ( model.Valid() && m_AssetManager->IsEditable( this ) )
  {
    std::string artFile;
    try
    {
      artFile = model->GetFilePath();
    }
    catch( const Nocturnal::Exception& )
    {
    }

    if (!artFile.empty())
    {
      std::string assetManifestFile = FinderSpecs::Content::MANIFEST_DECORATION.GetExportFile( artFile, model->m_FragmentNode );

      if (FileSystem::Exists(assetManifestFile))
      {
        Asset::EntityManifestPtr manifest;
        try
        {
          manifest = Reflect::Archive::FromFile<Asset::EntityManifest>(assetManifestFile);
        }
        catch ( const Reflect::Exception& )
        {
        }

        if (manifest.ReferencesObject())
        {
          for ( V_tuid::const_iterator shaderIDItr = manifest->m_Shaders.begin(),
            shaderIDEnd = manifest->m_Shaders.end();
            shaderIDItr != shaderIDEnd; ++shaderIDItr )
          {
            currentShaderIds.insert( M_TuidToStr::value_type( *shaderIDItr, "" ) );
          }
        }
      }
    }

    Asset::ShaderUsagesAttributePtr shaderUsages = Reflect::DangerousCast< Asset::ShaderUsagesAttribute >( GetPackage< Asset::AssetClass >()->GetAttribute( Reflect::GetType< Asset::ShaderUsagesAttribute >() ) );
    if ( !shaderUsages.ReferencesObject() )
    {
      // add the shader usages attribute
      shaderUsages = new Asset::ShaderUsagesAttribute();
      Undo::ExistenceCommandPtr cmd = new AttributeExistenceCommand( Undo::ExistenceActions::Add, this, PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( shaderUsages, m_AssetManager ) );
      // Operation is not undoable, so don't hold on to the command.
    }

    // now modify the shader usage attribute to have an entry per shader
    // adding and removing entries as necessary

    // first add any shader group shaders to the list
    Attribute::AttributeViewer< Asset::ShaderGroupAttribute > shaderGroup( GetPackage< Asset::AssetClass >() );
    if ( shaderGroup.Valid() )
    {
      Asset::V_ShaderGroupSmartPtr::const_iterator groupItr = shaderGroup->m_ShaderGroups.begin();
      Asset::V_ShaderGroupSmartPtr::const_iterator groupEnd = shaderGroup->m_ShaderGroups.end();
      for ( ; groupItr != groupEnd; ++groupItr )
      {
        const Asset::ShaderGroup* group = *groupItr;
        M_tuid::const_iterator remapItr = group->m_ShaderMapping.begin();
        M_tuid::const_iterator remapEnd = group->m_ShaderMapping.end();
        for ( ; remapItr != remapEnd; ++remapItr )
        {
          if ( remapItr->first != remapItr->second && remapItr->second != TUID::Null )
          {
            Nocturnal::Insert< M_TuidToStr >::Result inserted = currentShaderIds.insert( M_TuidToStr::value_type( remapItr->second, group->GetTitle() ) );
            if ( !inserted.second )
            {
              if ( !inserted.first->second.empty() )
              {
                inserted.first->second += ", ";
                inserted.first->second += group->GetTitle();
              }
            }
          }
        }
      }
    }

    // next remove the shader usages that are no longer valid
    Asset::V_ShaderUsage validUsages;
    Asset::V_ShaderUsage::iterator itr = shaderUsages->m_ShaderUsages.begin();
    Asset::V_ShaderUsage::iterator end = shaderUsages->m_ShaderUsages.end();

    for ( ; itr != end; ++itr )
    {
      M_TuidToStr::iterator found = currentShaderIds.find( (*itr)->m_ShaderID );
      if ( found != currentShaderIds.end() )
      {
        (*itr)->m_ShaderGroupName = found->second;
        currentShaderIds.erase( (*itr)->m_ShaderID );
        validUsages.push_back( (*itr) );
      }
    }

    // for what is left, add new shader usages
    M_TuidToStr::const_iterator newItr = currentShaderIds.begin();
    M_TuidToStr::const_iterator newEnd = currentShaderIds.end();
    for ( ; newItr != newEnd; ++newItr )
    {
      Asset::ShaderUsagePtr shaderUsage = new Asset::ShaderUsage();
      shaderUsage->m_ShaderID = (newItr->first);
      if ( !newItr->second.empty() )
      {
        shaderUsage->m_ShaderGroupName = newItr->second;
      }
      validUsages.push_back( shaderUsage );
    }

    // alphabetize the shaders by name
    std::sort( validUsages.begin(), validUsages.end(), SortShaderUsages() );

    // now update the attribute with the valid usages
    shaderUsages->m_ShaderUsages = validUsages;

    // Notify listeners that the persistent data has changed.
    shaderUsages->RaiseChanged( shaderUsages->GetClass()->FindField( &Asset::ShaderUsagesAttribute::m_ShaderUsages ) );

    m_AssetManager->ClearUndoQueue();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds an Animation Attribute to this Asset if possible (if there's not already
// one) and associates a new Animation Set with the Animation Attribute.
// 
Undo::CommandPtr AssetClass::AddAnimationSet( std::string& msg )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  // Make sure that there is an AnimationAttribute on this asset, and if not, add one.
  Asset::AssetClass* pkg = GetPackage< Asset::AssetClass >();
  Luna::AnimationAttribute* animationAttrib = Reflect::ObjectCast< Luna::AnimationAttribute >( FindAttribute( Reflect::GetType< Asset::AnimationAttribute >() ) );

  if ( !animationAttrib )
  {
    // No AnimationAttribute so add it.
    Asset::AnimationAttributePtr animPkg = Reflect::AssertCast< Asset::AnimationAttribute >( Reflect::Registry::GetInstance()->CreateInstance( Reflect::GetType< Asset::AnimationAttribute >() ) );
    if ( !pkg->ValidateAttribute( animPkg, msg ) )
    {
      msg += "An Animation attribute could not be added to " + GetName() + ".";
      return NULL;
    }

    // Add an undoable command to the batch that adds the animation attribute to this asset.
    Luna::AttributeWrapperPtr newAttrib = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( animPkg, m_AssetManager );
    animationAttrib = Reflect::AssertCast< Luna::AnimationAttribute >( newAttrib );
    batch->Push( new AttributeExistenceCommand( Undo::ExistenceActions::Add, this, animationAttrib ) );
  }
  else
  {
    // Already has an AnimationAttribute, make sure that there's no AnimationSet associated.
    if ( animationAttrib->GetAnimationSetID() != TUID::Null )
    {
      std::stringstream error;
      error << "Failed to add an Animation Set to " << GetFilePath() << " because it already has a reference to an Animation Set in its Animation attribute.";
      msg += error.str();
      return NULL;
    }
  }

  // If we get to here, we better have an AnimationAttribute to work with.
  NOC_ASSERT( animationAttrib );

  // Create the AnimationSet and add a reference to it on the AnimationAttribute...
  Asset::AnimationSetAssetPtr animationSetPkg = Reflect::AssertCast< Asset::AnimationSetAsset >( Reflect::Registry::GetInstance()->CreateInstance( Reflect::GetType< Asset::AnimationSetAsset >() ) );
  animationSetPkg->MakeDefault();

  // Update the ArtFileAttribute reference on the AnimationSet to match this asset class.
  Luna::AttributeWrapper* artFileAttribute = FindAttribute( Reflect::GetType< Asset::ArtFileAttribute >() );
  if ( artFileAttribute )
  {
    Attribute::AttributeEditor< Asset::ArtFileAttribute > artFileEditor( animationSetPkg );
    if ( artFileEditor.Valid() )
    {
      artFileEditor->m_FileID = artFileAttribute->GetPackage< Asset::ArtFileAttribute >()->m_FileID;
    }
  }

  // Figure out where to put the new animation set.
  tuid fileID = TUID::Null;
  std::string filePath = GetFilePath();
  FileSystem::StripExtension( filePath );
  FinderSpecs::Asset::ANIMSET_DECORATION.Modify( filePath );

  if ( FileSystem::Exists( filePath ) )
  {
    std::stringstream stream;
    stream << "There is already a file located at: " << filePath << ". You need to hook up the Animation Set manually.";
    wxMessageBox( stream.str(), "Error", wxCENTER | wxICON_ERROR | wxOK, GetAssetManager()->GetAssetEditor() );
    return NULL;
  }

  try
  {
    fileID = File::GlobalManager().Open( filePath );
  }
  catch ( const Nocturnal::Exception& e )
  {
    msg += "Error: " + e.Get();
    return NULL;
  }

  if ( fileID == TUID::Null )
  {
    msg += "Unable to assign a TUID to " + filePath;
    return NULL;
  }

  // Let the Asset Editor know that we need a new AssetClass created.
  animationSetPkg->m_AssetClassID = fileID;
  animationSetPkg->Serialize();
  animationSetPkg = NULL;
  std::string loadMsg;
  Luna::AssetClass* animationSet = m_AssetManager->Open( filePath, loadMsg, false );
  if ( !animationSet )
  {
    msg += "Error loading " + filePath + ": " + loadMsg;
    return NULL;
  }

  // Make an undoable command to associate the AnimationAttribute with the new AnimationSet.
  batch->Push( new Undo::PropertyCommand< tuid >( new Nocturnal::MemberProperty< Luna::AnimationAttribute, tuid >( animationAttrib, &Luna::AnimationAttribute::GetAnimationSetID, &Luna::AnimationAttribute::SetAnimationSetID ), fileID ) );

  // Return the batch if it's not empty.
  if ( !batch->IsEmpty() )
  {
    return batch;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Called when a control in the enumerator is changed while this asset is 
// selected.  Derived classes can NOC_OVERRIDE this function to do additional work.
// 
void AssetClass::Changed( Inspect::Control* control )
{
  GetPackage< Asset::AssetClass >()->RaiseChanged();
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to determine if the clipboard data references one or more
// attributes.
// 
static inline bool IsAttributeData( const Inspect::ReflectClipboardDataPtr& data )
{
  Inspect::ClipboardElementArray* elementArray = Reflect::ObjectCast< Inspect::ClipboardElementArray >( data );
  if ( elementArray )
  {
    const Reflect::Class* classToCheck = Reflect::Registry::GetInstance()->GetClass( elementArray->GetCommonBaseTypeID() );
    if ( classToCheck->HasType( Reflect::GetType< Attribute::AttributeBase >() ) )
    {
      // Clipboard contains one or more attributes.
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Assets can accept clipboard data from attributes or other assets.
// 
bool AssetClass::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  if ( __super::CanHandleClipboardData( data ) )
  {
    return true;
  }

  return IsAttributeData( data );
}

///////////////////////////////////////////////////////////////////////////////
// Copies data from the clipboard into this asset.
// 
bool AssetClass::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  if ( CanHandleClipboardData( data ) )
  {
    // Make a copy of the data so that we can use it directly without fear of 
    // modifying what's on the clipboard.
    Inspect::ClipboardElementArrayPtr dataCopy = Reflect::AssertCast< Inspect::ClipboardElementArray >( data->Clone() );
    if ( IsAttributeData( data ) )
    {
      // Build a list of attributes from the source data.
      Attribute::M_Attribute srcAttributes;
      Reflect::V_Element::const_iterator elementItr = dataCopy->m_Elements.begin();
      Reflect::V_Element::const_iterator elementEnd = dataCopy->m_Elements.end();
      for ( ; elementItr != elementEnd; ++elementItr )
      {
        Attribute::AttributeBase* attribPkg = Reflect::ObjectCast< Attribute::AttributeBase >( *elementItr );
        if ( attribPkg )
        {
          srcAttributes.insert( Attribute::M_Attribute::value_type( attribPkg->GetSlot(), attribPkg ) );
        }
      }

      // Copy all the attribute data onto this object.
      if ( !srcAttributes.empty() )
      {
        batch->Push( CopyAttributesFrom( srcAttributes ) );
      }

      return true;
    }
    else
    {
      return __super::HandleClipboardData( data, op, batch );
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Handles copying settings from another asset class, into this one.
// 
Undo::CommandPtr AssetClass::CopyFrom( Luna::PersistentData* src )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();
  if ( src->HasType( Reflect::GetType< Luna::AssetClass >() ) )
  {
    // Note that we are cloning the source data here.  This way we can manipulate
    // its members without changing the actual data source.
    Asset::AssetClassPtr srcAsset = Reflect::AssertCast< Asset::AssetClass >( src->GetPackage< Asset::AssetClass >()->Clone() );
    Luna::AssetClassPtr newSource = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AssetClass >( srcAsset, m_AssetManager );
    Asset::AssetClass* dstAsset = GetPackage< Asset::AssetClass >();

    // Keep around a copy of the source attributes so that they can be cleared
    // from the source.  This prevents them from clobbering the attributes
    // that already exist on this asset (which are wrapped in Attributes and 
    // need to be maintained).
    Attribute::M_Attribute srcAttribs = srcAsset->GetAttributes();
    srcAsset->Clear();

    // Replace any other data on our cloned source that should not be overwritten.
    {
      srcAsset->m_Description = dstAsset->m_Description;
    }

    // Perform the copy of the asset class
    batch->Push( __super::CopyFrom( newSource ) ); 

    // Perform the copy of each attribute
    batch->Push( CopyAttributesFrom( srcAttribs ) );
  }
  else
  {
    // Trying to copy a non-asset onto an asset.  Let the base class deal with that.
    batch->Push( __super::CopyFrom( src ) );
  }

  // Return the batch, or NULL if the batch is empty
  if ( !batch->IsEmpty() )
  {
    return batch;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to deal with copying attributes into this asset from the
// given source.
// 
Undo::CommandPtr AssetClass::CopyAttributesFrom( const Attribute::M_Attribute& srcAttribs )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  // Copy data from each source attribute into the attribute that occupies the same
  // slot on this class.
  Attribute::M_Attribute leftOvers = srcAttribs;
  Attribute::M_Attribute::const_iterator srcAttrItr = srcAttribs.begin();
  Attribute::M_Attribute::const_iterator srcAttrEnd = srcAttribs.end();
  for ( ; srcAttrItr != srcAttrEnd; ++srcAttrItr )
  {
    i32 slot = srcAttrItr->first;
    Attribute::AttributeBase* srcPkg = srcAttrItr->second;
    Luna::AttributeWrapper* destAttr = FindAttribute( slot );
    if ( destAttr )
    {
      batch->Push( destAttr->CopyFrom( PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( srcPkg, m_AssetManager ) ) );
      leftOvers.erase( slot );
    }
  }

  // If there are any attributes left in the source list, try adding them to this asset.
  if ( !leftOvers.empty() )
  {
    Asset::AssetClass* pkg = GetPackage< Asset::AssetClass >();
    Attribute::M_Attribute::const_iterator leftOverItr = leftOvers.begin();
    Attribute::M_Attribute::const_iterator leftOverEnd = leftOvers.end();
    for ( ; leftOverItr != leftOverEnd; ++leftOverItr )
    {
      // If the attribute can be added to this asset, perform the command.
      std::string unused;
      if ( pkg->ValidateAttribute( leftOverItr->second, unused ) )
      {
        Luna::AttributeWrapperPtr srcAttrib = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AttributeWrapper >( leftOverItr->second, m_AssetManager );
        batch->Push( new AttributeExistenceCommand( Undo::ExistenceActions::Add, this, srcAttrib ) );
      }
    }
  }

  if ( !batch->IsEmpty() )
  {
    return batch;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified attribute and returns true if successful.  On success,
// interested listeners will be notified.
// 
bool AssetClass::AddAttribute( const Luna::AttributeWrapperPtr& attribute )
{
  if ( AddAttribute( attribute, true ) )
  {
    m_AttributeAdded.Raise( AttributeExistenceArgs( this, attribute.Ptr() ) );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified attribute and returns true if successful.  On success,
// interested listeners will be notified.
// 
bool AssetClass::RemoveAttribute( const Luna::AttributeWrapperPtr& attribute )
{
  Luna::AttributeWrapperPtr preventDeletion = attribute;
  if ( RemoveAttribute( attribute, true ) )
  {
    m_AttributeRemoved.Raise( AttributeExistenceArgs( this, attribute.Ptr() ) );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to add a new attribute to this asset class.  The updatePackage
// flag indicates whether the change needs to be pushed down into the persistent
// data or not.
// 
bool AssetClass::AddAttribute( const Luna::AttributeWrapperPtr& attribute, bool updatePackage )
{
  if ( updatePackage )
  {
    Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      package->SetAttribute( attribute->GetPackage< Attribute::AttributeBase >() );
    }
    catch ( const Asset::Exception& e )
    {
      Console::Error( "%s\n", e.what() );
      return false;
    }
  }

  Nocturnal::Insert<M_AttributeSmartPtr>::Result inserted = m_Attributes.insert( M_AttributeSmartPtr::value_type( attribute->GetSlot(), attribute ) );
  if ( !inserted.second )
  {
    throw Nocturnal::Exception( "Attempted to add the same attribute (%s) twice to asset (%s)", attribute->GetName().c_str(), GetName().c_str() );
  }

  attribute->SetAssetClass( this );
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to remove the specified attribute from this asset.  You can 
// specify whether or not to update the persistent data.
// 
bool AssetClass::RemoveAttribute( const Luna::AttributeWrapperPtr& attribute, bool updatePackage )
{
  M_AttributeSmartPtr::iterator found = m_Attributes.find( attribute->GetSlot() );
  if ( found == m_Attributes.end() )
  {
    Console::Error( "Unable to remove attribute %s from asset %s because it has already been removed.\n", attribute->GetName().c_str(), GetName().c_str() );
    return false;
  }

  if ( updatePackage )
  {
    Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      package->RemoveAttribute( attribute->GetSlot() );
    }
    catch ( const Nocturnal::Exception& )
    {
      Console::Error( "Unable to update persistent data for removal of attribute %s from asset %s.\n", attribute->GetName().c_str(), GetName().c_str() );
      return false;
    }
  }

  found->second->SetAssetClass( NULL );
  m_Attributes.erase( found );
  return true;
}

#include "Precompile.h"
#include "AssetReferenceNode.h"

#include "AddAssetNodeCommand.h"
#include "AssetDocument.h"
#include "AssetEditor.h"
#include "AssetManager.h"
#include "AssetPreferences.h"
#include "AttributeContainer.h"
#include "AttributeNode.h"
#include "ContextMenuCallbacks.h"
#include "FieldFactory.h"
#include "RemoveAssetNodeCommand.h"

#include "File/Manager.h"
#include "File/ManagedFileDialog.h"
#include "FileBrowser/FileBrowser.h"
#include "FileSystem/FileSystem.h"
#include "Finder/Finder.h"
#include "Editor/SessionManager.h"
#include "Editor/RefreshSelectionCommand.h"
#include "Core/Enumerator.h"
#include "UIToolKit/ImageManager.h"


using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AssetReferenceNode );

///////////////////////////////////////////////////////////////////////////////
// Static type initialization.
// 
void AssetReferenceNode::InitializeType()
{
  Reflect::RegisterClass<Luna::AssetReferenceNode>( "Luna::AssetReferenceNode" );
  Enumerator::InitializePanel( "Asset", CreatePanelSignature::Delegate( &Luna::AssetReferenceNode::CreatePanel ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static type cleanup.
// 
void AssetReferenceNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::AssetReferenceNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetReferenceNode::AssetReferenceNode( Luna::AssetManager* manager, const tuid& assetClassID, const Reflect::Field* field )
: Luna::AssetNode( manager )
, m_Asset( NULL )
, m_AssetID( assetClassID )
, m_Element( NULL )
, m_Field( field )
, m_AttributeContainer( NULL )
{
  SetName( MakeLabel() );
  SetIcon( MakeIcon() );
  SetStyle( LabelStyles::Bold );

  // Add listeners
  GetAssetManager()->AddAssetUnloadingListener( AssetLoadSignature::Delegate ( this, &AssetReferenceNode::AssetUnloading ) );

  GetAssetEditorPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &AssetReferenceNode::PreferenceChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetReferenceNode::~AssetReferenceNode()
{
  // Remove listeners
  GetAssetManager()->RemoveAssetUnloadingListener( AssetLoadSignature::Delegate ( this, &AssetReferenceNode::AssetUnloading ) );

  GetAssetEditorPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &AssetReferenceNode::PreferenceChanged ) );

  if ( IsFieldAssociated() )
  {
    m_Element->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate ( this, &AssetReferenceNode::OnElementChanged ) );
  }

  if ( m_Asset )
  {
    AssetDocument* doc = GetAssetManager()->FindAssetDocument( m_Asset );
    doc->AddDocumentModifiedListener( DocumentChangedSignature::Delegate ( this, &AssetReferenceNode::DocumentModified ) );
    m_Asset->UnregisterAssetReferenceNode( this );
    m_Asset->GetPackage< Asset::AssetClass >()->RemoveEngineTypeChangedListener( Asset::EngineTypeChangeSignature::Delegate( this, &AssetReferenceNode::EngineTypeChanged ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the asset class that this node references (if it is loaded). If the 
// asset is not loaded, the base class is called.
// 
Luna::AssetClass* AssetReferenceNode::GetAssetClass() const
{
  if ( !m_Asset )
  {
    return __super::GetAssetClass();
  }

  return m_Asset;
}

///////////////////////////////////////////////////////////////////////////////
// Only call this function once the asset class referenced by this node is loaded.
// Creates all the appropriate child nodes for this reference (fields, attributes,
// etc.).
// 
void AssetReferenceNode::CreateChildren()
{
  if ( m_Asset )
  {
    // Fields on the asset class
    Luna::FieldFactory::GetInstance()->CreateChildFieldNodes( this, m_Asset->GetPackage< Reflect::Element >(), GetAssetManager() );

    m_AttributeContainer = new Luna::AttributeContainer( GetAssetManager(), m_Asset );
    AddChild( m_AttributeContainer );

    // Child for each attribute
    M_AttributeSmartPtr::const_iterator attributeItr = m_Asset->GetAttributes().begin();
    M_AttributeSmartPtr::const_iterator attributeEnd = m_Asset->GetAttributes().end();
    for ( ; attributeItr != attributeEnd; ++attributeItr )
    {
      const AttributeWrapperPtr& attribute = attributeItr->second;
      Luna::AttributeNodePtr attributeNode = new Luna::AttributeNode( attribute );
      m_AttributeContainer->AddChild( attributeNode );
      attributeNode->CreateChildren();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the this node is activated (double-clicked).  Loads the
// associated asset class.
// 
void AssetReferenceNode::ActivateItem()
{
  // Should we prompt if the file is already open elsewhere in the tree?

  if ( m_AssetID == TUID::Null )
  {
    ContextMenuArgsPtr args = new ContextMenuArgs();
    OnChangePathFinder( args );
    if ( !args->GetBatch()->IsEmpty() )
    {
      GetAssetManager()->Push( args->GetBatch() );
    }
    return;
  }

  if ( !m_Asset )
  {
    Load();
    GetAssetManager()->GetSelection().Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Loads the associated asset class (if it's not already loaded).  Updates the
// UI elements and child nodes accordingly.
// 
void AssetReferenceNode::Load()
{
  if ( !m_Asset )
  {
    Luna::AssetClass* assetClass = GetAssetManager()->FindAsset( m_AssetID );
    if ( !assetClass )
    {
      std::string error;
      assetClass = GetAssetManager()->Open( File::GlobalManager().GetPath( m_AssetID ), error, false );
      if ( !error.empty() )
      {
        Console::Error( "%s\n", error.c_str() );
      }
    }

    m_Asset = assetClass;
    ClearCachedAssetClass();
    CreateChildren();

    if ( m_Asset )
    {
      // Add listeners
      AssetDocument* doc = GetAssetManager()->FindAssetDocument( m_Asset );
      doc->AddDocumentModifiedListener( DocumentChangedSignature::Delegate ( this, &AssetReferenceNode::DocumentModified ) );

      m_Asset->GetPackage< Asset::AssetClass >()->AddEngineTypeChangedListener( Asset::EngineTypeChangeSignature::Delegate( this, &AssetReferenceNode::EngineTypeChanged ) );
      m_Asset->RegisterAssetReferenceNode( this );

      SetName( MakeLabel() );
      SetIcon( MakeIcon() );
    }
    else
    {
      FilePathOptions::FilePathOption filePathOption = FilePathOptions::PartialPath;
      GetAssetEditorPreferences()->GetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );
      SetName( "Error loading: " + Luna::TuidToLabel( m_AssetID, filePathOption ) );
      SetIcon( "enginetype_unknown_16.png" );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Unloads the asset class from this particlar references (all child nodes are
// deleted).
// 
void AssetReferenceNode::Unload()
{
  if ( m_Asset )
  {
    // Remove listeners
    AssetDocument* doc = GetAssetManager()->FindAssetDocument( m_Asset );
    doc->RemoveDocumentModifiedListener( DocumentChangedSignature::Delegate ( this, &AssetReferenceNode::DocumentModified ) );

    m_Asset->GetPackage< Asset::AssetClass >()->RemoveEngineTypeChangedListener( Asset::EngineTypeChangeSignature::Delegate( this, &AssetReferenceNode::EngineTypeChanged ) );

    m_Asset->UnregisterAssetReferenceNode( this );

    DeleteChildren();
    m_Asset = NULL;
    SetName( MakeLabel() );
    SetIcon( MakeIcon() );
    SetStyle( LabelStyles::Bold );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the ID of the asset that this node references.
// 
tuid AssetReferenceNode::GetAssetID() const
{
  return m_AssetID;
}

///////////////////////////////////////////////////////////////////////////////
// Changes the asset that this node references.
// 
void AssetReferenceNode::SetAssetID( const tuid& newFileID )
{
  if ( m_AssetID != newFileID )
  {
    //  Update the serializer if necessary
    if ( IsFieldAssociated() )
    {
      // This will result in a callback (OnElementChanged) which will handle
      // the rest of the operation.
      Reflect::SerializerPtr serializer = m_Field->CreateSerializer();
      serializer->ConnectField( m_Element, m_Field );
      std::stringstream stream;
      stream << TUID::HexFormat << newFileID;
      stream >> *serializer;
    }
    else
    {
      // There's no field associated, so just update our internal pointer and UI.
      SetAssetIDHelper( newFileID );
    }

    GetAssetManager()->GetSelection().Refresh();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Associates a field with this asset reference.  This has implications with
// how the UI for this node is displayed and what operations are allowed.
// 
void AssetReferenceNode::AssociateField( Reflect::Element* element, const Reflect::Field* field )
{
  // Sanity checks
  NOC_ASSERT( element );
  NOC_ASSERT( field );
  NOC_ASSERT( field->m_Flags & Reflect::FieldFlags::FileID );

  // Don't try to associate a reference that's already associated!
  if ( IsFieldAssociated() )
  {
    throw Nocturnal::Exception( "Attempted to add field (%s) to reference '%s' when there is already a field associated", field->m_UIName.c_str(), GetName().c_str() );
  }

  // Store the values
  m_Element = element;
  m_Field = field;

  // Update the label
  SetName( MakeLabel() );

  // Listeners
  m_Element->AddChangedListener( Reflect::ElementChangeSignature::Delegate ( this, &AssetReferenceNode::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if there is a field associated with this node.
// 
bool AssetReferenceNode::IsFieldAssociated() const
{
  return m_Element && m_Field;
}

///////////////////////////////////////////////////////////////////////////////
// Update the property panel UI.
// 
void AssetReferenceNode::ConnectProperties( EnumerateElementArgs& args )
{
  if ( m_Asset )
  {
    args.EnumerateElement( m_Asset->GetPackage< Reflect::Element >() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Enables the static "Asset" panel for asset classes.
// 
bool AssetReferenceNode::ValidatePanel( const std::string& name )
{
  if ( name == "Asset" )
  {
    return true;
  }

  if ( m_Asset )
  {
    return m_Asset->ValidatePanel( name );
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Called right before the context menu is shown.  Rebuilds the context menu
// since many of the items are determined by current selection and what attributes
// are present on the given asset.
// 
void AssetReferenceNode::PreShowContextMenu()
{
  ContextMenuItemSet& contextMenu = GetContextMenu();
  contextMenu.Clear();

  if ( m_Asset )
  {
    m_Asset->PopulateContextMenu( contextMenu );
  }
  else
  {
    const size_t numSelected = GetAssetManager()->GetSelection().GetItems().Size();

    ContextMenuItemPtr menuItem = new ContextMenuItem( "Edit", "Open file in the Asset Editor." );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AssetReferenceNode::OnOpen ) );
    contextMenu.AppendItem( menuItem );

    SubMenuPtr rcsSubMenu = new SubMenu( "Perforce" );
    
    menuItem = new ContextMenuItem( "Check Out" );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AssetReferenceNode::OnCheckOutPath ) );
    rcsSubMenu->AppendItem( menuItem );

    menuItem = new ContextMenuItem( "Revision History" );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AssetReferenceNode::OnRevisionHistory ) );
    rcsSubMenu->AppendItem( menuItem );

    contextMenu.AppendItem( rcsSubMenu );

    if ( IsFieldAssociated() )
    {
      contextMenu.AppendSeparator();
      menuItem = new ContextMenuItem( "Change File Path", "Change this file's path using the Open File dialog" );
      menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AssetReferenceNode::OnChangePath ) );
      menuItem->Enable( numSelected == 1 );
      contextMenu.AppendItem( menuItem );

    
      wxBitmap finderIcon = UIToolKit::GlobalImageManager().GetBitmap( "magnify_16.png" );
      menuItem = new ContextMenuItem( "Change File Path (Asset Finder)", "Change this file's path using the Asset Finder", finderIcon );
      menuItem->AddCallback( ContextMenuSignature::Delegate( this, &AssetReferenceNode::OnChangePathFinder ) );
      menuItem->Enable( numSelected == 1 );
      contextMenu.AppendItem( menuItem );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Assets can be copied.
// 
bool AssetReferenceNode::CanBeCopied() const
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Pass through function to copy the Asset Class data.
// 
Inspect::ReflectClipboardDataPtr AssetReferenceNode::GetClipboardData()
{
  Inspect::ReflectClipboardDataPtr data;
  if ( m_Asset )
  {
    data = m_Asset->GetClipboardData();
  }
  // TODO: store file path as clipboard data if asset isn't loaded

  return data;
}

///////////////////////////////////////////////////////////////////////////////
// Pass trhough to determine if the clipboard data is valid to be dropped/pasted
// onto this node.
// 
bool AssetReferenceNode::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  bool isValid = false;
  if ( m_Asset )
  {
    isValid = m_Asset->CanHandleClipboardData( data );
  }
  return isValid;
}

///////////////////////////////////////////////////////////////////////////////
// Pass through function to paste clipboard data onto an Asset Class.
// 
bool AssetReferenceNode::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  bool handled = false;
  if ( m_Asset )
  {
    handled = m_Asset->HandleClipboardData( data, op, batch );
  }
  return handled;
}

///////////////////////////////////////////////////////////////////////////////
// Updates the cached asset ID and the UI associated with this item.
// 
void AssetReferenceNode::SetAssetIDHelper( const tuid& newFileID )
{
  if ( m_AssetID != newFileID )
  {
    Unload();

    // Update our internal data
    m_AssetID = newFileID;

    // Update UI
    SetName( MakeLabel() );
    SetIcon( MakeIcon() );
    SetStyle( LabelStyles::Bold );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Finds the child Luna::AttributeNode that corresponds to the specified Luna::AttributeWrapper.
// 
Luna::AttributeNode* AssetReferenceNode::FindAttributeNode( Luna::AttributeWrapper* attribute )
{
  if ( m_AttributeContainer )
  {
    M_AttributeNodeDumbPtr::const_iterator found = m_AttributeContainer->GetAttributes().find( attribute->GetSlot() );
    if ( found != m_AttributeContainer->GetAttributes().end() )
    {
      return found->second;
    }
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for adding the specified Luna::AttributeWrapper to this reference (will
// result in the creation of a new Luna::AttributeNode).  The command will not have
// been executed, so you must call Redo() on it to actually perform the operation.
// 
Undo::CommandPtr AssetReferenceNode::GetAddAttributeCommand( Luna::AttributeWrapper* attribute )
{
  Luna::AttributeNodePtr node = new Luna::AttributeNode( attribute );
  node->CreateChildren();
  return new Luna::AddAssetNodeCommand( m_AttributeContainer, node, NULL, false );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a command for removing the specified Luna::AttributeWrapper from this reference
// (will remove the corresponding Luna::AttributeNode from this reference).  The 
// command will not have been executed, so you must call Redo() on it to actually
// perform the operation.
// 
Undo::CommandPtr AssetReferenceNode::GetRemoveAttributeCommand( Luna::AttributeWrapper* attribute )
{
  Luna::AttributeNode* node = FindAttributeNode( attribute );
  NOC_ASSERT( node );
  return new Luna::RemoveAssetNodeCommand( node, false );
}

///////////////////////////////////////////////////////////////////////////////
// Creates a static panel for this class type in the property window.
// 
void AssetReferenceNode::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel( "Asset", true );
  {
    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "Name" );
      typedef std::string ( Luna::AssetReferenceNode::*Getter )() const;
      typedef void ( Luna::AssetReferenceNode::*Setter )( const std::string& );
      Inspect::Value* textBox = args.m_Enumerator->AddValue< Luna::AssetReferenceNode, std::string, Getter, Setter >( args.m_Selection, &Luna::AssetReferenceNode::GetFileName );
      textBox->SetReadOnly( true );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "Engine Type" );
      typedef std::string ( Luna::AssetReferenceNode::*Getter )() const;
      typedef void ( Luna::AssetReferenceNode::*Setter )( const std::string& );
      Inspect::Value* textBox = args.m_Enumerator->AddValue< Luna::AssetReferenceNode, std::string, Getter, Setter >( args.m_Selection, &Luna::AssetReferenceNode::GetEngineTypeName );
      textBox->SetReadOnly( true );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "Asset ID" );
      typedef std::string ( Luna::AssetReferenceNode::*Getter )() const;
      typedef void ( Luna::AssetReferenceNode::*Setter )( const std::string& );
      Inspect::Value* textBox = args.m_Enumerator->AddValue< Luna::AssetReferenceNode, std::string, Getter, Setter >( args.m_Selection, &Luna::AssetReferenceNode::GetFileIDString );
      textBox->SetReadOnly( true );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel( "File" );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      typedef std::string ( Luna::AssetReferenceNode::*Getter )() const;
      typedef void ( Luna::AssetReferenceNode::*Setter )( const std::string& );
      Inspect::Value* textBox = args.m_Enumerator->AddValue< Luna::AssetReferenceNode, std::string, Getter, Setter >( args.m_Selection, &Luna::AssetReferenceNode::GetFilePath );
      textBox->SetJustification( Inspect::Value::kRight );
      textBox->SetReadOnly( true );
    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}

///////////////////////////////////////////////////////////////////////////////
// Returns a string displaying the engine type for this asset.
// 
std::string AssetReferenceNode::GetEngineTypeName() const
{
  if ( m_Asset )
  {
    return m_Asset->GetPackage< Asset::AssetClass >()->GetEngineTypeName();
  }

  return "Unknown";
}

///////////////////////////////////////////////////////////////////////////////
// Returns the file name of the ID associated with this node.
// 
std::string AssetReferenceNode::GetFileName() const
{
  std::string fileName = GetFilePath();
  if ( !fileName.empty() )
  {
    fileName = FileSystem::GetLeaf( fileName );
    FileSystem::StripExtension( fileName );
  }
  return fileName;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the full file path associated with this asset ID.
// 
std::string AssetReferenceNode::GetFilePath() const
{
  std::string filePath;
  try
  {
    filePath = File::GlobalManager().GetPath( m_AssetID );
  }
  catch ( const File::Exception& )
  {
  }

  return filePath;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the file ID for this asset as a string (hex) representation.
// 
std::string AssetReferenceNode::GetFileIDString() const
{
  std::stringstream stream;
  stream << TUID::HexFormat << m_AssetID;
  return stream.str();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the label to use on this node.
// 
std::string AssetReferenceNode::MakeLabel() const
{
  std::string label;
  if ( IsFieldAssociated() )
  {
    if ( m_Field->m_SerializerID == Reflect::GetType< Reflect::U64Serializer >() )
    {
      label += m_Field->m_UIName + ": ";
    }
  }

  if ( m_AssetID != TUID::Null )
  {
    FilePathOptions::FilePathOption filePathOption = FilePathOptions::PartialPath;
    GetAssetEditorPreferences()->GetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );
    label += Luna::TuidToLabel( m_AssetID, filePathOption );
  }
  else
  {
    label += "(double-click to set)";
  }

  if ( !m_Asset && m_AssetID != TUID::Null )
  {
    label += " (double-click to load)";
  }

  return label;
}

///////////////////////////////////////////////////////////////////////////////
// Gets the appropriate icon to use for this node.
// 
std::string AssetReferenceNode::MakeIcon() const
{
  std::string icon;
  
  if ( m_Field )
  {
    icon = m_Field->GetProperty( Asset::AssetProperties::SmallIcon );
  }

  if ( icon.empty() )
  {
    if ( m_Asset )
    {
      icon = m_Asset->GetIcon();
    }
    else
    {
      icon = "asset_reference_16.png";
    }
  }
  return icon;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an asset is unloaded.  Checks to see if the asset that is
// being unloaded is the one referenced by this node and updates its UI 
// accordingly.
// 
void AssetReferenceNode::AssetUnloading( const AssetLoadArgs& args )
{
  if ( args.m_AssetClass->GetFileID() == m_AssetID )
  {
    Unload();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user changes the preference related to how file paths
// are displayed. Updates the name of this node accordingly.
// 
void AssetReferenceNode::PreferenceChanged( const Reflect::ElementChangeArgs& args )
{
  if ( args.m_Element && args.m_Field )
  {
    if ( args.m_Field == GetAssetEditorPreferences()->FilePathOption() )
    {
      SetName( MakeLabel() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the modified state on an asset class is changed.  Updates
// the font style on this item.
// 
void AssetReferenceNode::DocumentModified( const DocumentChangedArgs& args )
{
  if ( m_Asset )
  {
    if ( args.m_Document->IsModified() )
    {
      SetStyle( GetStyle() | LabelStyles::Italic );
    }
    else
    {
      SetStyle( GetStyle() & ~LabelStyles::Italic );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for opening this reference node.
// 
void AssetReferenceNode::OnOpen( const ContextMenuArgsPtr& args )
{
  Load();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the element associated with this node is changed.  Updates
// the file ID if necessary.
// 
void AssetReferenceNode::OnElementChanged( const Reflect::ElementChangeArgs& args )
{
  if ( IsFieldAssociated() )
  {
    if ( args.m_Field == m_Field )
    {
      tuid newFileID = TUID::Null;
      Reflect::SerializerPtr serializer = m_Field->CreateSerializer();
      serializer->ConnectField( m_Element, m_Field );
      std::stringstream stream;
      stream << *serializer;
      TUID::Parse( stream.str(), newFileID );

      SetAssetIDHelper( newFileID );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the engine type changes.  Updates the UI (icon and property
// panel).
// 
void AssetReferenceNode::EngineTypeChanged( const Asset::EngineTypeChangeArgs& args )
{
  // Fix icon
  SetIcon( MakeIcon() );

  // Refresh the selection so that the Engine Type field updates as well.
  // We have to wait and do this on the next frame.
  GetAssetManager()->GetAssetEditor()->PostCommand( new RefreshSelectionCommand( &GetAssetManager()->GetSelection() ) );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback that displays a normal File Open dialog for choosing
// a new asset for this node to reference.
// 
void AssetReferenceNode::OnChangePath( const ContextMenuArgsPtr& args )
{
  File::ManagedFileDialog dialog( GetAssetManager()->GetAssetEditor(), "Change File Path", Finder::ProjectAssets().c_str() );
  dialog.SetTuidRequired( true );

  std::string currentPath = GetFilePath();
  if ( !currentPath.empty() )
  {
    dialog.SetPath( currentPath.c_str() );
  }
  else if ( m_AssetID != TUID::Null )
  {
    // If the TUID did not resolve to a path, the user is probably trying to repair 
    // this path and just needs to add a file to the resolver with this TUID, 
    // so set the requested file ID on the the dialog.
    dialog.SetRequestedFileID( m_AssetID );
  }

  std::string specName;
  if ( m_Field->GetProperty( Asset::AssetProperties::FilterSpec, specName ) )
  {
    const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );

    dialog.SetFilter( spec->GetDialogFilter() );
  }
  else if ( m_Field->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
  {
    const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );

    dialog.SetFilter( spec->GetDialogFilter() );
  }

  if ( dialog.ShowModal() == wxID_OK )
  {
    if ( m_AssetID != dialog.GetFileID() )
    {
      args->GetBatch()->Push( new Undo::PropertyCommand< tuid >( new Nocturnal::MemberProperty< Luna::AssetReferenceNode, tuid >( this, &AssetReferenceNode::GetAssetID, &AssetReferenceNode::SetAssetID ), dialog.GetFileID() ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for displaying the Asset Finder so that a new asset
// can be referenced by this node.
// 
void AssetReferenceNode::OnChangePathFinder( const ContextMenuArgsPtr& args )
{
  File::FileBrowser dialog( GetAssetManager()->GetAssetEditor(), wxID_ANY, "Change File Path" );

  std::string specName;
  if ( m_Field->GetProperty( Asset::AssetProperties::FilterSpec, specName ) )
  {
    const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );
    dialog.SetFilter( *spec );
  }
  else if ( m_Field->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
  {
    const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );
    dialog.SetFilter( *spec );
  }
  dialog.SetTuidRequired( true );

  std::string currentPath = GetFilePath();
  if ( currentPath.empty() && m_AssetID != TUID::Null )
  {
    // If the TUID did not resolve to a path, the user is probably trying to repair 
    // this path and just needs to add a file to the resolver with this TUID, 
    // so set the requested file ID on the the dialog.
    dialog.SetRequestedFileID( m_AssetID );
  }

  if ( dialog.ShowModal() == wxID_OK )
  {
    if ( m_AssetID != dialog.GetFileID() )
    {
      args->GetBatch()->Push( new Undo::PropertyCommand< tuid >( new Nocturnal::MemberProperty< Luna::AssetReferenceNode, tuid >( this, &AssetReferenceNode::GetAssetID, &AssetReferenceNode::SetAssetID ), dialog.GetFileID() ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for checking out the asset file referenced by this node.
// 
void AssetReferenceNode::OnCheckOutPath( const ContextMenuArgsPtr& args )
{
  SessionManager::GetInstance()->CheckOut( GetFilePath() );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for displaying the revision history on this file.
// 
void AssetReferenceNode::OnRevisionHistory( const ContextMenuArgsPtr& args )
{
  const std::string& path = GetFilePath();
  if ( !path.empty() )
  {
    GetAssetManager()->GetAssetEditor()->RevisionHistory( path );
  }
}

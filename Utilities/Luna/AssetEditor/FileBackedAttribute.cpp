#include "Precompile.h"
#include "FileBackedAttribute.h"

#include "AssetEditor.h"
#include "AssetManager.h"
#include "AssetPreferences.h"
#include "PersistentDataFactory.h"

#include "File/ManagedFileDialog.h"
#include "FileBrowser/FileBrowser.h"
#include "Finder/Finder.h"
#include "Editor/ContextMenuGenerator.h"
#include "Editor/SessionManager.h"
#include "UIToolKit/ImageManager.h"


// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::FileBackedAttribute );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::FileBackedAttribute types.
// 
void FileBackedAttribute::InitializeType()
{
  Reflect::RegisterClass<Luna::FileBackedAttribute>( "Luna::FileBackedAttribute" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::FileBackedAttribute >(), &Luna::FileBackedAttribute::CreateFileBackedAttribute );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::FileBackedAttribute types.
// 
void FileBackedAttribute::CleanupType()
{
  Reflect::UnregisterClass<Luna::FileBackedAttribute>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr FileBackedAttribute::CreateFileBackedAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
  return new Luna::FileBackedAttribute( Reflect::AssertCast< Asset::FileBackedAttribute >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileBackedAttribute::FileBackedAttribute( Asset::FileBackedAttribute* attribute, Luna::AssetManager* assetManager )
: Luna::AttributeWrapper( attribute, assetManager )
{
  // Add listeners
  GetAssetEditorPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedAttribute::OnPreferenceChanged ) );

  attribute->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedAttribute::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FileBackedAttribute::~FileBackedAttribute()
{
  // Remove listeners
  GetAssetEditorPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedAttribute::OnPreferenceChanged ) );

  GetPackage< Reflect::Element >()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedAttribute::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this attribute.
// 
std::string FileBackedAttribute::GetName() const
{
  const Asset::FileBackedAttribute* package = GetPackage< Asset::FileBackedAttribute >();
  std::string name = package->GetClass()->m_UIName;

  FilePathOptions::FilePathOption filePathOption = FilePathOptions::PartialPath;
  GetAssetEditorPreferences()->GetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );

  name += ": " + Luna::TuidToLabel( package->GetFileID(), filePathOption );
  return name;
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the context menu with items appropriate for file backed attributes.
// 
void FileBackedAttribute::PopulateContextMenu( ContextMenuItemSet& menu )
{
  // TODO: Make this work with multiple selection
  const size_t numSelected = GetAssetManager()->GetSelection().GetItems().Size();

  menu.AppendSeparator();
  ContextMenuItemPtr menuItem = new ContextMenuItem( "Edit", "Open file in the appropriate editor." );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedAttribute::OnOpen ) );
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  SubMenuPtr rcsSubMenu = new SubMenu( "Perforce" );

  menuItem = new ContextMenuItem( "Check Out" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedAttribute::OnCheckOutPath ) );
  rcsSubMenu->AppendItem( menuItem );

  menuItem = new ContextMenuItem( "Revision History" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedAttribute::OnRevisionHistoryPath ) );
  rcsSubMenu->AppendItem( menuItem );

  menu.AppendItem( rcsSubMenu );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( "Change File Path", "Change this file's path using the Open File dialog" );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedAttribute::OnChangePath ) );
  menuItem->Enable( numSelected == 1 );
  menu.AppendItem( menuItem );


  wxBitmap finderIcon = UIToolKit::GlobalImageManager().GetBitmap( "magnify_16.png" );
  menuItem = new ContextMenuItem( "Change File Path (Asset Finder)", "Change this file's path using the Asset Finder", finderIcon );
  menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedAttribute::OnChangePathFinder ) );
  menuItem->Enable( numSelected == 1 );
  menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the file ID of this attribute.
// 
tuid FileBackedAttribute::GetFileID() const
{
  const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
  return pkg->GetFileID();
}

///////////////////////////////////////////////////////////////////////////////
// Sets the file ID on this attribute.
// 
void FileBackedAttribute::SetFileID( const tuid& fileID )
{
  Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
  pkg->SetFileID( fileID );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the string path representation of the file backing this node.
// 
std::string FileBackedAttribute::GetFilePath() const
{
  const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
  std::string filePath;

  try
  {
    filePath = pkg->GetFilePath();
  }
  catch ( const Nocturnal::Exception& )
  {
  }

  return filePath;
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to update the name on any UI displaying this attribute.
// 
void FileBackedAttribute::OnElementChanged( const Reflect::ElementChangeArgs& args )
{
  // Convert element changed event into a name change event (might be a bit
  // heavy-handed since a change on the element does not always indciate that
  // the name needs to be udpated).
  m_NameChanged.Raise( AttributeChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user preference about file paths is changed.  Converts
// this event into an "attribute name change" event that way the associated
// asset node can update.
// 
void FileBackedAttribute::OnPreferenceChanged( const Reflect::ElementChangeArgs& args )
{
  if ( args.m_Element && args.m_Field )
  {
    if ( args.m_Field == GetAssetEditorPreferences()->FilePathOption() )
    {
      m_NameChanged.Raise( AttributeChangeArgs( this ) );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses to open this item.
// 
void FileBackedAttribute::OnOpen( const ContextMenuArgsPtr& args )
{
#pragma TODO( "Might need to do something different for references to Assets (open inline)" )
  SessionManager::GetInstance()->Edit( GetFilePath() );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback that displays a normal File Open dialog for choosing
// a new file for this node to reference.
// 
void FileBackedAttribute::OnChangePath( const ContextMenuArgsPtr& args )
{
  File::ManagedFileDialog dialog( GetAssetManager()->GetAssetEditor(), "Change File Path", Finder::ProjectAssets().c_str() );
  dialog.SetTuidRequired( true );

  tuid currentFileID = GetFileID();
  std::string currentPath = GetFilePath();
  if ( !currentPath.empty() )
  {
    dialog.SetPath( currentPath.c_str() );
  }
  else if ( currentFileID != TUID::Null )
  {
    // If the TUID did not resolve to a path, the user is probably trying to repair 
    // this path and just needs to add a file to the resolver with this TUID, 
    // so set the requested file ID on the the dialog.
    dialog.SetRequestedFileID( currentFileID );
  }

  const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
  const Finder::FinderSpec* spec = pkg->GetFileFilter();
  if ( spec )
  {
    dialog.SetFilter( spec->GetDialogFilter() );
  }

  if ( dialog.ShowModal() == wxID_OK )
  {
    if ( currentFileID != dialog.GetFileID() )
    {
      args->GetBatch()->Push( new Undo::PropertyCommand< tuid >( new Nocturnal::MemberProperty< Luna::FileBackedAttribute, tuid >( this, &Luna::FileBackedAttribute::GetFileID, &Luna::FileBackedAttribute::SetFileID ), dialog.GetFileID() ) );
      GetAssetManager()->GetSelection().Refresh();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for displaying the Asset Finder so that a new file
// can be referenced by this node.
// 
void FileBackedAttribute::OnChangePathFinder( const ContextMenuArgsPtr& args )
{
  File::FileBrowser dialog( GetAssetManager()->GetAssetEditor(), wxID_ANY, "Change File Path" );
  const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
  const Finder::FinderSpec* spec = pkg->GetFileFilter();
  if ( spec )
  {
    dialog.SetFilter( *spec );
  }
  dialog.SetTuidRequired( true );

  tuid currentFileID = GetFileID();
  std::string currentPath = GetFilePath();
  if ( currentPath.empty() && currentFileID != TUID::Null )
  {
    // If the TUID did not resolve to a path, the user is probably trying to repair 
    // this path and just needs to add a file to the resolver with this TUID, 
    // so set the requested file ID on the the dialog.
    dialog.SetRequestedFileID( currentFileID );
  }

  if ( dialog.ShowModal() == wxID_OK )
  {
    if ( currentFileID != dialog.GetFileID() )
    {
      args->GetBatch()->Push( new Undo::PropertyCommand< tuid >( new Nocturnal::MemberProperty< Luna::FileBackedAttribute, tuid >( this, &Luna::FileBackedAttribute::GetFileID, &Luna::FileBackedAttribute::SetFileID ), dialog.GetFileID() ) );
      GetAssetManager()->GetSelection().Refresh();
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for checking out the asset file referenced by this node.
// 
void FileBackedAttribute::OnCheckOutPath( const ContextMenuArgsPtr& args )
{
  SessionManager::GetInstance()->CheckOut( GetFilePath() );
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for showing the revision history for the file represented
// by this node.
// 
void FileBackedAttribute::OnRevisionHistoryPath( const ContextMenuArgsPtr& args )
{
  GetAssetManager()->GetAssetEditor()->RevisionHistory( GetFilePath() );
}


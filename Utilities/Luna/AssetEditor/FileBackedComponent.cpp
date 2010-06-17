#include "Precompile.h"
#include "FileBackedComponent.h"

#include "AssetEditor.h"
#include "AssetManager.h"
#include "AssetPreferences.h"
#include "PersistentDataFactory.h"

#include "Editor/ContextMenuGenerator.h"
#include "Finder/Finder.h"
#include "Application/UI/ImageManager.h"
#include "Application/UI/FileDialog.h"


// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::FileBackedComponent );

///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::FileBackedComponent types.
// 
void FileBackedComponent::InitializeType()
{
    Reflect::RegisterClass<Luna::FileBackedComponent>( "Luna::FileBackedComponent" );
    PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::FileBackedComponent >(), &Luna::FileBackedComponent::CreateFileBackedComponent );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::FileBackedComponent types.
// 
void FileBackedComponent::CleanupType()
{
    Reflect::UnregisterClass<Luna::FileBackedComponent>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function. 
// 
Luna::PersistentDataPtr FileBackedComponent::CreateFileBackedComponent( Reflect::Element* attribute, Luna::AssetManager* assetManager )
{
    return new Luna::FileBackedComponent( Reflect::AssertCast< Asset::FileBackedComponent >( attribute ), assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileBackedComponent::FileBackedComponent( Asset::FileBackedComponent* attribute, Luna::AssetManager* assetManager )
: Luna::ComponentWrapper( attribute, assetManager )
{
    // Add listeners
    GetAssetEditorPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedComponent::OnPreferenceChanged ) );

    attribute->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedComponent::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FileBackedComponent::~FileBackedComponent()
{
    // Remove listeners
    GetAssetEditorPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedComponent::OnPreferenceChanged ) );

    GetPackage< Reflect::Element >()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &FileBackedComponent::OnElementChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this attribute.
// 
std::string FileBackedComponent::GetName() const
{
    const Asset::FileBackedComponent* package = GetPackage< Asset::FileBackedComponent >();
    std::string name = package->GetClass()->m_UIName;

    FilePathOptions::FilePathOption filePathOption = FilePathOptions::PartialPath;
    GetAssetEditorPreferences()->GetEnum( GetAssetEditorPreferences()->FilePathOption(), filePathOption );

    name += ": " + Luna::PathToLabel( package->GetPath(), filePathOption );
    return name;
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the context menu with items appropriate for file backed attributes.
// 
void FileBackedComponent::PopulateContextMenu( ContextMenuItemSet& menu )
{
    // TODO: Make this work with multiple selection
    const size_t numSelected = GetAssetManager()->GetSelection().GetItems().Size();

    menu.AppendSeparator();
    ContextMenuItemPtr menuItem = new ContextMenuItem( "Edit", "Open file in the appropriate editor." );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedComponent::OnOpen ) );
    menu.AppendItem( menuItem );

    menu.AppendSeparator();
    SubMenuPtr rcsSubMenu = new SubMenu( "Perforce" );

    menuItem = new ContextMenuItem( "Check Out" );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedComponent::OnCheckOutPath ) );
    rcsSubMenu->AppendItem( menuItem );

    menuItem = new ContextMenuItem( "Revision History" );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedComponent::OnRevisionHistoryPath ) );
    rcsSubMenu->AppendItem( menuItem );

    menu.AppendItem( rcsSubMenu );

    menu.AppendSeparator();
    menuItem = new ContextMenuItem( "Change File Path", "Change this file's path using the Open File dialog" );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedComponent::OnChangePath ) );
    menuItem->Enable( numSelected == 1 );
    menu.AppendItem( menuItem );


    wxBitmap finderIcon = Nocturnal::GlobalImageManager().GetBitmap( "magnify_16.png" );
    menuItem = new ContextMenuItem( "Change File Path (Asset Finder)", "Change this file's path using the Asset Finder", finderIcon );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedComponent::OnChangePathFinder ) );
    menuItem->Enable( numSelected == 1 );
    menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the string path representation of the file backing this node.
// 
std::string FileBackedComponent::GetFilePath() const
{
    const Asset::FileBackedComponent* pkg = GetPackage< Asset::FileBackedComponent >();
    return pkg->GetPath().Get();
}

void FileBackedComponent::SetFilePath( const std::string& path )
{
    Asset::FileBackedComponent* pkg = GetPackage< Asset::FileBackedComponent >();
    Nocturnal::Path filePath( path );
    pkg->SetPath( filePath );
}

///////////////////////////////////////////////////////////////////////////////
// Overridden to update the name on any UI displaying this attribute.
// 
void FileBackedComponent::OnElementChanged( const Reflect::ElementChangeArgs& args )
{
    // Convert element changed event into a name change event (might be a bit
    // heavy-handed since a change on the element does not always indciate that
    // the name needs to be udpated).
    m_NameChanged.Raise( ComponentChangeArgs( this ) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user preference about file paths is changed.  Converts
// this event into an "attribute name change" event that way the associated
// asset node can update.
// 
void FileBackedComponent::OnPreferenceChanged( const Reflect::ElementChangeArgs& args )
{
    if ( args.m_Element && args.m_Field )
    {
        if ( args.m_Field == GetAssetEditorPreferences()->FilePathOption() )
        {
            m_NameChanged.Raise( ComponentChangeArgs( this ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user chooses to open this item.
// 
void FileBackedComponent::OnOpen( const ContextMenuArgsPtr& args )
{
#pragma TODO( "Open the file in the editor" )
    NOC_BREAK(); 
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback that displays a normal File Open dialog for choosing
// a new file for this node to reference.
// 
void FileBackedComponent::OnChangePath( const ContextMenuArgsPtr& args )
{
    Nocturnal::FileDialog dialog( GetAssetManager()->GetAssetEditor(), "Change File Path" );

    std::string currentPath = GetFilePath();
    if ( !currentPath.empty() )
    {
        dialog.SetPath( currentPath.c_str() );
    }

    const Asset::FileBackedComponent* pkg = GetPackage< Asset::FileBackedComponent >();
    const Finder::FinderSpec* spec = pkg->GetFileFilter();
    if ( spec )
    {
        dialog.SetFilter( spec->GetDialogFilter() );
    }

    if ( dialog.ShowModal() == wxID_OK )
    {
        args->GetBatch()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::FileBackedComponent, std::string >( this, &Luna::FileBackedComponent::GetFilePath, &Luna::FileBackedComponent::SetFilePath ), dialog.GetFilePath() ) );
        GetAssetManager()->GetSelection().Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for displaying the Asset Finder so that a new file
// can be referenced by this node.
// 
void FileBackedComponent::OnChangePathFinder( const ContextMenuArgsPtr& args )
{
    NOC_BREAK();
#pragma TODO( "Reimplement to use the Vault" )
    //File::FileBrowser dialog( GetAssetManager()->GetAssetEditor(), wxID_ANY, "Change File Path" );
    //const Asset::FileBackedComponent* pkg = GetPackage< Asset::FileBackedComponent >();
    //const Finder::FinderSpec* spec = pkg->GetFileFilter();
    //if ( spec )
    //{
    //    dialog.SetFilter( *spec );
    //}

    //std::string currentPath = GetFilePath();

    //if ( dialog.ShowModal() == wxID_OK )
    //{
    //    if ( currentPath != dialog.GetPath() )
    //    {
    //        args->GetBatch()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::FileBackedComponent, std::string >( this, &Luna::FileBackedComponent::GetFilePath, &Luna::FileBackedComponent::SetFilePath ), dialog.GetPath() ) );
    //        GetAssetManager()->GetSelection().Refresh();
    //    }
    //}
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for checking out the asset file referenced by this node.
// 
void FileBackedComponent::OnCheckOutPath( const ContextMenuArgsPtr& args )
{
#pragma TODO( "Make this check out the file" )
NOC_BREAK();
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for showing the revision history for the file represented
// by this node.
// 
void FileBackedComponent::OnRevisionHistoryPath( const ContextMenuArgsPtr& args )
{
    GetAssetManager()->GetAssetEditor()->RevisionHistory( GetFilePath() );
}


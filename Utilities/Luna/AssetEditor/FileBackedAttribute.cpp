#include "Precompile.h"
#include "FileBackedAttribute.h"

#include "AssetEditor.h"
#include "AssetManager.h"
#include "AssetPreferences.h"
#include "PersistentDataFactory.h"

#include "Editor/ContextMenuGenerator.h"
#include "Finder/Finder.h"
#include "Luna/UI/ImageManager.h"
#include "Luna/UI/FileDialog.h"


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

    name += ": " + Luna::PathToLabel( package->GetPath(), filePathOption );
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


    wxBitmap finderIcon = Luna::GlobalImageManager().GetBitmap( "magnify_16.png" );
    menuItem = new ContextMenuItem( "Change File Path (Asset Finder)", "Change this file's path using the Asset Finder", finderIcon );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileBackedAttribute::OnChangePathFinder ) );
    menuItem->Enable( numSelected == 1 );
    menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the string path representation of the file backing this node.
// 
std::string FileBackedAttribute::GetFilePath() const
{
    const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
    return pkg->GetPath().Get();
}

void FileBackedAttribute::SetFilePath( const std::string& path )
{
    Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
    Nocturnal::Path filePath( path );
    pkg->SetPath( filePath );
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
#pragma TODO( "Open the file in the editor" )
    NOC_BREAK(); 
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback that displays a normal File Open dialog for choosing
// a new file for this node to reference.
// 
void FileBackedAttribute::OnChangePath( const ContextMenuArgsPtr& args )
{
    Luna::FileDialog dialog( GetAssetManager()->GetAssetEditor(), "Change File Path" );

    std::string currentPath = GetFilePath();
    if ( !currentPath.empty() )
    {
        dialog.SetPath( currentPath.c_str() );
    }

    const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
    const Finder::FinderSpec* spec = pkg->GetFileFilter();
    if ( spec )
    {
        dialog.SetFilter( spec->GetDialogFilter() );
    }

    if ( dialog.ShowModal() == wxID_OK )
    {
        args->GetBatch()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::FileBackedAttribute, std::string >( this, &Luna::FileBackedAttribute::GetFilePath, &Luna::FileBackedAttribute::SetFilePath ), dialog.GetFilePath() ) );
        GetAssetManager()->GetSelection().Refresh();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for displaying the Asset Finder so that a new file
// can be referenced by this node.
// 
void FileBackedAttribute::OnChangePathFinder( const ContextMenuArgsPtr& args )
{
    NOC_BREAK();
#pragma TODO( "Reimplement to use the Vault" )
    //File::FileBrowser dialog( GetAssetManager()->GetAssetEditor(), wxID_ANY, "Change File Path" );
    //const Asset::FileBackedAttribute* pkg = GetPackage< Asset::FileBackedAttribute >();
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
    //        args->GetBatch()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::FileBackedAttribute, std::string >( this, &Luna::FileBackedAttribute::GetFilePath, &Luna::FileBackedAttribute::SetFilePath ), dialog.GetPath() ) );
    //        GetAssetManager()->GetSelection().Refresh();
    //    }
    //}
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for checking out the asset file referenced by this node.
// 
void FileBackedAttribute::OnCheckOutPath( const ContextMenuArgsPtr& args )
{
#pragma TODO( "Make this check out the file" )
NOC_BREAK();
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for showing the revision history for the file represented
// by this node.
// 
void FileBackedAttribute::OnRevisionHistoryPath( const ContextMenuArgsPtr& args )
{
    GetAssetManager()->GetAssetEditor()->RevisionHistory( GetFilePath() );
}


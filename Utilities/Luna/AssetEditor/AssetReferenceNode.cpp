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

#include "Finder/Finder.h"
#include "Editor/RefreshSelectionCommand.h"
#include "Core/Enumerator.h"
#include "Application/UI/ImageManager.h"
#include "Application/UI/FileDialog.h"


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
AssetReferenceNode::AssetReferenceNode( Luna::AssetManager* manager, const Nocturnal::Path& assetPath, const Reflect::Field* field )
: Luna::AssetNode( manager )
, m_Asset( NULL )
, m_Element( NULL )
, m_Field( field )
, m_AttributeContainer( NULL )
, m_AssetPath( assetPath )
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
        m_Asset->GetPackage< Asset::AssetClass >()->RemoveAssetTypeChangedListener( Asset::AssetTypeChangeSignature::Delegate( this, &AssetReferenceNode::AssetTypeChanged ) );
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

    if ( !m_AssetPath.Exists() )
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
        Luna::AssetClass* assetClass = GetAssetManager()->FindAsset( m_AssetPath.Hash() );
        if ( !assetClass )
        {
            std::string error;
            assetClass = GetAssetManager()->Open( m_AssetPath.Get(), error, false );
            if ( !error.empty() )
            {
                Log::Error( "%s\n", error.c_str() );
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

            m_Asset->GetPackage< Asset::AssetClass >()->AddAssetTypeChangedListener( Asset::AssetTypeChangeSignature::Delegate( this, &AssetReferenceNode::AssetTypeChanged ) );
            m_Asset->RegisterAssetReferenceNode( this );

            SetName( MakeLabel() );
            SetIcon( MakeIcon() );
        }
        else
        {
            SetName( "Error loading: " + m_AssetPath.Get() );
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

        m_Asset->GetPackage< Asset::AssetClass >()->RemoveAssetTypeChangedListener( Asset::AssetTypeChangeSignature::Delegate( this, &AssetReferenceNode::AssetTypeChanged ) );

        m_Asset->UnregisterAssetReferenceNode( this );

        DeleteChildren();
        m_Asset = NULL;
        SetName( MakeLabel() );
        SetIcon( MakeIcon() );
        SetStyle( LabelStyles::Bold );
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
    NOC_ASSERT( field->m_Flags & Reflect::FieldFlags::Path );

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


            wxBitmap finderIcon = Nocturnal::GlobalImageManager().GetBitmap( "magnify_16.png" );
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
            Inspect::Value* textBox = args.m_Enumerator->AddValue< Luna::AssetReferenceNode, std::string, Getter, Setter >( args.m_Selection, &Luna::AssetReferenceNode::GetAssetTypeName );
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
std::string AssetReferenceNode::GetAssetTypeName() const
{
    if ( m_Asset )
    {
        return Asset::AssetClass::GetAssetTypeName( m_Asset->GetPackage< Asset::AssetClass >()->GetAssetType() );
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
        Nocturnal::Path np( fileName );
        np.RemoveExtension();
        fileName = np.Filename();
    }
    return fileName;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the full file path associated with this asset ID.
// 
std::string AssetReferenceNode::GetFilePath() const
{
    return m_AssetPath.Get();
}

void AssetReferenceNode::SetFilePath( const std::string& path )
{
    m_AssetPath.Set( path );
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

    if ( m_AssetPath.Exists() )
    {
        label += m_AssetPath.Get();

        if ( !m_Asset )
        {
            label += " (double-click to load)";
        }
    }
    else
    {
        label += "(double-click to set)";
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
    if ( args.m_AssetClass->GetPath().Hash() == m_AssetPath.Hash() )
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
#pragma TODO( "handle file reference changing" )
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the engine type changes.  Updates the UI (icon and property
// panel).
// 
void AssetReferenceNode::AssetTypeChanged( const Asset::AssetTypeChangeArgs& args )
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
    Nocturnal::FileDialog dialog( GetAssetManager()->GetAssetEditor(), "Change File Path" );

    std::string currentPath = GetFilePath();
    if ( !currentPath.empty() )
    {
        dialog.SetPath( currentPath.c_str() );
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
        if ( GetFilePath() != dialog.GetFilePath() )
        {
            args->GetBatch()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::AssetReferenceNode, std::string >( this, &AssetReferenceNode::GetFilePath, &AssetReferenceNode::SetFilePath ), dialog.GetFilePath() ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for displaying the Asset Finder so that a new asset
// can be referenced by this node.
// 
void AssetReferenceNode::OnChangePathFinder( const ContextMenuArgsPtr& args )
{
    NOC_BREAK();
#pragma TODO( "Reimplemnt to use the Vault" )
    //File::FileBrowser dialog( GetAssetManager()->GetAssetEditor(), wxID_ANY, "Change File Path" );

    //std::string specName;
    //if ( m_Field->GetProperty( Asset::AssetProperties::FilterSpec, specName ) )
    //{
    //    const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );
    //    dialog.SetFilter( *spec );
    //}
    //else if ( m_Field->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
    //{
    //    const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );
    //    dialog.SetFilter( *spec );
    //}

    //if ( dialog.ShowModal() == wxID_OK )
    //{
    //    if ( GetFilePath() != dialog.GetPath() )
    //    {
    //        args->GetBatch()->Push( new Undo::PropertyCommand< std::string >( new Nocturnal::MemberProperty< Luna::AssetReferenceNode, std::string >( this, &AssetReferenceNode::GetFilePath, &AssetReferenceNode::SetFilePath ), dialog.GetPath() ) );
    //    }
    //}
}

///////////////////////////////////////////////////////////////////////////////
// Context menu callback for checking out the asset file referenced by this node.
// 
void AssetReferenceNode::OnCheckOutPath( const ContextMenuArgsPtr& args )
{
#pragma TODO( "Make this check out the file" )
NOC_BREAK();
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

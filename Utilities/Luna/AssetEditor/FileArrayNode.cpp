#include "Precompile.h"
#include "FileArrayNode.h"

#include "AssetManager.h"
#include "AssetReferenceNode.h"
#include "AssetUtils.h"
#include "FieldFileReference.h"

#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::FileArrayNode );


///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void FileArrayNode::InitializeType()
{
    Reflect::RegisterClass<Luna::FileArrayNode>( "Luna::FileArrayNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void FileArrayNode::CleanupType()
{
    Reflect::UnregisterClass<Luna::FileArrayNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified field is flagged as a fileRef and if the 
// serializer on the field is appropriate.
// 
bool FileArrayNode::IsFileArray( Reflect::Element* element, const Reflect::Field* field )
{
    bool isFileArray = false;

    if ( field->m_Flags & Reflect::FieldFlags::Path )
    {
        if ( field->m_SerializerID == Reflect::GetType< Reflect::SetSerializer >() )
        {
            isFileArray = true;
        }
    }

    return isFileArray;
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
FileArrayNode::FileArrayNode( Luna::AssetManager* assetManager, Reflect::Element* element, const Reflect::Field* field )
: Luna::ArrayNode( assetManager, element, field )
, m_IsAssetReference( false )
, m_IgnoreChange( false )
{
    // Sanity check
    NOC_ASSERT( IsFileArray( element, field ) );

    m_IsAssetReference = Luna::IsAssetFileReference( element, field );

#pragma TODO( "Implement Add File functionality" )
    ContextMenuItemSet& contextMenu = GetContextMenu();
    ContextMenuItemPtr menuItem = new ContextMenuItem( "Add file" );
    menuItem->Disable();
    contextMenu.AppendSeparator();
    contextMenu.AppendItem( menuItem );

    menuItem = new ContextMenuItem( "Add file (Asset Finder)", "Add a new file to this list using the Asset Finder", UIToolKit::GlobalImageManager().GetBitmap( "magnify_16.png" ) );
    menuItem->Disable();
    contextMenu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
FileArrayNode::~FileArrayNode()
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates a child node for each file in this array.
// 
void FileArrayNode::CreateChildren()
{
#pragma TODO( "reimplement for Nocturnal::Path" )
    //Reflect::SetSerializer* serializer = Reflect::AssertCast< Reflect::SetSerializer >( m_Serializer );
    //for each ( const File::ReferencePtr& fileRef in serializer->m_Data.get() )
    //{
    //    Luna::AssetNode* newNode = NULL;
    //    if ( m_IsAssetReference )
    //    {
    //        Luna::AssetReferenceNodePtr node = new Luna::AssetReferenceNode( GetAssetManager(), fileID, NULL );
    //        node->AssociateField( GetElement(), GetField() );
    //        AddChild( node );
    //        newNode = node;
    //    }
    //    else
    //    {
    //        Luna::FieldFileReferencePtr node = new Luna::FieldFileReference( GetAssetManager(), GetElement(), GetField(), fileID );
    //        node->SetUseLabelPrefix( false );
    //        AddChild( node ); 
    //        newNode = node;
    //    }

    //    // Context menu
    //    ContextMenuItemSet& contextMenu = newNode->GetContextMenu();
    //    ContextMenuItemPtr menuItem = new ContextMenuItem( "Delete" );
    //    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &FileArrayNode::DeleteSelectedChildren ) );
    //    contextMenu.AppendSeparator();
    //    contextMenu.AppendItem( menuItem );
    //}
}

///////////////////////////////////////////////////////////////////////////////
// Called whenever the field associated with this array node is changed.  
// 
void FileArrayNode::HandleFieldChanged()
{
    if ( !m_IgnoreChange )
    {
        DeleteChildren();
        CreateChildren();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Deletes any child items that are selected and removes them from the current
// selection.
// 
void FileArrayNode::DeleteSelectedChildren( const ContextMenuArgsPtr& args )
{
    bool changed = false;

    OS_SelectableDumbPtr selection = GetAssetManager()->GetSelection().GetItems();
    Reflect::U64ArraySerializer* serializer = Reflect::AssertCast< Reflect::U64ArraySerializer >( m_Serializer );
    V_AssetNodeDumbPtr nodesToDelete;
    V_tuid fileIDs = serializer->m_Data.Get();
    NOC_ASSERT( GetChildren().Size() == fileIDs.size() );

    V_tuid::iterator dataItr = fileIDs.begin();
    OS_AssetNodeSmartPtr::Iterator childItr = GetChildren().Begin();
    OS_AssetNodeSmartPtr::Iterator childEnd = GetChildren().End();
    for ( ; childItr != childEnd; ++childItr )
    {
        Luna::AssetNode* child = *childItr;
        if ( child->IsSelected() )
        {
            nodesToDelete.push_back( child );
            dataItr = fileIDs.erase( dataItr );
            selection.Remove( child );
        }
        else
        {
            ++dataItr;
        }
    }

    if ( nodesToDelete.size() > 0 )
    {
        GetAssetManager()->GetSelection().SetItems( selection );
        for each ( Luna::AssetNode* node in nodesToDelete )
        {
            RemoveChild( node );
        }
        serializer->m_Data.Set( fileIDs );
        m_IgnoreChange = true;
        m_Element->RaiseChanged( m_Field );
        m_IgnoreChange = false;
    }
}


#include "Precompile.h"
#include "AssetNode.h"

#include "ContextMenuCallbacks.h"
#include "AssetClass.h"
#include "AssetEditor.h"

#include "Application/Inspect/DragDrop/ReflectClipboardData.h"
#include "Application/UI/ArtProvider.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AssetNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void AssetNode::InitializeType()
{
    Reflect::RegisterClass<Luna::AssetNode>( TXT( "Luna::AssetNode" ) );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void AssetNode::CleanupType()
{
    Reflect::UnregisterClass<Luna::AssetNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetNode::AssetNode( Luna::AssetManager* assetManager )
: m_AssetManager( assetManager )
, m_Name( TXT( "" ) )
, m_Icon( TXT( "null" ) )
, m_Style( LabelStyles::Normal )
, m_Parent( NULL )
, m_CachedAsset( NULL )
, m_IsRearranging( false )
{
    AddDefaultContextMenuItems( m_ContextMenu );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetNode::~AssetNode()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name that should be displayed in the UI for this asset node.
// 
const tstring& AssetNode::GetName() const
{
    return m_Name;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the name that should be displayed in the UI for this asset node.
// 
void AssetNode::SetName( const tstring& name )
{
    if ( name != m_Name )
    {
        NodeNameChangeArgs args( this, m_Name, name );

        m_Name = name;

        m_NameChanged.Raise( args );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to convert a named icon to an index within an image list.
// 
i32 AssetNode::GetIconIndex() const
{
    return Nocturnal::GlobalFileIconsTable().GetIconID( GetIcon() );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of the icon for this node.
// 
const tstring& AssetNode::GetIcon() const
{
    return m_Icon;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the icon that should be displayed in the UI for this node.
// 
void AssetNode::SetIcon( const tstring& icon )
{
    if ( icon != m_Icon )
    {
        m_Icon = icon;
        m_IconChanged.Raise( this );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the current style for how this node should be displayed in the UI.
// 
u32 AssetNode::GetStyle() const
{
    return m_Style;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the style for how this node should be displayed in the UI.
// 
void AssetNode::SetStyle( u32 style )
{
    if ( m_Style != style )
    {
        NodeStyleChangeArgs args( this, m_Style, style );
        m_Style = style;
        m_StyleChanged.Raise( args );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the children of this node.
// 
const OS_AssetNodeSmartPtr& AssetNode::GetChildren() const
{
    return m_Children;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a child to this asset node.  If you do not specify what sibling to put
// the child before, the child will be placed at the end of the child list.
// Returns true if the child was successfully added.
// 
bool AssetNode::AddChild( Luna::AssetNodePtr child, Luna::AssetNode* beforeSibling )
{
    bool result = false;
    if ( !beforeSibling )
    {
        result = m_Children.Append( child );
    }
    else
    {
        result = m_Children.Insert( child, beforeSibling );
    }

    if ( result )
    {
        child->SetParent( this );

        if ( !m_IsRearranging )
        {
            m_ChildAdded.Raise( ChildChangeArgs( this, child ) );
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified child from this node's list.
// 
bool AssetNode::RemoveChild( Luna::AssetNodePtr child )
{
    bool result = false;
    result = m_Children.Remove( child );
    if ( result )
    {
        if ( !m_IsRearranging )
        {
            m_ChildRemoved.Raise( ChildChangeArgs( this, child ) );
        }
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the children from this node.
// 
void AssetNode::DeleteChildren()
{
#pragma TODO( "Events to distinguish removing all children from removing a single child" )
    OS_AssetNodeSmartPtr::Iterator childItr = m_Children.Begin();
    OS_AssetNodeSmartPtr::Iterator childEnd = m_Children.End();
    for ( ; childItr != childEnd; ++childItr )
    {
        Luna::AssetNodePtr child = *childItr;
        m_ChildRemoved.Raise( ChildChangeArgs( this, child ) );
    }

    m_Children.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// This function is provided so that adding children to a node is separate 
// from constructing an asset node.  Override this function in a derived
// class if it needs to set up its inital children.
// 
void AssetNode::CreateChildren()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this node is allowed to be rearranged within the tree.
// 
bool AssetNode::CanBeMoved() const
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if this node is allowed to be copied (this is needed for drag
// and drop operations as well as clipboard copy/paste).
// 
bool AssetNode::CanBeCopied() const
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes should NOC_OVERRIDE this function to provide custom clipboard
// formatted data for use with copy/paste and drag/drop.
// 
Inspect::ReflectClipboardDataPtr AssetNode::GetClipboardData()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes should NOC_OVERRIDE this function to determine whether or not
// a paste/drop operation is valid with the specified data.
// 
bool AssetNode::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes should NOC_OVERRIDE this function to handle paste/drop operations.
// 
bool AssetNode::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
    return false;
}

///////////////////////////////////////////////////////////////////////////////
// Rearranges the specified child to be located before the specified sibling.  
// If the sibling parameter is NULL, this node is moved to the end of the list 
// of siblings.  This function does nothing if it determines that the child is
// already located before the specified sibling.  Do not call this function with
// items that are not children of this node.
// 
bool AssetNode::RearrangeChildren( Luna::AssetNode* child, Luna::AssetNode* beforeSibling )
{
    // Determine whether the rearrange is even necessary
    bool rearrange = false;
    if ( beforeSibling )
    {
        OS_AssetNodeSmartPtr::Iterator currentBefore = GetChildren().FindPrevSibling( Luna::AssetNodePtr( child ) );
        if ( currentBefore != GetChildren().End() )
        {
            rearrange = ( ( *currentBefore ).Ptr() ) != beforeSibling;
        }
        else
        {
            rearrange = true;
        }
    }
    else
    {
        rearrange = GetChildren().Back().Ptr() != child;
    }

    // Do the rearrange
    bool isOk = false;
    if ( rearrange )
    {
        // Make sure that the child is not deleted while we are removing it.
        Luna::AssetNodePtr hold( child );

        // Supress Add/Remove/Parent events
        m_IsRearranging = true;

        if ( RemoveChild( hold ) )
        {
            if ( AddChild( hold, beforeSibling ) )
            {
                isOk = true;
            }
            else
            {
                throw Nocturnal::Exception( TXT( "Unable to rearrange %s before %s" ), GetName().c_str(), beforeSibling->GetName().c_str() );
            }
        }
        m_IsRearranging = false;
    }

    if ( isOk )
    {
        // Fire event
        m_ChildRearranged.Raise( ChildChangeArgs( this, child ) );
    }

    return isOk;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified node is allowed to made into a child of this
// one.
// 
bool AssetNode::CanAddChild( Luna::AssetNode* child ) const
{
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes should NOC_OVERRIDE this function to provide an undoable command
// for deleting the selected children of this node.
// 
Undo::CommandPtr AssetNode::DeleteSelectedChildren()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Travels up the hierarchy and returns the first Luna::AssetClass that is found.
// 
Luna::AssetClass* AssetNode::GetAssetClass() const
{
    if ( !m_CachedAsset && m_Parent )
    {
        m_CachedAsset = m_Parent->GetAssetClass();
    }

    // If you hit this assertion, there's a node in the hierarchy that does not 
    // correspond to an asset class. This should not be possible!
    NOC_ASSERT( m_CachedAsset );

    return m_CachedAsset;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the parent of this node (might be NULL).
// 
Luna::AssetNode* AssetNode::GetParent() const
{
    return m_Parent;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified node is an ancestor of this one.
// 
bool AssetNode::IsAncestor( Luna::AssetNode* node )
{
    // I expect that you will probably want to check for this condition elsewhere.
    NOC_ASSERT( node != this );

    bool isAncestor = false;

    Luna::AssetNode* parent = m_Parent;
    while ( parent && !isAncestor )
    {
        isAncestor = parent == node;
        parent = parent->GetParent();
    }

    return isAncestor;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the context menu for this node.  This function is called when the
// menu is about to be shown. 
// 
ContextMenuItemSet& AssetNode::GetContextMenu()
{
    return m_ContextMenu;
}

///////////////////////////////////////////////////////////////////////////////
// Replaces this node's context menu with the one specified.
// 
void AssetNode::SetContextMenu( const ContextMenuItemSet& menu )
{
    m_ContextMenu = menu;
}

///////////////////////////////////////////////////////////////////////////////
// Called before the context menu is shown.  Override this function to manipulate
// the menu before it is shown.
// 
void AssetNode::PreShowContextMenu()
{
}

///////////////////////////////////////////////////////////////////////////////
// Override this funciton to handle item activation (double-clicking on an item
// in UI tree).
void AssetNode::ActivateItem()
{
}

///////////////////////////////////////////////////////////////////////////////
// Adds the default context menu items to the specified menu.
// 
void AssetNode::AddDefaultContextMenuItems( ContextMenuItemSet& menu ) const
{
}

///////////////////////////////////////////////////////////////////////////////
// Changes the parent of this node and notifies listeners.
// 
void AssetNode::SetParent( Luna::AssetNode* parent )
{
    if ( parent != m_Parent )
    {
        Luna::AssetNode* oldParent = m_Parent;

        if ( !m_IsRearranging )
        {
            m_ParentChanging.Raise( ParentChangeArgs( this, oldParent, parent ) );
        }

        m_Parent = parent;

        // Asset class will be recalculated next time it is needed.
        ClearCachedAssetClass();

        if ( !m_IsRearranging )
        {
            m_ParentChanged.Raise( ParentChangeArgs( this, oldParent, m_Parent ) );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Clears the cached asset class pointer on this node and recursively does so
// on all descendants of this node.
// 
void AssetNode::ClearCachedAssetClass()
{
    m_CachedAsset = NULL; 

    // Recurse on children.
    OS_AssetNodeSmartPtr::Iterator childItr = m_Children.Begin();
    OS_AssetNodeSmartPtr::Iterator childEnd = m_Children.End();
    for ( ; childItr != childEnd; ++childItr )
    {
        ( *childItr )->ClearCachedAssetClass();
    }
}

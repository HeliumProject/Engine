#include "EditorPch.h"
#include "Editor/SceneOutliner.h"

#include "Application/UndoQueue.h"

#include "EditorScene/Scene.h"
#include "EditorScene/SceneManager.h"

using namespace Helium;
using namespace Helium::Editor;

// Helper macro.  In debug, asserts that m_TreeCtrl exists.  In release,
// bails out of the function if m_TreeCtrl does not exist.
#define VERIFY_TREE_CTRL()  \
	HELIUM_ASSERT( m_TreeCtrl );  \
	if ( !m_TreeCtrl )        \
{                         \
	return;                 \
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
SceneOutliner::SceneOutliner( Editor::SceneManager* sceneManager )
	: m_SceneManager( sceneManager )
	, m_CurrentScene( NULL )
	, m_TreeCtrl( NULL )
	, m_IgnoreSelectionChange( false )
	, m_DisplayCounts( false )
{
	m_SceneManager->e_CurrentSceneChanged.Add( Editor::SceneChangeSignature::Delegate::Create<SceneOutliner, void (SceneOutliner::*)( const Editor::SceneChangeArgs& args )> ( this, &SceneOutliner::CurrentSceneChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
SceneOutliner::~SceneOutliner()
{
	DisconnectDynamicEventTable();
	m_SceneManager->e_CurrentSceneChanged.Remove( Editor::SceneChangeSignature::Delegate::Create<SceneOutliner, void (SceneOutliner::*)( const Editor::SceneChangeArgs& args )> ( this, &SceneOutliner::CurrentSceneChanged ) );
	DisconnectSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// This function needs to be called to create the internal tree control GUI, and
// should only be called once.
// 
SortTreeCtrl* SceneOutliner::InitTreeCtrl( wxWindow* parent, wxWindowID id )
{
	HELIUM_ASSERT( !m_TreeCtrl );
	if ( !m_TreeCtrl )
	{
		m_TreeCtrl = CreateTreeCtrl( parent, id );
		HELIUM_ASSERT( m_TreeCtrl );

		// Set up dynamic event handlers for UI callbacks
		ConnectDynamicEventTable();
	}
	return m_TreeCtrl;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the expand/collapse state of the outline so that it can be restored 
// later.
// 
void SceneOutliner::SaveState( SceneOutlinerState& state )
{
	VERIFY_TREE_CTRL();

	m_StateInfo.SetHorizontalScrollBarPos( m_TreeCtrl->GetScrollPos( wxHORIZONTAL ) );
	m_StateInfo.SetVerticalScrollBarPos( m_TreeCtrl->GetScrollPos( wxVERTICAL ) );
	state = m_StateInfo;
}

///////////////////////////////////////////////////////////////////////////////
// Restores the expand/collapse state of the outline from the specified data.
// 
void SceneOutliner::RestoreState( const SceneOutlinerState& state )
{
	m_StateInfo.Reset();
	m_StateInfo = state;
}

///////////////////////////////////////////////////////////////////////////////
// Disables sorting in this outliner.
// 
void SceneOutliner::DisableSorting()
{
	m_TreeCtrl->DisableSorting();
}

///////////////////////////////////////////////////////////////////////////////
// Enables sorting in this outliner (you still need to call Sort if you want
// to actual reorder items in the tree).
// 
void SceneOutliner::EnableSorting()
{
	m_TreeCtrl->EnableSorting();
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to fetch the client data off a given tree item.
// 
SceneOutlinerItemData* SceneOutliner::GetTreeItemData( const wxTreeItemId& item )
{
	HELIUM_ASSERT( item.IsOk() );
	HELIUM_ASSERT( m_TreeCtrl->GetItemData( item ) != NULL );

	return static_cast< SceneOutlinerItemData* >( m_TreeCtrl->GetItemData( item ) );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the current scene displayed by this oultiner.
// 
void SceneOutliner::UpdateCurrentScene( Editor::Scene* scene )
{
	if ( m_CurrentScene != scene )
	{
		Editor::Scene* oldScene = m_CurrentScene;

		m_TreeCtrl->Freeze();

		CurrentSceneChanging( scene );

		Clear();
		DisconnectSceneListeners();

		m_CurrentScene = scene;

		ConnectSceneListeners();

		CurrentSceneChanged( oldScene );

		m_TreeCtrl->Thaw();

		// Presumably, derived classes will have populated the tree by this point,
		// so we can restore the state of the outline now.
		DoRestoreState();
	}
}

///////////////////////////////////////////////////////////////////////////////
// Actually restores the state info from the stored member variable.  Should
// be called after the current scene has been switched out.
// 
void SceneOutliner::DoRestoreState()
{
	VERIFY_TREE_CTRL();

	if ( m_StateInfo.GetExpandedObjects().size() > 0 )
	{
		std::set< Reflect::Object* >::const_iterator itr = m_StateInfo.GetExpandedObjects().begin();
		std::set< Reflect::Object* >::const_iterator end = m_StateInfo.GetExpandedObjects().end();
		for ( ; itr != end; ++itr )
		{
			Reflect::Object* object = *itr;
			M_TreeItems::const_iterator found = m_Items.find( object );
			if ( found != m_Items.end() )
			{
				m_TreeCtrl->EnsureVisible( found->second );
				m_TreeCtrl->Expand( found->second );
			}
		}
	}

	m_TreeCtrl->SetScrollPos( wxHORIZONTAL, m_StateInfo.GetHorizontalScrollBarPos() );
	m_TreeCtrl->SetScrollPos( wxVERTICAL, m_StateInfo.GetVerticalScrollBarPos() );
}

///////////////////////////////////////////////////////////////////////////////
// Recursively sorts the tree control, starting at the specified root.
// 
void SceneOutliner::Sort( const wxTreeItemId& root )
{
	HELIUM_EDITOR_SCOPE_TIMER();

	m_TreeCtrl->Sort( root );
}

///////////////////////////////////////////////////////////////////////////////
// Clears out the tree.
// 
void SceneOutliner::Clear()
{
	m_Items.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified tree item as a child of root.
// 
wxTreeItemId SceneOutliner::AddItem( const wxTreeItemId& parent, const std::string& name, int32_t image, SceneOutlinerItemData* data, bool isSelected, bool countable)
{
	HELIUM_EDITOR_SCOPE_TIMER();
	HELIUM_ASSERT(data); 

	bool isVisible = true;

	Editor::SceneNode* node = Reflect::SafeCast< Editor::SceneNode >( data->GetObject() );
	if ( node )
	{
		isVisible = node->IsVisible();
		node->AddNameChangedListener( SceneNodeChangeSignature::Delegate( this, &SceneOutliner::SceneNodeNameChanged ) );
		node->AddVisibilityChangedListener( SceneNodeChangeSignature::Delegate( this, &SceneOutliner::SceneNodeVisibilityChanged ) );
	}

	// Actually add the item
	wxTreeItemId item = m_TreeCtrl->AppendItem( parent, name.c_str(), image, image, data );
	m_Items.insert( M_TreeItems::value_type( data->GetObject(), item ) );

	data->SetItemText( name ); 
	data->SetCountable( countable ); 
	data->SetCachedCount( countable ? 1 : 0 ); 

	UpdateItemVisibility( item, isVisible );

	if ( isSelected )
	{
		bool isIgnoring = m_IgnoreSelectionChange;
		m_IgnoreSelectionChange = true;
		m_TreeCtrl->SelectItem( item );
		m_IgnoreSelectionChange = isIgnoring;
	}

	if ( m_DisplayCounts && countable )
	{
		UpdateItemCounts( parent, data->GetCachedCount() ); 
	}

	return item;
}

///////////////////////////////////////////////////////////////////////////////
// Deletes the tree item associated with the specified Object.
// 
void SceneOutliner::DeleteItem( Reflect::Object* object )
{
	HELIUM_EDITOR_SCOPE_TIMER();

	// Delete the item if it is in our list
	M_TreeItems::iterator found = m_Items.find( object );
	if ( found != m_Items.end() )
	{
		wxTreeItemId item = found->second;

		SceneOutlinerItemData* data = (SceneOutlinerItemData*) m_TreeCtrl->GetItemData( item ); 

		// Sanity checks
		HELIUM_ASSERT( data );
		HELIUM_ASSERT( data->GetObject() == object );

		// access the node and the data before erasing it from the tree control
		// just in case we have to update the counts
		wxTreeItemId parent = m_TreeCtrl->GetItemParent( item ); 
		int cachedCount = data->GetCachedCount(); 

		m_TreeCtrl->Delete( item );
		data = NULL; 

		// now update the counts if necessary. 
		if ( m_DisplayCounts )
		{
			UpdateItemCounts(parent, -cachedCount); 
		}
	}
}

void SceneOutliner::UpdateItemCounts( const wxTreeItemId& node, int delta )
{
	if(!node)
	{
		return; 
	}

	SceneOutlinerItemData* data = (SceneOutlinerItemData*) m_TreeCtrl->GetItemData(node); 
	if(!data)
	{
		return; // probably only INVISIBLE_ROOT has no data
	}

	int finalCount = data->GetCachedCount() + delta; 
	HELIUM_ASSERT(finalCount >= 0); 

	data->SetCachedCount( finalCount ); 

	if( finalCount > 0 )
	{
		std::stringstream str; 
		str << data->GetItemText() << TXT( " (" ) << finalCount << TXT( ")" ); 
		m_TreeCtrl->SetItemText(node, str.str().c_str()); 
	}
	else
	{
		m_TreeCtrl->SetItemText(node, data->GetItemText().c_str()); 
	}

	wxTreeItemId parent = m_TreeCtrl->GetItemParent(node); 
	UpdateItemCounts(parent, delta); 
}

///////////////////////////////////////////////////////////////////////////////
// Changes the display of the item to Reflect the current visibility setting.
// Invisible items are rendered in a different color font.
// 
void SceneOutliner::UpdateItemVisibility( const wxTreeItemId& item, bool visible )
{
	if ( visible )
	{
		m_TreeCtrl->SetItemTextColour( item, wxNullColour );
	}
	else
	{
		static wxColour color = wxTheColourDatabase->Find( TXT( "DARK TURQUOISE" ) );
		m_TreeCtrl->SetItemTextColour( item, color );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to connect listeners for events on the current scene.  Can
// be overridden by derived classes to connect their own event listeners at
// the same time.  If overridden, the base class implementation should still
// be called.
// 
void SceneOutliner::ConnectSceneListeners()
{
	if ( m_CurrentScene )
	{
		m_CurrentScene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &SceneOutliner::SelectionChanged ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to remove all of this class's listeners from the current
// scene.  Can be overridden in derived classes to clean up their own listeners,
// but make sure to call the base class implementation as well.
// 
void SceneOutliner::DisconnectSceneListeners()
{
	if ( m_CurrentScene )
	{
		m_CurrentScene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &SceneOutliner::SelectionChanged ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Called by UpdateCurrentScene just before actually changing the current scene
// displayed by the tree control.  Derived classes can HELIUM_OVERRIDE this function
// to do custom work.
// 
void SceneOutliner::CurrentSceneChanging( Editor::Scene* newScene )
{
	// Override in derived classes if you need to do something here.
}

///////////////////////////////////////////////////////////////////////////////
// Called by UpdateCurrentScene after the current scene has been changed, but
// before thawing the tree control so that it will refresh.  Can be overridden
// in derived classes to initially populate the tree.
// 
void SceneOutliner::CurrentSceneChanged( Editor::Scene* oldScene )
{
	// Override in derived classes if you need to do something here.
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is changed.  Sets the current scene
// shown in the tree.
// 
void SceneOutliner::CurrentSceneChanged( const SceneChangeArgs& args )
{
	VERIFY_TREE_CTRL();

	UpdateCurrentScene( args.m_Scene );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when selection changes.  Updates the selected items in the
// tree control to match the specified selection.
// 
void SceneOutliner::SelectionChanged( const SelectionChangeArgs& args )
{
	HELIUM_EDITOR_SCOPE_TIMER();

	m_IgnoreSelectionChange = true;
	m_TreeCtrl->Freeze();

	m_TreeCtrl->UnselectAll();

	OS_ObjectDumbPtr::Iterator itr = args.m_Selection.Begin();
	OS_ObjectDumbPtr::Iterator end = args.m_Selection.End();
	for ( ; itr != end; ++itr )
	{
		M_TreeItems::const_iterator found = m_Items.find( *itr );
		if ( found != m_Items.end() )
		{
			m_TreeCtrl->SelectItem( found->second );
		}
	}

	m_TreeCtrl->Thaw();
	m_IgnoreSelectionChange = false;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the name of a node changes.  Updates the label on the 
// associated tree item.
// 
void SceneOutliner::SceneNodeNameChanged( const SceneNodeChangeArgs& args )
{
	M_TreeItems::const_iterator found = m_Items.find( args.m_Node );
	if ( found != m_Items.end() )
	{
		const wxTreeItemId& item = found->second;

		SceneOutlinerItemData* data = ( SceneOutlinerItemData* ) m_TreeCtrl->GetItemData( item ); 
		data->SetItemText( args.m_Node->GetName() ); 

		// TODO: Make the node counting code work better with rename of hierarchy nodes
		// this code will not work that great if we get a rename of a hierarchy node that
		// is counting its children. right now, that doesn't happen. 
		// 
		// however, the 2 things that we should do are: 
		// * hook rename begin to just display the item text, not the count 
		// * make this code that sets the item text be the same as what is in UpdateCounts
		//
		m_TreeCtrl->SetItemText( item, args.m_Node->GetName().c_str() );

		Sort( m_TreeCtrl->GetItemParent( item ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when node visibility changes.  Updates the UI to indicate whether
// a node is visible or not by changing how the item is displayed in the tree.
// 
void SceneOutliner::SceneNodeVisibilityChanged( const SceneNodeChangeArgs& args )
{
	M_TreeItems::const_iterator found = m_Items.find( args.m_Node );
	if ( found != m_Items.end() )
	{
		UpdateItemVisibility( found->second, args.m_Node->IsVisible() );
	}
}

///////////////////////////////////////////////////////////////////////////////
// GUI callback for when a label is done being edited by the user.  Creates a
// command to change the name of the dependency node associated with the item.
// 
void SceneOutliner::OnEndLabelEdit( wxTreeEvent& args )
{
	if ( !args.IsEditCancelled() )
	{
		SceneOutlinerItemData* data = GetTreeItemData( args.GetItem() );
		Reflect::Object* object = data->GetObject();
		Editor::SceneNode* node = Reflect::SafeCast< Editor::SceneNode >( object );
		if ( node )
		{
			const std::string newName ( args.GetLabel().c_str() );
			if ( node->GetName() != newName )
			{
				// Create an undoable command to rename the object
				m_CurrentScene->Push( new PropertyUndoCommand<std::string>( new Helium::MemberProperty<Editor::SceneNode, std::string> (node, &Editor::SceneNode::GetName, &Editor::SceneNode::SetGivenName), newName) );
				m_CurrentScene->Execute( false );

				// Sort
				Sort( m_TreeCtrl->GetItemParent( args.GetItem() ) );

				// Keep the item in view
				if ( !m_TreeCtrl->IsVisible( args.GetItem() ) )
				{
					m_TreeCtrl->EnsureVisible( args.GetItem() );
				}
			}
			args.Veto();
			wxTreeEvent evt ( 0 );
			OnSelectionChanged( evt );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the user selects an item in the tree.  The selection might
// be vetoed if the item is not selectable.
// 
void SceneOutliner::OnSelectionChanging( wxTreeEvent& args )
{
	HELIUM_EDITOR_SCOPE_TIMER();

	if ( !m_IgnoreSelectionChange )
	{
		const wxTreeItemId& item = args.GetItem();
		// Any item can be deselected, so only check the case where an item is
		// about to be selected.
		if ( !m_TreeCtrl->IsSelected( item ) )
		{
			SceneOutlinerItemData* data = GetTreeItemData( item );
			SceneNode* node = Reflect::SafeCast<SceneNode>( data->GetObject() );
			if ( node )
			{
				if ( !node->IsSelectable() )
				{
					args.Veto();
				}
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a selection change has been completed by a user interaction.
// Updates the scene selection to match that of the tree.
// 
void SceneOutliner::OnSelectionChanged( wxTreeEvent& args )
{
	HELIUM_EDITOR_SCOPE_TIMER();

	if ( m_CurrentScene && !m_IgnoreSelectionChange )
	{
		OS_SceneNodeDumbPtr nodes;

		wxArrayTreeItemIds selections;
		const size_t numSelections = m_TreeCtrl->GetSelections( selections );
		for ( size_t i = 0; i < numSelections; i++ )
		{
			SceneOutlinerItemData* itemData = GetTreeItemData( selections[i] );
			if ( itemData )
			{
				SceneNode* node = Reflect::SafeCast<SceneNode>( itemData->GetObject() );
				if ( node )
				{
					nodes.Append( node );
				}
			}
		}  

		// Set the selection on the scene.  Note, the second parameter is a delegate that
		// tells the scene selection event not to pass the event back to this class since
		// we have already updated ourselves.
		m_CurrentScene->Push( m_CurrentScene->GetSelection().SetItems( nodes, SelectionChangingSignature::Delegate (), SelectionChangedSignature::Delegate( this, &SceneOutliner::SelectionChanged ) ) );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is expanded in the tree.  Stores the expansion state
// in case it is needed later.  See SaveState and RestoreState.
// 
void SceneOutliner::OnExpanded( wxTreeEvent& args )
{
	M_TreeItems::const_iterator found = m_Items.find( GetTreeItemData( args.GetItem() )->GetObject() );
	if ( found != m_Items.end() )
	{
		m_StateInfo.AddExpandedObject( found->first );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is collapsed in the tree.  Updates the expansion state
// of the tree item.  See SaveState and RestoreState for more information.
// 
void SceneOutliner::OnCollapsed( wxTreeEvent& args )
{
	M_TreeItems::const_iterator found = m_Items.find( GetTreeItemData( args.GetItem() )->GetObject() );
	if ( found != m_Items.end() )
	{
		m_StateInfo.RemoveExpandedObject( found->first );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an item is deleted from the tree.  Updates the internal
// item list to Reflect the change in the tree.
// 
void SceneOutliner::OnDeleted( wxTreeEvent& args )
{
	// If the object is a dependency node, disconnect our listeners from it
	Reflect::Object* object = GetTreeItemData( args.GetItem() )->GetObject();
	Editor::SceneNode* node = Reflect::SafeCast< Editor::SceneNode >( object );
	if ( node )
	{
		node->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate( this, &SceneOutliner::SceneNodeNameChanged ) );
		node->RemoveVisibilityChangedListener( SceneNodeChangeSignature::Delegate( this, &SceneOutliner::SceneNodeVisibilityChanged ) );
	}

	// Delete the item if it is in our list
	m_Items.erase( object );
}

///////////////////////////////////////////////////////////////////////////////
// Pass shortcut keys up to the main frame/view for handling.
// 
void SceneOutliner::OnChar( wxKeyEvent& args )
{
	switch ( args.GetKeyCode() )
	{
	case WXK_UP:
	case WXK_DOWN:
	case WXK_LEFT:
	case WXK_RIGHT:
		// Do nothing, the arrow keys are already handled by the tree control
		args.Skip();
		break;

	case WXK_DELETE:
		if ( m_SceneManager->HasCurrentScene() )
		{
			m_SceneManager->GetCurrentScene()->Push( m_SceneManager->GetCurrentScene()->DeleteSelected() );
		}
		break;

	case 'F':
		if ( m_SceneManager->HasCurrentScene() )
		{
			m_SceneManager->GetCurrentScene()->FrameSelected();
		}
		break;

		// Pass everything else on to the view to handle the normal keyboard shortcuts
	default:
		args.Skip();
		args.ResumePropagation( wxEVENT_PROPAGATE_MAX );
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Hookup callbacks to GUI events on the tree control owned by this class
// (replaces the static event table for responding to GUI events).
// 
void SceneOutliner::ConnectDynamicEventTable()
{
	if ( m_TreeCtrl )
	{
		// Set up dynamic event handlers for UI callbacks from the tree control
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( SceneOutliner::OnEndLabelEdit ), NULL, this );
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( SceneOutliner::OnSelectionChanging ), NULL, this );
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( SceneOutliner::OnSelectionChanged ), NULL, this );
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( SceneOutliner::OnExpanded ), NULL, this );
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( SceneOutliner::OnCollapsed ), NULL, this );
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( SceneOutliner::OnDeleted ), NULL, this );
		m_TreeCtrl->Connect( m_TreeCtrl->GetId(), wxEVT_CHAR, wxKeyEventHandler( SceneOutliner::OnChar ), NULL, this );
	}
}

///////////////////////////////////////////////////////////////////////////////
// Unhook callbacks to GUI events.
// 
void SceneOutliner::DisconnectDynamicEventTable()
{
	if ( m_TreeCtrl )
	{
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_END_LABEL_EDIT, wxTreeEventHandler( SceneOutliner::OnEndLabelEdit ), NULL, this );
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGING, wxTreeEventHandler( SceneOutliner::OnSelectionChanging ), NULL, this );
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( SceneOutliner::OnSelectionChanged ), NULL, this );
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_EXPANDED, wxTreeEventHandler( SceneOutliner::OnExpanded ), NULL, this );
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_ITEM_COLLAPSED, wxTreeEventHandler( SceneOutliner::OnCollapsed ), NULL, this );
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_COMMAND_TREE_DELETE_ITEM, wxTreeEventHandler( SceneOutliner::OnDeleted ), NULL, this );
		m_TreeCtrl->Disconnect( m_TreeCtrl->GetId(), wxEVT_CHAR, wxKeyEventHandler( SceneOutliner::OnChar ), NULL, this );
	}
}

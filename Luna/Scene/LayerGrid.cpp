#include "Precompile.h"
#include "LayerGrid.h"
#include "DependencyCommand.h"
#include "Grid.h"
#include "Layer.h"
#include "Scene.h"
#include "SceneEditor.h"
#include "SceneManager.h"
#include "SceneEditorIDs.h"
#include "Foundation/Log.h"
#include "Application/UI/ArtProvider.h"
#include "Foundation/Container/Insert.h" 

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Table for GUI events
// 
BEGIN_EVENT_TABLE(LayerGrid, wxEvtHandler )
EVT_MENU( ID_NewLayer,LayerGrid::OnNewLayer )
EVT_MENU( ID_NewLayerFromSelection,LayerGrid::OnNewLayerFromSelection )
EVT_MENU( ID_DeleteLayer,LayerGrid::OnDeleteLayer )
EVT_MENU( ID_AddSelectionToLayer,LayerGrid::OnAddSelectionToLayer )
EVT_MENU( ID_RemoveSelectionFromLayer,LayerGrid::OnRemoveSelectionFromLayer )
EVT_MENU( ID_SelectLayerMembers,LayerGrid::OnSelectLayerMembers )
EVT_MENU( ID_SelectLayer,LayerGrid::OnSelectLayer )
EVT_MENU( ID_CleanUpLayers,LayerGrid::OnCleanUpLayers )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
// LGridLayer::LNameChangeInfo
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LayerGrid::NameChangeInfo::NameChangeInfo()
: m_Layer( NULL )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LayerGrid::NameChangeInfo::~NameChangeInfo()
{
}

///////////////////////////////////////////////////////////////////////////////
// Clears out internal member data to original state.
// 
void LayerGrid::NameChangeInfo::Clear()
{
  m_Layer = NULL;
  m_OldName.clear();
}

///////////////////////////////////////////////////////////////////////////////
//LayerGrid
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LayerGrid::LayerGrid( wxWindow* parent, Luna::SceneManager* sceneManager )
: m_SceneManager( sceneManager )
, m_Scene( NULL )
, m_Panel( new wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, wxT( "LayerGrid Panel" ) ) )
, m_Grid( new Grid( m_Panel, SceneEditorIDs::ID_LayerGrid, true ) )
, m_ToolBar( new wxToolBar( m_Panel, wxID_ANY, wxDefaultPosition, wxSize( -1, 25 ), wxBORDER_NONE | wxTB_HORIZONTAL | wxTB_NODIVIDER, wxT( "LayerToolBar" ) ) )
{
    m_Panel->SetHelpText( TXT( "This is the layer panel.  You can select and manipulate layers here." ) );
    m_ToolBar->SetHelpText( TXT( "This is the layer toolbar." ) );
    m_Grid->GetPanel()->SetHelpText( TXT( "This is the layer grid, you can select a layer to manipulate here." ) );

    // Handle all toolbar events so that this class can process button clicks
  m_ToolBar->PushEventHandler( this );

  // Set up the toolbar that goes at the top of the panel
  m_ToolBar->AddTool( ID_NewLayer, wxT( "" ), wxArtProvider::GetBitmap( wxART_NEW ), wxT( "Create New Layer" ) );
  m_ToolBar->AddTool( ID_NewLayerFromSelection, wxT( "" ), wxArtProvider::GetBitmap( wxART_NEW_DIR ), wxT( "Create New Layer From Selection" ) );
  m_ToolBar->AddTool( ID_DeleteLayer, wxT( "" ), wxArtProvider::GetBitmap( wxART_DELETE ), wxT( "Delete Highlighted Layer(s)" ) );
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool( ID_AddSelectionToLayer, wxT( "" ), wxArtProvider::GetBitmap( wxART_ADD_BOOKMARK ), wxT( "Add Selection To Highlighted Layer(s)" ) );
  m_ToolBar->AddTool( ID_RemoveSelectionFromLayer, wxT( "" ), wxArtProvider::GetBitmap( wxART_DEL_BOOKMARK ), wxT( "Remove Selection From Highlighted Layer(s)" ) );
  m_ToolBar->AddSeparator();
  m_ToolBar->AddTool( ID_SelectLayerMembers, wxT( "" ), wxArtProvider::GetBitmap( wxART_MISSING_IMAGE ), wxT( "Select Layer Members" ) );
  m_ToolBar->AddTool( ID_SelectLayer, wxT( "" ), wxArtProvider::GetBitmap( wxART_MISSING_IMAGE ), wxT( "Select Highlighted Layer(s)" ) );
  m_ToolBar->Realize();

  // Add everything to a top level sizer
  wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->Add( m_ToolBar, 0, wxALIGN_TOP );
  mainSizer->Add( m_Grid->GetPanel(), 1, wxEXPAND );
  mainSizer->Layout();
  m_Panel->SetAutoLayout( true );
  m_Panel->SetSizer( mainSizer );
  mainSizer->SetSizeHints( m_Panel );

  // Make sure the toolbar buttons start out disabled
  UpdateToolBarButtons();

  // Listeners that are not dependent on the current scene
  if ( m_SceneManager )
  {
    m_SceneManager->AddCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &LayerGrid::CurrentSceneChanging ) );
    m_SceneManager->AddCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &LayerGrid::CurrentSceneChanged ) );
  }
  m_Grid->AddRowVisibilityChangedListener( GridRowChangeSignature::Delegate  ( this, &LayerGrid::LayerVisibleChanged ) );
  m_Grid->AddRowSelectabilityChangedListener( GridRowChangeSignature::Delegate  ( this, &LayerGrid::LayerSelectableChanged ) );
  m_Grid->AddRowRenamedListener( GridRowRenamedSignature::Delegate ( this, &LayerGrid::RowRenamed ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LayerGrid::~LayerGrid()
{
  // Remove listeners that are not dependent on the current scene
  if ( m_SceneManager )
  {
    m_SceneManager->RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &LayerGrid::CurrentSceneChanging ) );
    m_SceneManager->RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &LayerGrid::CurrentSceneChanged ) );
  }
  m_Grid->RemoveRowVisibilityChangedListener( GridRowChangeSignature::Delegate  ( this, &LayerGrid::LayerVisibleChanged ) );
  m_Grid->RemoveRowSelectabilityChangedListener( GridRowChangeSignature::Delegate  ( this, &LayerGrid::LayerSelectableChanged ) );
  m_Grid->RemoveRowRenamedListener( GridRowRenamedSignature::Delegate ( this, &LayerGrid::RowRenamed ) );

  DisconnectSceneListeners();

  m_ToolBar->PopEventHandler();

  if ( m_Panel->GetParent() == NULL )
  {
    delete m_Panel;
    m_Panel = NULL;
  }

  delete m_Grid;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the top-level panel that houses all the controls that belong to
// this class.  This panel can be nested inside another panel, or added to
// the main frame of the application.
// 
wxPanel*LayerGrid::GetPanel() const
{
  return m_Panel;
}

///////////////////////////////////////////////////////////////////////////////
//
wxToolBar*  LayerGrid::GetToolBar()  const
{
  return m_ToolBar;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a layer to the list managed by this control  The layer will show up
// in the grid, and its selectability and visibility will be able to be 
// changed.
// 
bool LayerGrid::AddLayer( Luna::Layer* layer )
{
  //Verify this is the correct destination
  Content::Layer* cLayer = layer->GetPackage<Content::Layer>();

    Nocturnal::Insert<M_LayerDumbPtr>::Result inserted = m_Layers.insert( M_LayerDumbPtr::value_type( layer->GetName(), layer ) );
    NOC_ASSERT( inserted.second );

    // Listen for name changes to this layer
    layer->AddNameChangingListener( SceneNodeChangeSignature::Delegate ( this, &LayerGrid::NameChanging ) );
    layer->AddNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LayerGrid::NameChanged ) );

    bool  result = m_Grid->AddRow( layer->GetName(), layer->IsVisible(), layer->IsSelectable() );

    // Select the newly added row
    if(result)
    {
      m_Grid->SelectRow(m_Grid->GetRowNumber(layer->GetName()), false);
      return true;
    }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified layer from the list managed by this control.
// 
bool LayerGrid::RemoveLayer( Luna::Layer* layer )
{
  //Verify this is the correct destination
  Content::Layer* cLayer = layer->GetPackage<Content::Layer>();

    bool foundLayer = m_Layers.erase( layer->GetName() ) > 0;
    NOC_ASSERT( foundLayer );

    // We are no longer tracking this layer, so don't listen for name changes any more
    layer->RemoveNameChangingListener( SceneNodeChangeSignature::Delegate ( this, &LayerGrid::NameChanging ) );
    layer->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LayerGrid::NameChanged ) );
    return m_Grid->RemoveRow( layer->GetName() );

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Removes layers with one or zero elements
// 
void LayerGrid::OnCleanUpLayers(wxCommandEvent& event)
{
  CleanUpLayers();
}

///////////////////////////////////////////////////////////////////////////////
// Increments the internal batch count.  Every call to BeginBatch() should be
// accompanied by a call to EndBatch().  When the batch count reaches zero,
// the grid will redraw.  Use this function to halt UI updates to the grid
// when making a lot of changes all at once.
// 
void LayerGrid::BeginBatch()
{
  m_Grid->BeginBatch();
}

///////////////////////////////////////////////////////////////////////////////
// Decrements teh internal batch count.  When the batch count reaches zero, the
// grid will redraw.  See BeginBatch() for more information.
// 
void LayerGrid::EndBatch()
{
  m_Grid->EndBatch();
}

///////////////////////////////////////////////////////////////////////////////
// Connects this object to all the events that we want to listen for.
// 
void LayerGrid::ConnectSceneListeners()
{
  if ( m_Scene )
  {
    // Add listeners for when layers are added/removed from a the scene
    m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &LayerGrid::NodeAdded ) );
    m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LayerGrid::SceneNodeRemoved ) );

    // Listen for changes to the scene's selection
    m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LayerGrid::SelectionChanged ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Disconnects this object from all the listeners that we added ourselves to.
// 
void LayerGrid::DisconnectSceneListeners()
{
  if ( m_Scene )
  {
    // Remove layer creation listeners on the scene
    m_Scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &LayerGrid::NodeAdded ) );
    m_Scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LayerGrid::SceneNodeRemoved ) );

    // Remove selection change listener on scene
    m_Scene->RemoveSelectionChangedListener( SelectionChangedSignature::Delegate ( this, &LayerGrid::SelectionChanged ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes all the layers currently managed by this control.
// 
void LayerGrid::RemoveAllLayers()
{
  // Remove name change listener on each layer
  M_LayerDumbPtr::const_iterator layerItr = m_Layers.begin();
  M_LayerDumbPtr::const_iterator layerEnd = m_Layers.end();
  for ( ; layerItr != layerEnd; ++layerItr )
  {
    Luna::Layer* layer = layerItr->second;
    layer->RemoveNameChangingListener( SceneNodeChangeSignature::Delegate ( this, &LayerGrid::NameChanging ) );
    layer->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate ( this, &LayerGrid::NameChanged ) );
  }

  m_Layers.clear();
  m_Grid->RemoveAllRows();
}

///////////////////////////////////////////////////////////////////////////////
// Enables/disables the toolbar buttons.
// 
void LayerGrid::UpdateToolBarButtons()
{
  // Enable/disable everything in the toolbar based on whether or not we have
  // a valid scene.
  for ( i32 toolId = ID_NewLayer; toolId < ID_COUNT; ++toolId )
  {
    m_ToolBar->EnableTool( toolId, m_Scene != NULL );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds undoable commands to m_Scene's undo queue which will do one of two thing:
// 
// Either...
// 1. Adds the currently selected scene items to the currently highlighted
// layers.  Set addToLayer to true for this option.
// or...
// 2. Removes the currently selected scene items from the currently highlighted
// layers.  Set addToLayer to false for this option.
//
// If there are no selected items, or no selected layers, nothing happens.  It 
// is also safe to remove items from a layer even if they are not part of the
// layer (nothing happens), or add items to a layer that already belong to
// that layer (again, nothing happens).
// 
void LayerGrid::LayerSelectedItems( bool addToLayer )
{
  NOC_ASSERT( m_Scene );

  // Decide whether we are adding the selected scene items to the highlighted
  // layers, or removing the items from the layers.
  const DependencyCommand::DependencyAction action = addToLayer ? DependencyCommand::Connect : DependencyCommand::Disconnect;

  // If there are selected nodes in the scene, and selected rows in this control...
  const OS_SelectableDumbPtr& selectedNodes = m_Scene->GetSelection().GetItems();
  std::set< u32 > selectedRows = m_Grid->GetSelectedRows();
  if ( selectedNodes.Size() > 0 && selectedRows.size() > 0 )
  {
    //Log::Debug( "LayerSelectedItems\n" );
    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    OS_SelectableDumbPtr::Iterator nodeItr = selectedNodes.Begin();
    OS_SelectableDumbPtr::Iterator nodeEnd = selectedNodes.End();
    std::set< u32 >::const_iterator rowItr;
    const std::set< u32 >::const_iterator rowEnd = selectedRows.end();
    const M_LayerDumbPtr::const_iterator layerEnd = m_Layers.end();

    // For each node in the scene's selection list...
    for ( ; nodeItr != nodeEnd; ++nodeItr )
    {
      // ensure that we are not trying to add one layer to another layer
      {
        Layer* layerTest = Reflect::ObjectCast< Layer >( *nodeItr );
        if( layerTest )
        {
          continue;
        }
      }

      //Check the current selection
      if(IsSelectableValid(*nodeItr) == false)
      {
        //Invalid or incompatible
        continue;
      }

      Luna::SceneNode* node = Reflect::ObjectCast< Luna::SceneNode >( *nodeItr );
      if ( node )
      {
        // For each row that is highlighted...
        for ( rowItr = selectedRows.begin(); rowItr != rowEnd; ++rowItr )
        {
          // Find the layer that goes with the highlighted row
          M_LayerDumbPtr::iterator layerItr = m_Layers.find( m_Grid->GetRowName( *rowItr ) );
          if ( layerItr != layerEnd )
          {
            // Check to see if the node is already in the current layer...
            Luna::Layer* layer = layerItr->second;
            S_SceneNodeSmartPtr::const_iterator foundDescendant = layer->GetDescendants().find( node );

            // If the node is already in this layer, and we are suppose to be adding the node to the layer,
            // just skip the command (doCommand = false).  If the node is not in this layer, and we are
            // suppose to be removing the node from the layer, skip the command as well.  Otherwise, go 
            // ahead and carry out the command (doCommand = true).
            const bool doCommand = addToLayer ? ( foundDescendant == layer->GetDescendants().end() ) : ( foundDescendant != layer->GetDescendants().end() );
            if ( doCommand )
            {
              // Finally make an undoable command to add/remove the node to/from the layer
              batch->Push( new DependencyCommand( action, layer, node ) );
              //Log::Debug( "\t\t%s node %s %s layer %s [row=%d]\n", addToLayer ? "Added" : "Removed", node->GetName().c_str(), addToLayer ? "to" : "from", layer->GetName().c_str(), *rowItr );
            }
            else
            {
              //Log::Debug( "\t\tNode %s was already a member of layer %s [row=%d]\n", node->GetName().c_str(), layer->GetName().c_str(), *rowItr );
            }
          }
          else
          {
            // Something is wrong.  The rows that are selected in the grid do not correspond to
            // items in our list of layers (m_Layers).  Somehow those lists got out of sync.
            Log::Error( TXT( "Unable to add selection to layer [row=%d] because it doesn't exist\n" ), *rowItr );
            NOC_BREAK();
          }
        }
      }
    }

    //Log::Debug( "\n" );
    if( !batch->IsEmpty() )
    {
      m_Scene->Push( batch );
      m_Scene->Execute( false );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// This is a handy function for use while debugging.  It prints the currently
// selected layers to the output window.
// 
void LayerGrid::DebugDumpSelection()
{
#ifdef _DEBUG
  Log::Debug( TXT( "Dumping grid selection.\n" ) );
  std::set< u32 > selection = m_Grid->GetSelectedRows();
  const size_t numSelected = selection.size();
  if ( numSelected == 0 )
  {
    Log::Debug( TXT( "\tNo items are selected.\n" ) );
  }
  else
  {
    Log::Debug( TXT( "\t%d item%s selected.\n" ), numSelected, ( numSelected == 1 ) ? TXT( "" ) : TXT( "s" ) );
    std::set< u32 >::const_iterator rowItr = selection.begin();
    std::set< u32 >::const_iterator rowEnd = selection.end();
    for ( ; rowItr != rowEnd; ++rowItr )
    {
      const tstring& name = m_Grid->GetRowName( *rowItr );
      Log::Debug( TXT( "\t\t%s\n" ), name.c_str() );
    }
  }
  Log::Debug( TXT( "\n" ) );
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "New Layer" button is clicked on the toolbar.  Creates an
// un-doable command that adds a layer to the scene (thereby adding it to this
// control).
// 
void LayerGrid::OnNewLayer( wxCommandEvent& event )
{
  if ( m_Scene )
  {
        Luna::Layer* layer = new Luna::Layer( m_Scene, new Content::Layer() );
        m_Scene->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, m_Scene, layer ) );
    m_Scene->Execute( false ); 
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "New Layer From Selection" button is clicked on the toolbar.
// 
void LayerGrid::OnNewLayerFromSelection( wxCommandEvent& dummyEvt )
{
  if ( m_Scene )
  {    
    if(IsSelectionValid() == false)
    {
      return;
    }
   
    Undo::BatchCommandPtr batch       = new Undo::BatchCommand ();
        Luna::Layer* layer = new Luna::Layer( m_Scene, new Content::Layer() );

    // Generate a name for this layer
        GenerateLayerName(layer);

        batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, m_Scene, layer ) );

    // Step 2: add all the selected items to the layer
    const OS_SelectableDumbPtr& selection = m_Scene->GetSelection().GetItems();
    OS_SelectableDumbPtr::Iterator itr = selection.Begin();
    OS_SelectableDumbPtr::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
      //If the element is a supported type
      if(IsSelectableValid(*itr))
      {
        Luna::SceneNode* node = Reflect::ObjectCast< Luna::SceneNode >( *itr );
        if ( node )
        {
                    batch->Push( new DependencyCommand( DependencyCommand::Connect, layer, node ) );
        }
      }
    }

    m_Scene->Push( batch );
    m_Scene->Execute( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "Delete Layer" button is clicked on the toolbar.  If any
// layers are selected in this control, they are deleted from the scene (and
// thereby deleted from this control as well).  This adds an undoable operation
// to the scene's undo queue.
// 
void LayerGrid::OnDeleteLayer( wxCommandEvent& event_ )
{
  DeleteSelectedLayers();
}

///////////////////////////////////////////////////////////////////////////////
//
void LayerGrid::DeleteSelectedLayers()
{
  // If anything selected in the grid
  if ( m_Scene && m_Grid->IsAnythingSelected() )
  {
    LayerSelectedItems( false );

    // Begin undo batch
    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    // Get an ordered list of the selected rows, and traverse the list in reverse order.
    // This makes sure that removing an item doesn't change the row number of another
    // item that will be removed later in the loop.  If we don't do this, we run the
    // risk of invalidating the selection array as we iterate over it.
    const std::set< u32 >& selection = m_Grid->GetSelectedRows();
    std::set< u32 >::const_reverse_iterator rowItr = selection.rbegin();
    std::set< u32 >::const_reverse_iterator rowEnd = selection.rend();
    for ( ; rowItr != rowEnd; ++rowItr )
    {
      M_LayerDumbPtr::iterator layerItr = m_Layers.find( m_Grid->GetRowName( *rowItr ) );
      NOC_ASSERT( layerItr != m_Layers.end() );
      // NOTE: m_Layers is changing as we iterate over the selection (items are being 
      // removed via callbacks), so don't hold on to any iterators that point into the list.  
      // Recalculate m_Layers.end() each time through the loop.
      if ( layerItr != m_Layers.end() )
      {
        Luna::Layer* layer = layerItr->second;

        // If the layer that we are about to delete is in the scene's selection list,
        // we had better just clear out the selection list (otherwise the attribute
        // editor will still be showing a layer that is no longer in the scene).  This
        // has to be done before actually deleting the layer.
        if ( m_Scene->GetSelection().Contains( layer ) )
        {
          batch->Push( m_Scene->GetSelection().Clear() );
        }
        
        // If the layer has any members, we should remove them before removing the layer,
        // otherwise if those members are deleted, they will be pointing to an invalid
        // layer that will eventually be Disconnected.
        Luna::S_SceneNodeSmartPtr descendents = layer->GetDescendants();
        for ( Luna::S_SceneNodeSmartPtr::iterator itr = descendents.begin(), end = descendents.end(); itr != end; ++itr )
        {
          batch->Push( new DependencyCommand( DependencyCommand::Disconnect, layer, *itr ) );
        }

        // Push the command to delete the layer
        batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, m_Scene, layer ) );
      }
    }

    // End undo batch
    m_Scene->Push( batch );

    m_Scene->Execute( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "Add Selection To Layer" button is clicked on the toolbar.
// Adds all of the currently selected scene nodes to each of the currently
// highlighted layers.  This operation adds an undoable command to the
// scene's undo queue.
// 
void LayerGrid::OnAddSelectionToLayer( wxCommandEvent& event )
{
  if ( m_Scene )
  {
    LayerSelectedItems( true );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "Remove Selection From Layer" button is clicked on the toolbar.
// Removes all the currently selected scene nodes from the currently hightlighted
// layers (if there is any of either).  This operation adds an undoable command
// to the scene's undo queue.
// 
void LayerGrid::OnRemoveSelectionFromLayer( wxCommandEvent& event )
{
  if ( m_Scene )
  {
    LayerSelectedItems( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "Select Layer Members" button is clicked on the toolbar.
// 
void LayerGrid::OnSelectLayerMembers( wxCommandEvent& event )
{
  if ( m_Scene )
  {
    OS_SelectableDumbPtr newSelection;
    M_LayerDumbPtr::const_iterator layerItr = m_Layers.begin();
    M_LayerDumbPtr::const_iterator layerEnd = m_Layers.end();
    for ( ; layerItr != layerEnd; ++layerItr )
    {
      Luna::Layer* layer = layerItr->second;
      if ( m_Grid->IsSelected( layer->GetName() ) )
      {
        S_SceneNodeSmartPtr::const_iterator dependItr = layer->GetDescendants().begin();
        S_SceneNodeSmartPtr::const_iterator dependEnd = layer->GetDescendants().end();
        for ( ; dependItr != dependEnd; ++dependItr )
        {
          newSelection.Append( *dependItr );
        }
      }
    }

    m_Scene->Push( m_Scene->GetSelection().SetItems( newSelection ) );
    m_Scene->Execute( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Called when the "Select Layer" button is clicked on the toolbar.  Goes 
// through all the layers that are currently selected in this control and
// adds them to the scene's selection list (implicity this is an undoable
// operation).
// 
void LayerGrid::OnSelectLayer( wxCommandEvent& event )
{
  if ( m_Scene )
  {
    OS_SelectableDumbPtr newSelection;

    const std::set< u32 >& selection = m_Grid->GetSelectedRows();
    std::set< u32 >::const_iterator rowItr = selection.begin();
    std::set< u32 >::const_iterator rowEnd = selection.end();
    for ( ; rowItr != rowEnd; ++rowItr )
    {
      M_LayerDumbPtr::iterator layerItr = m_Layers.find( m_Grid->GetRowName( *rowItr ) );
      NOC_ASSERT( layerItr != m_Layers.end() );
      Luna::Layer* layer = layerItr->second;
      newSelection.Append( layer );
    }

    m_Scene->Push( m_Scene->GetSelection().SetItems( newSelection ) );
    m_Scene->Execute( false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the scene's selection changes (as opposed to the selection
// within the layer grid).  Checks the selection to see if any of our layers
// are contained within.  If any of the layers managed by this grid are within
// the selection, the grid is updated to make sure that the appropriate rows 
// are selected.  This mainly facilitates undo/redo of selection so that this
// control shows the same layers as selected as other controls in the scene.
// 
void LayerGrid::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
  if ( selection.Size() > 0 )
  {
    u32 numLayersInSelection = 0;
    OS_SelectableDumbPtr::Iterator itr = selection.Begin();
    OS_SelectableDumbPtr::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
      Luna::Layer* lunaLayer = Reflect::ObjectCast< Luna::Layer >( *itr );
      if ( lunaLayer )
      {
        Content::Layer* contentLayer = lunaLayer->GetPackage<Content::Layer>();
          NOC_ASSERT( m_Layers.find( lunaLayer->GetName() ) != m_Layers.end() );

          ++numLayersInSelection;
          // If this is the first layer that we found in the selection list...
          if ( numLayersInSelection == 1 )
          {
            // Clear the grid's selection as soon as we find a layer contained
            // in the selection list.  The grid's selection will be rebuilt as
            // we continue to iterate over the selection list.
            m_Grid->DeselectAllRows();
          }

          i32 row = m_Grid->GetRowNumber( lunaLayer->GetName() );
          NOC_ASSERT( row >= 0 );
          if ( row >= 0 )
          {
            m_Grid->SelectRow( row, true );
          }
        }
      }
    }
  }

///////////////////////////////////////////////////////////////////////////////
// Callback just before a layer's name is changed.  Stores the layer and the
// current name of the layer for processing in NameChanged.
// 
void LayerGrid::NameChanging( const SceneNodeChangeArgs& args )
{
  M_LayerDumbPtr::const_iterator layerItr = m_Layers.find( args.m_Node->GetName() );
  if ( layerItr != m_Layers.end() )
  {
    Luna::Layer* layer = layerItr->second;
    const tstring& name = layerItr->first;
    if ( args.m_Node != layer || layer->GetName() != name )
    {
      Log::Error( TXT( "Layer in list (named %s), does not match layer named %s.\n" ), name.c_str(), args.m_Node->GetName().c_str() );
      NOC_BREAK();
    }
    m_NameChangeInfo.m_Layer = layer;
    m_NameChangeInfo.m_OldName = name;
  }
  else
  {
    Log::Error( TXT( "Layer named %s is not in the grid.\n" ), args.m_Node->GetName().c_str() );
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a layer's name changes.  Uses the old name of the layer
// that we stashed to earlier to update our layer list and the row label in 
// the grid.
// 
void LayerGrid::NameChanged( const SceneNodeChangeArgs& args )
{
  const tstring& oldName = m_NameChangeInfo.m_OldName;
  M_LayerDumbPtr::iterator layerItr = m_Layers.find( oldName );
  if ( layerItr != m_Layers.end() )
  {
    Luna::Layer* layer = layerItr->second;
    const tstring& newName = args.m_Node->GetName();
    m_Layers.erase( layerItr );
    m_Layers.insert( M_LayerDumbPtr::value_type( newName, layer ) );

    bool nameUpdated = m_Grid->SetRowName( oldName, newName );
    NOC_ASSERT( nameUpdated );
  }
  else
  {
    Log::Error( TXT( "Layer named %s is not in the grid.\n" ), m_NameChangeInfo.m_OldName.c_str() );
    NOC_BREAK();
  }

  m_NameChangeInfo.Clear();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a row's visibility checkbox is changed in the grid.  
// Changes the visibility member of the layer to match the row that was changed.
// 
void LayerGrid::LayerVisibleChanged( const GridRowChangeArgs& args )
{
  const tstring& name = m_Grid->GetRowName( args.m_RowNumber );
  M_LayerDumbPtr::const_iterator layerItr = m_Layers.find( name );
  if ( layerItr != m_Layers.end() )
  {
    Luna::Layer* layer = layerItr->second;
    layer->SetVisible( m_Grid->IsRowVisibleChecked( args.m_RowNumber ) );
    layer->GetScene()->Execute( false );
  }
  else
  {
    Log::Error( TXT( "LayerVisibleChanged - layer named %s not found\n" ), name.c_str() );
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a row's selectability checkbox is changed in the grid.
// Changes the selectability member of the layer to match the row that was
// changed.
// 
void LayerGrid::LayerSelectableChanged( const GridRowChangeArgs& args )
{
  const tstring& name = m_Grid->GetRowName( args.m_RowNumber );
  M_LayerDumbPtr::const_iterator layerItr = m_Layers.find( name );
  if ( layerItr != m_Layers.end() )
  {
    Luna::Layer* layer = layerItr->second;
    bool selectable = m_Grid->IsRowSelectableChecked( args.m_RowNumber );

    layer->SetSelectable( selectable );

    if (!selectable)
    {
      OS_SelectableDumbPtr newSelection;

      OS_SelectableDumbPtr selection = layer->GetScene()->GetSelection().GetItems();
      OS_SelectableDumbPtr::Iterator itr = selection.Begin();
      OS_SelectableDumbPtr::Iterator end = selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::SceneNode* node = Reflect::ObjectCast<Luna::SceneNode>( *itr );

        if (!node || !layer->ContainsMember( node ))
        {
          newSelection.Append(*itr);
        }
      }

      if (newSelection.Size() != selection.Size())
      {
        layer->GetScene()->GetSelection().SetItems( newSelection );
      }
    }

    layer->GetScene()->Execute( false );
  }
  else
  {
    Log::Error( TXT( "LayerSelectableChanged - layer named %s not found\n" ), name.c_str() );
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a row is renamed by the user through the layer grid UI.
// Renames the corresponding layer node in the scene.
// 
void LayerGrid::RowRenamed( const GridRowRenamedArgs& args )
{
  M_LayerDumbPtr::iterator found = m_Layers.find( args.m_OldName );
  if ( found != m_Layers.end() )
  {
    Luna::Layer* layer = found->second;
    layer->GetScene()->Push( new Undo::PropertyCommand< tstring >( new Nocturnal::MemberProperty< Luna::Layer, tstring >( layer, &Luna::Layer::GetName, &Luna::Layer::SetGivenName ), args.m_NewName ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is about to change.  Clears out all the
// layers and disconnect all the scene listeners.
// 
void LayerGrid::CurrentSceneChanging( const SceneChangeArgs& args )
{
  if ( args.m_Scene == m_Scene )
  {
    DisconnectSceneListeners();
    RemoveAllLayers();
    m_Scene = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene has been changed.  Connects the listeners
// to the scene and updates the UI elements.  The control is not automatically
// populated with the current scene's layers, so that falls to the owner of 
// this control.  However, any additionally added layers will automatically
// show up in the UI.
// 
void LayerGrid::CurrentSceneChanged( const SceneChangeArgs& args )
{
  if ( args.m_Scene != m_Scene )
  {
    m_Scene = args.m_Scene;
    UpdateToolBarButtons();
    ConnectSceneListeners();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a layer has been added to the scene.  Adds a layer to
// the UI.
// 
void LayerGrid::NodeAdded( const NodeChangeArgs& args )
{
  if ( args.m_Node->HasType( Reflect::GetType<Luna::Layer>() ) )
  {
    AddLayer( Reflect::DangerousCast< Luna::Layer >( args.m_Node ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a layer has been removed from the scene.  Removes the
// corresponding layer from the UI.
// 
void LayerGrid::SceneNodeRemoved( const NodeChangeArgs& args )
{
  if ( args.m_Node->HasType( Reflect::GetType<Luna::Layer>() ) )
  {
    RemoveLayer( Reflect::DangerousCast< Luna::Layer >( args.m_Node ) );
  }
}

#include "Precompile.h"
#include "TypeGrid.h"
#include "GridWithIcons.h"
#include "HierarchyNodeType.h"
#include "Scene.h"
#include "SceneEditorIDs.h"
#include "SceneManager.h"
#include "Foundation/Log.h"

// Using
using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
TypeGrid::TypeGrid( wxWindow* parent, Luna::SceneManager* sceneManager )
: m_SceneManager( sceneManager )
, m_Panel( new wxPanel( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER, "TypeGrid Panel" ) )
, m_Grid( new GridWithIcons( m_Panel, SceneEditorIDs::ID_ObjectGrid ) )
, m_Scene( NULL )
{
  // Add everything to a top level sizer
  wxBoxSizer* mainSizer = new wxBoxSizer( wxVERTICAL );
  mainSizer->Add( m_Grid->GetPanel(), 1, wxEXPAND );
  mainSizer->Layout();
  m_Panel->SetAutoLayout( true );
  m_Panel->SetSizer( mainSizer );
  mainSizer->SetSizeHints( m_Panel );

  m_SceneManager->AddCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &TypeGrid::CurrentSceneChanging ) );
  m_SceneManager->AddCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &TypeGrid::CurrentSceneChanged ) );
  m_Grid->AddRowVisibilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypeGrid::VisibilityChanged ) );
  m_Grid->AddRowSelectabilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypeGrid::SelectabilityChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TypeGrid::~TypeGrid()
{
  if ( m_Panel->GetParent() == NULL )
  {
    delete m_Panel;
    m_Panel = NULL;
  }

  m_SceneManager->RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &TypeGrid::CurrentSceneChanging ) );
  m_SceneManager->RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &TypeGrid::CurrentSceneChanged ) );
  m_Grid->RemoveRowVisibilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypeGrid::VisibilityChanged ) );
  m_Grid->RemoveRowSelectabilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypeGrid::SelectabilityChanged ) );
  delete m_Grid;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the top-level panel that houses all the controls that belong to
// this class.  This panel can be nested inside another panel, or added to
// the main frame of the application.
// 
wxPanel* TypeGrid::GetPanel() const
{
  return m_Panel;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a specific node type to be displayed in the grid.
// 
bool TypeGrid::AddType( Luna::HierarchyNodeType* type )
{
  m_NodeTypes.insert( M_HierarchyNodeTypeDumbPtr::value_type( type->GetName(), type ) );
  return m_Grid->AddRow( type->GetName(), type->IsVisible(), type->IsSelectable(), type->GetImageIndex() );
}

///////////////////////////////////////////////////////////////////////////////
// Removes a node type that is being displayed in the grid.
// 
bool TypeGrid::RemoveType( Luna::HierarchyNodeType* type )
{
  m_NodeTypes.erase( type->GetName() );
  return m_Grid->RemoveRow( type->GetName() );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a specific node type to be displayed in the grid.
//  callback from scene
void TypeGrid::AddNodeType( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Luna::HierarchyNodeType>() ) )
  {
    AddType( Reflect::DangerousCast< Luna::HierarchyNodeType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes a node type that is being displayed in the grid.
//  callback from scene
void TypeGrid::RemoveNodeType( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Luna::HierarchyNodeType>() ) )
  {
    RemoveType( Reflect::DangerousCast< Luna::HierarchyNodeType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the visibility checkbox for a particular item is clicked
// on.  Changes the corresponding node type's visibility to match the change
// in the UI.
// 
void TypeGrid::VisibilityChanged( const GridRowChangeArgs& args )
{
  const std::string& typeName = m_Grid->GetRowName( args.m_RowNumber );
  M_HierarchyNodeTypeDumbPtr::const_iterator typeItr = m_NodeTypes.find( typeName );
  if ( typeItr != m_NodeTypes.end() )
  {
    Luna::HierarchyNodeType* nodeType = typeItr->second;
    nodeType->SetVisible( m_Grid->IsRowVisibleChecked( args.m_RowNumber ) );
    m_Scene->Execute( false );
  }
  else
  {
    Log::Error( "Unable to change visible property - Node type (%s) was not in list.\n", typeName.c_str() );
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the selectability checkbox for a particular item is clicked
// on.  Changes the corresponding node type's selectability to match the change
// in the UI.
// 
void TypeGrid::SelectabilityChanged( const GridRowChangeArgs& args )
{
  const std::string& typeName = m_Grid->GetRowName( args.m_RowNumber );
  M_HierarchyNodeTypeDumbPtr::const_iterator typeItr = m_NodeTypes.find( typeName );
  if ( typeItr != m_NodeTypes.end() )
  {
    Luna::HierarchyNodeType* nodeType = typeItr->second;
    bool selectable = m_Grid->IsRowSelectableChecked( args.m_RowNumber );

    nodeType->SetSelectable( selectable );
    
    if (!selectable)
    {
      OS_SelectableDumbPtr newSelection;

      OS_SelectableDumbPtr selection = m_Scene->GetSelection().GetItems();
      OS_SelectableDumbPtr::Iterator itr = selection.Begin();
      OS_SelectableDumbPtr::Iterator end = selection.End();
      for ( ; itr != end; ++itr )
      {
        Luna::HierarchyNode* node = Reflect::ObjectCast<Luna::HierarchyNode>( *itr );

        if (!node || node->GetNodeType() != nodeType)
        {
          newSelection.Append(*itr);
        }
      }

      if (newSelection.Size() != selection.Size())
      {
        m_Scene->GetSelection().SetItems( newSelection );
      }
    }

    m_Scene->Execute( false );
  }
  else
  {
    Log::Error( "Unable to change selectable property - Node type (%s) was not in list.\n", typeName.c_str() );
    NOC_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is about to change.  Clears out the
// grid and sets the internal scene member to the new scene.
// 
void TypeGrid::CurrentSceneChanging( const SceneChangeArgs& args )
{
  m_NodeTypes.clear();
  m_Grid->RemoveAllRows();

  if (args.m_Scene)
  {
    args.m_Scene->RemoveNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate (this, &TypeGrid::AddNodeType) );
    args.m_Scene->RemoveNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate (this, &TypeGrid::RemoveNodeType) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is about to change.  Clears out the
// grid and sets the internal scene member to the new scene.
// 
void TypeGrid::CurrentSceneChanged( const SceneChangeArgs& args )
{
  if ( args.m_Scene != m_Scene )
  {
    m_Scene = args.m_Scene;
  }

  if ( args.m_Scene )
  {
    args.m_Scene->AddNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate (this, &TypeGrid::AddNodeType) );
    args.m_Scene->AddNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate (this, &TypeGrid::RemoveNodeType) );
  }
}

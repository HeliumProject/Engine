#include "Precompile.h"

#include "TypesPanel.h"

#include "EditorIDs.h"

using namespace Helium;
using namespace Helium::Editor;

TypesPanel::TypesPanel( SceneManager* manager, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: TypesPanelGenerated( parent, id, pos, size, style )
, m_SceneManager( manager )
, m_Grid( new GridWithIcons( this, EventIds::ID_ObjectGrid ) )
, m_Scene( NULL )
{
  GetSizer()->Add( m_Grid->GetPanel(), 1, wxEXPAND );
  Layout();

  m_SceneManager->AddCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &TypesPanel::CurrentSceneChanging ) );
  m_SceneManager->AddCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &TypesPanel::CurrentSceneChanged ) );
  m_Grid->AddRowVisibilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypesPanel::VisibilityChanged ) );
  m_Grid->AddRowSelectabilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypesPanel::SelectabilityChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
TypesPanel::~TypesPanel()
{
  m_SceneManager->RemoveCurrentSceneChangingListener( SceneChangeSignature::Delegate ( this, &TypesPanel::CurrentSceneChanging ) );
  m_SceneManager->RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &TypesPanel::CurrentSceneChanged ) );
  m_Grid->RemoveRowVisibilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypesPanel::VisibilityChanged ) );
  m_Grid->RemoveRowSelectabilityChangedListener( GridRowChangeSignature::Delegate ( this, &TypesPanel::SelectabilityChanged ) );
  delete m_Grid;
}

///////////////////////////////////////////////////////////////////////////////
// Adds a specific node type to be displayed in the grid.
// 
bool TypesPanel::AddType( Editor::HierarchyNodeType* type )
{
  m_NodeTypes.insert( M_HierarchyNodeTypeDumbPtr::value_type( type->GetName(), type ) );
  return m_Grid->AddRow( type->GetName(), type->IsVisible(), type->IsSelectable(), type->GetImageIndex() );
}

///////////////////////////////////////////////////////////////////////////////
// Removes a node type that is being displayed in the grid.
// 
bool TypesPanel::RemoveType( Editor::HierarchyNodeType* type )
{
  m_NodeTypes.erase( type->GetName() );
  return m_Grid->RemoveRow( type->GetName() );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a specific node type to be displayed in the grid.
//  callback from scene
void TypesPanel::AddNodeType( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Editor::HierarchyNodeType>() ) )
  {
    AddType( Reflect::DangerousCast< Editor::HierarchyNodeType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Removes a node type that is being displayed in the grid.
//  callback from scene
void TypesPanel::RemoveNodeType( const NodeTypeExistenceArgs& args )
{
  if ( args.m_NodeType->HasType( Reflect::GetType<Editor::HierarchyNodeType>() ) )
  {
    RemoveType( Reflect::DangerousCast< Editor::HierarchyNodeType >( args.m_NodeType ) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the visibility checkbox for a particular item is clicked
// on.  Changes the corresponding node type's visibility to match the change
// in the UI.
// 
void TypesPanel::VisibilityChanged( const GridRowChangeArgs& args )
{
  const tstring& typeName = m_Grid->GetRowName( args.m_RowNumber );
  M_HierarchyNodeTypeDumbPtr::const_iterator typeItr = m_NodeTypes.find( typeName );
  if ( typeItr != m_NodeTypes.end() )
  {
    Editor::HierarchyNodeType* nodeType = typeItr->second;
    nodeType->SetVisible( m_Grid->IsRowVisibleChecked( args.m_RowNumber ) );
    m_Scene->Execute( false );
  }
  else
  {
    Log::Error( TXT( "Unable to change visible property - Node type (%s) was not in list.\n" ), typeName.c_str() );
    HELIUM_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the selectability checkbox for a particular item is clicked
// on.  Changes the corresponding node type's selectability to match the change
// in the UI.
// 
void TypesPanel::SelectabilityChanged( const GridRowChangeArgs& args )
{
  const tstring& typeName = m_Grid->GetRowName( args.m_RowNumber );
  M_HierarchyNodeTypeDumbPtr::const_iterator typeItr = m_NodeTypes.find( typeName );
  if ( typeItr != m_NodeTypes.end() )
  {
    Editor::HierarchyNodeType* nodeType = typeItr->second;
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
        Editor::HierarchyNode* node = Reflect::ObjectCast<Editor::HierarchyNode>( *itr );

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
    Log::Error( TXT( "Unable to change selectable property - Node type (%s) was not in list.\n" ), typeName.c_str() );
    HELIUM_BREAK();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is about to change.  Clears out the
// grid and sets the internal scene member to the new scene.
// 
void TypesPanel::CurrentSceneChanging( const SceneChangeArgs& args )
{
  m_NodeTypes.clear();
  m_Grid->RemoveAllRows();

  if (args.m_Scene)
  {
    args.m_Scene->RemoveNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate (this, &TypesPanel::AddNodeType) );
    args.m_Scene->RemoveNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate (this, &TypesPanel::RemoveNodeType) );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the current scene is about to change.  Clears out the
// grid and sets the internal scene member to the new scene.
// 
void TypesPanel::CurrentSceneChanged( const SceneChangeArgs& args )
{
  if ( args.m_Scene != m_Scene )
  {
    m_Scene = args.m_Scene;
  }

  if ( args.m_Scene )
  {
    args.m_Scene->AddNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate (this, &TypesPanel::AddNodeType) );
    args.m_Scene->AddNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate (this, &TypesPanel::RemoveNodeType) );
  }
}

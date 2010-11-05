#include "Precompile.h"

#include "DirectoryPanel.h"

#include "MainFrame.h"
#include "Pipeline/SceneGraph/SceneManager.h"
#include "EditorIDs.h"

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::SceneGraph;

DirectoryPanel::DirectoryPanel( SceneManager* manager, TreeMonitor* treeMonitor, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: DirectoryPanelGenerated( parent, id, pos, size, style )
, m_SceneManager( manager )
, m_TreeMonitor( treeMonitor )
{
    SetHelpText( TXT( "This is the directory panel, you can explore the hierarchy of your scene here." ) );

#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    // Hierarchy
    m_HierarchyOutline = new HierarchyOutliner( m_SceneManager );
    SortTreeCtrl* hierarchyTree = m_HierarchyOutline->InitTreeCtrl( m_HierarchyPanel, wxID_ANY );
    hierarchyTree->SetHelpText( TXT( "This is the hierarchy tree, it allows you to interact with the scene hierarchy." ) );
    hierarchyTree->SetImageList( GlobalFileIconsTable().GetSmallImageList() );
    m_HierarchyPanel->GetSizer()->Add( hierarchyTree, 1, wxEXPAND );
    m_TreeMonitor->AddTree( hierarchyTree );

    // Entities
    m_EntityOutline = new EntityAssetOutliner( m_SceneManager );
    SortTreeCtrl* entityTree = m_EntityOutline->InitTreeCtrl( m_EntitiesPanel, wxID_ANY );
    entityTree->SetHelpText( TXT( "This is the entity tree, it allows you to see all the entities placed in the scene." ) );
    entityTree->SetImageList( GlobalFileIconsTable().GetSmallImageList() );
    m_EntitiesPanel->GetSizer()->Add( entityTree, 1, wxEXPAND );
    m_TreeMonitor->AddTree( entityTree );

    // Types
    m_TypeOutline = new NodeTypeOutliner( m_SceneManager );
    SortTreeCtrl* typeTree = m_TypeOutline->InitTreeCtrl( m_TypesPanel, EventIds::ID_TypeOutlineControl );
    typeTree->SetHelpText( TXT( "This is the type tree, it allows you to see all the different types in the scene." ) );
    typeTree->SetImageList( GlobalFileIconsTable().GetSmallImageList() );
    m_TypesPanel->GetSizer()->Add( typeTree, 1, wxEXPAND );
    m_TreeMonitor->AddTree( typeTree );
#endif

}

DirectoryPanel::~DirectoryPanel()
{
#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    delete m_TypeOutline;
    delete m_EntityOutline;
    delete m_HierarchyOutline;
#endif
}

void DirectoryPanel::SaveState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState )
{
#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    m_TypeOutline->SaveState( typesState );
    m_EntityOutline->SaveState( entityState );
    m_HierarchyOutline->SaveState( hierarchyState );
#endif
}

void DirectoryPanel::RestoreState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState )
{
#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    m_TypeOutline->RestoreState( typesState );
    m_EntityOutline->RestoreState( entityState );
    m_HierarchyOutline->RestoreState( hierarchyState );
#endif
}
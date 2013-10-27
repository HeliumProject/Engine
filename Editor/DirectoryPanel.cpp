#include "EditorPch.h"

#include "DirectoryPanel.h"

#include "MainFrame.h"
#include "EditorScene/SceneManager.h"
#include "EditorIDs.h"

using namespace Helium;
using namespace Helium::Editor;

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
#endif

}

DirectoryPanel::~DirectoryPanel()
{
#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    delete m_HierarchyOutline;
#endif
}

void DirectoryPanel::SaveState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState )
{
#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    m_HierarchyOutline->SaveState( hierarchyState );
#endif
}

void DirectoryPanel::RestoreState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState )
{
#ifndef EDITOR_SCENE_DISABLE_OUTLINERS
    m_HierarchyOutline->RestoreState( hierarchyState );
#endif
}
#include "Precompile.h"

#include "DirectoryPanel.h"

#include "MainFrame.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneEditorIDs.h"

using namespace Luna;

DirectoryPanel::DirectoryPanel( SceneManager* manager, TreeMonitor* treeMonitor, wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: DirectoryPanelGenerated( parent, id, pos, size, style )
, m_SceneManager( manager )
, m_TreeMonitor( treeMonitor )
{
#ifndef LUNA_SCENE_DISABLE_OUTLINERS
    // Hierarchy
    m_HierarchyOutline = new HierarchyOutliner( m_SceneManager );
    SortTreeCtrl* hierarchyTree = m_HierarchyOutline->InitTreeCtrl( m_HierarchyPanel, wxID_ANY );
    hierarchyTree->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList() );
    m_HierarchyPanel->GetSizer()->Add( hierarchyTree, 1, wxEXPAND );
    m_TreeMonitor->AddTree( hierarchyTree );

    // Entities
    m_EntityOutline = new EntityAssetOutliner( m_SceneManager );
    SortTreeCtrl* entityTree = m_EntityOutline->InitTreeCtrl( m_EntitiesPanel, wxID_ANY );
    entityTree->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList() );
    m_EntitiesPanel->GetSizer()->Add( entityTree, 1, wxEXPAND );
    m_TreeMonitor->AddTree( entityTree );

    // Types
    m_TypeOutline = new NodeTypeOutliner( m_SceneManager );
    SortTreeCtrl* typeTree = m_TypeOutline->InitTreeCtrl( m_TypesPanel, SceneEditorIDs::ID_TypeOutlineControl );
    typeTree->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList() );
    m_TypesPanel->GetSizer()->Add( typeTree, 1, wxEXPAND );
    m_TreeMonitor->AddTree( typeTree );
#endif

}

DirectoryPanel::~DirectoryPanel()
{
#ifndef LUNA_SCENE_DISABLE_OUTLINERS
    delete m_TypeOutline;
    delete m_EntityOutline;
    delete m_HierarchyOutline;
#endif
}


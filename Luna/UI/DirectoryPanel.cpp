#include "Precompile.h"

#include "DirectoryPanel.h"

#include "MainFrame.h"
#include "Scene/SceneManager.h"
#include "Scene/SceneEditorIDs.h"

using namespace Luna;

DirectoryPanel::DirectoryPanel( wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style )
: DirectoryPanelGenerated( parent, id, pos, size, style )
{
#ifndef LUNA_SCENE_DISABLE_OUTLINERS
#ifdef UI_REFACTOR
    // Hierarchy
    m_HierarchyOutline = new HierarchyOutliner( m_SceneManager );
    SortTreeCtrl* hierarchyTree = m_HierarchyOutline->InitTreeCtrl( this, wxID_ANY );
//    hierarchyTree->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList() );
    m_HierarchyPanel->AddChild( hierarchyTree );
    m_Owner->GetTreeMonitor().AddTree( hierarchyTree );

    // Entities
    m_EntityOutline = new EntityAssetOutliner( m_SceneManager );
    SortTreeCtrl* entityTree = m_EntityOutline->InitTreeCtrl( this, wxID_ANY );
//    entityTree->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList() );
    m_EntitiesPanel->AddChild( entityTree );
    m_Owner->GetTreeMonitor().AddTree( entityTree );

    // Types
    m_TypeOutline = new NodeTypeOutliner( m_SceneManager );
    SortTreeCtrl* typeTree = m_TypeOutline->InitTreeCtrl( this, SceneEditorIDs::ID_TypeOutlineControl );
//    typeTree->SetImageList( Nocturnal::GlobalFileIconsTable().GetSmallImageList() );
    m_TypesPanel->AddChild( typeTree );
    m_Owner->GetTreeMonitor().AddTree( typeTree );
#endif
#endif

}

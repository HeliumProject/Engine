#pragma once

#include "LunaGenerated.h"

#include "Scene/EntityAssetOutliner.h"
#include "Scene/HierarchyOutliner.h"
#include "Scene/NodeTypeOutliner.h"

namespace Luna
{
#ifdef UI_REFACTOR
    class MainFrame;
    class SceneManager;
#endif

    class DirectoryPanel : public DirectoryPanelGenerated
    {
    public:
   	    DirectoryPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

    private:
#ifdef UI_REFACTOR
        MainFrame* m_Owner;
        SceneManager* m_SceneManager;
#endif

        EntityAssetOutliner* m_EntityOutline;
        HierarchyOutliner*   m_HierarchyOutline;
        NodeTypeOutliner*    m_TypeOutline;
    };
}
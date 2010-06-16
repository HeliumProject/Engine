#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Group.h"
#include "wx/treebase.h"

namespace Inspect
{
  class APPLICATION_API PanelItemData : public wxTreeItemData
  {
  public:
    PanelItemData( Panel* panel )
    : wxTreeItemData()
    , m_Panel( panel )
    , m_IgnoreToggle( false )
    {
    }
    
    Panel* GetPanel() { return m_Panel; }
    
    void StartIgnoreToggle() { m_IgnoreToggle = true; }
    void EndIgnoreToggle() { m_IgnoreToggle = false; }
    bool GetIgnoreToggle() { return m_IgnoreToggle; }
    
  protected:
    Panel* m_Panel;
    bool m_IgnoreToggle;
  };
  
  const static char PANEL_ATTR_TEXT[] = "text";
  
  //
  // Contains other controls and distributes layout logic
  //

  class APPLICATION_API Panel : public Reflect::ConcreteInheritor<Panel, Group>
  {
  protected:
    // the item in the tree for this panel
    PanelItemData m_ItemData;

    // the title of the panel
    std::string m_Text;

    // the path of the panel
    std::string m_Path;

    // expanded state of the panel
    bool m_Expanded;

    // expandable state of the panel
    bool m_Expandable;

    // collapsable state of the panel
    bool m_Collapsable;

    // selected state of the panel
    bool m_Selected;

    // selectable state of the panel
    bool m_Selectable;

    // show the tree node (make the panel expanded if the tree node is not shown)
    bool m_ShowTreeNode;

    // should the controls be refreshed?
    bool m_RefreshControls;

    // do we own the window we are creating?
    bool m_OwnWindow;

  public:
    Panel();
    virtual ~Panel();

  protected:
    // process
    virtual bool Process(const std::string& key, const std::string& value);

  public:
    virtual wxWindow* GetContextWindow() NOC_OVERRIDE;

    virtual const wxWindow* GetContextWindow() const NOC_OVERRIDE;

    // cleanup
    virtual void Clear() NOC_OVERRIDE;
    
    // init
    virtual void Realize(Container* parent) NOC_OVERRIDE;
    
    virtual void UnRealize() NOC_OVERRIDE;

    virtual void SetEnabled(bool enabled) NOC_OVERRIDE;
    
    void RefreshControls();

    std::string GetPath();

  protected:
    // build the path of titles to this panel
    void BuildPath(std::string& path);
    
    void SetItemExpanded(bool expanded);
    
    wxTreeItemId GetParentTreeNode(Container* parent);

  public:
    // builds a list of expanded panels
    void GetExpanded(M_string& paths);

    // sets expansion from paths
    void SetExpanded(const M_string& paths);

    // return expanded state
    bool IsExpanded();
    
    // set expanded setting
    void SetExpanded(bool expanded, bool force = false);
    
    // set expanded setting recursively
    void SetExpandedRecursive(bool expanded, bool force = false);

    // return expandable state
    bool IsExpandable();

    // set expandable setting
    void SetExpandable(bool expandable);

    // return expandable state
    bool IsCollapsable();

    // set expandable setting
    void SetCollapsable(bool collapsable);
    
    // return tree node state
    bool GetShowTreeNode();
    
    // set tree node state
    void SetShowTreeNode(bool showTreeNode);

    // helper
    static PanelPtr CreatePanel(Control* control);

    // helper
    static PanelPtr CreatePanel(const V_Control& controls);

    virtual void Read();

    virtual void SetText(const std::string& text);

    const std::string& GetText() const;

    friend class APPLICATION_API Canvas;
  };

  typedef Nocturnal::SmartPtr<Panel> PanelPtr;
  typedef std::vector<Panel*> V_Panel;
}
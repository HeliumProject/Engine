#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Group.h"
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
  
  const static tchar PANEL_ATTR_TEXT[] = TXT( "text" );
  
  //
  // Contains other controls and distributes layout logic
  //

  class APPLICATION_API Panel : public Reflect::ConcreteInheritor<Panel, Group>
  {
  protected:
    // the item in the tree for this panel
    PanelItemData m_ItemData;

    // the title of the panel
    tstring m_Text;

    // the path of the panel
    tstring m_Path;

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
    virtual bool Process(const tstring& key, const tstring& value);

  public:
    virtual wxWindow* GetContextWindow() HELIUM_OVERRIDE;

    virtual const wxWindow* GetContextWindow() const HELIUM_OVERRIDE;

    // cleanup
    virtual void Clear() HELIUM_OVERRIDE;
    
    // init
    virtual void Realize(Container* parent) HELIUM_OVERRIDE;
    
    virtual void UnRealize() HELIUM_OVERRIDE;

    virtual void SetEnabled(bool enabled) HELIUM_OVERRIDE;
    
    void RefreshControls();

    tstring GetPath();

  protected:
    // build the path of titles to this panel
    void BuildPath(tstring& path);
    
    void SetItemExpanded(bool expanded);
    
    wxTreeItemId GetParentTreeNode(Container* parent);

  public:
    // builds a list of expanded panels
    void GetExpanded(std::map< tstring, tstring >& paths);

    // sets expansion from paths
    void SetExpanded(const std::map< tstring, tstring >& paths);

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

    virtual void SetText(const tstring& text);

    const tstring& GetText() const;

    friend class APPLICATION_API Canvas;
  };

  typedef Helium::SmartPtr<Panel> PanelPtr;
  typedef std::vector<Panel*> V_Panel;
}
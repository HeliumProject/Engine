#pragma once

#include <hash_map>

#include "Application/API.h"
#include "Application/Inspect/Controls/Container.h"
#include "Application/Inspect/Controls/TreeWnd/TreeWndCtrl.h"

namespace Inspect
{
  // Typedefs
  typedef std::map< tstring, bool > M_ExpandState;


  //
  // 
  //

  class APPLICATION_API CanvasStrip : public wxPanel
  {
  public:
    Canvas* m_Canvas;

    CanvasStrip( wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                 const wxString& name = wxT( "CanvasStrip" ) );

    Canvas* GetCanvas()
    {
      return m_Canvas;
    }

    void SetCanvas(Canvas* canvas)
    {
      m_Canvas = canvas;
    }

  protected:
    void OnShow(wxShowEvent&);
    void OnSize(wxSizeEvent&);
    void OnClick(wxMouseEvent&);
    void OnToggle(wxTreeEvent&);

    DECLARE_EVENT_TABLE();
  };

  //
  // Defines the base wx-derived window used by the canvas
  //

  class APPLICATION_API CanvasWindow : public Nocturnal::TreeWndCtrl
  {
  public:
    Canvas* m_Canvas;

    CanvasWindow(wxWindow *parent,
                 wxWindowID winid = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxScrolledWindowStyle,
                 const wxString& name = wxT( "CanvasWindow" ),
                 int treeStyle = ( wxTR_ALL_LINES | wxTR_HIDE_ROOT ),
                 unsigned int columnSize = WXTWC_DEFAULT_COLUMN_SIZE,
                 wxBitmap expandedBitmap = Nocturnal::TreeWndCtrlDefaultExpand,
                 wxBitmap collapsedBitmap = Nocturnal::TreeWndCtrlDefaultCollapse,
                 wxPen pen = Nocturnal::TreeWndCtrlDefaultPen,
                 unsigned int clickTolerance = WXTWC_DEFAULT_CLICK_TOLERANCE);
                 
    Canvas* GetCanvas();
    void SetCanvas(Canvas* canvas);

  protected:


  protected:
    void OnShow(wxShowEvent&);
    void OnSize(wxSizeEvent&);
    void OnClick(wxMouseEvent&);
    void OnToggle(wxTreeEvent&);

    DECLARE_EVENT_TABLE();
  };


  //
  // Conainer object that manages all the panels displayed in the property canvas
  //

  struct CanvasShowArgs
  {
    bool m_Show;

    CanvasShowArgs(bool show)
      : m_Show(show)
    {

    }
  };
  typedef Nocturnal::Signature<void, const CanvasShowArgs&> CanvasShowSignature;

  class APPLICATION_API Canvas : public Reflect::ConcreteInheritor<Canvas, Container>
  {
  public:
    enum ExpandState
    {
      Expanded,
      Collapsed,
      Default
    };

  protected:
    // tree window items
    wxArrayTreeItemIds m_TreeNodeStack;
    
    // list of panels that are expanded
    M_ExpandState m_PanelExpandState;

    // is the canvas locked?
    bool m_IsLocked;
    
    // should the canvas automatically expand its children panels and hide their nodes?
    bool m_PanelsExpanded;

    // standard control size
    Math::Point m_StdSize;

    // standard border width
    int m_Border;

    // standard pad b/t controls
    int m_Pad;

    //
    // Implementation
    //

  public:
    Canvas();
    virtual ~Canvas();


    //
    // Window
    //

    // actual window control
    CanvasWindow* GetControl();
    void SetControl(CanvasWindow* control);
    
    void Layout()
    {
      Realize( NULL );
    }


    //
    // Factory
    //

    // creation factory for child controls
    template<class T>
    Nocturnal::SmartPtr<T> Create(Interpreter* interpreter = NULL)
    {
      return Reflect::ObjectCast<T>( Canvas::Create( Reflect::GetType<T>(), interpreter ) );
    }

    // actual creation factory
    ControlPtr Create(int type, Interpreter* interpreter = NULL);


    //
    // Children
    //

    // NOC_OVERRIDE
    virtual void RemoveControl(Control* control) NOC_OVERRIDE;

    // NOC_OVERRIDE
    virtual void Clear() NOC_OVERRIDE;

    // NOC_OVERRIDE
    virtual void Realize(Container* parent) NOC_OVERRIDE;

    //
    // Expansion State
    //

    ExpandState GetPanelExpandState( const tstring& panelName ) const;
    void SetPanelExpandState( const tstring& panelName, ExpandState state );


    //
    // Lock - To lock the canvas means that none of the controls contained should be enabled for edit.
    // 

    bool IsLocked() const
    { 
      return m_IsLocked; 
    }

    void SetLocked( bool isLocked );
    
    //
    // Expand Panels - Make all children panels expanded and hide their tree nodes
    
    bool ArePanelsExpanded() const
    {
      return m_PanelsExpanded;
    }
    
    void SetPanelsExpanded( bool panelsExpanded );


    //
    // Metrics
    //

    virtual int GetStdSize(Math::Axis axis)
    {
      return m_StdSize[axis];
    }

    virtual int GetBorder()
    {
      return m_Border;
    }

    virtual int GetPad()
    {
      return m_Pad;
    }


    //
    // Scrolling
    //

    virtual Math::Point GetScroll();
    virtual void SetScroll(const Math::Point& scroll);


    //
    // Events
    //

  protected:
    CanvasShowSignature::Event m_Show;
  public:
    void AddShowListener( const CanvasShowSignature::Delegate& listener )
    {
      m_Show.Add( listener );
    }
    void RemoveShowListener( const CanvasShowSignature::Delegate& listener )
    {
      m_Show.Remove( listener );
    }
    void RaiseShow(const CanvasShowArgs& args)
    {
      m_Show.Raise( args );
    }
  };

  typedef Nocturnal::SmartPtr<Canvas> CanvasPtr;
}
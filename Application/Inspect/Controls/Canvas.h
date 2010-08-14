#pragma once

#include <hash_map>

#include "Application/API.h"
#include "Application/Inspect/Controls/Container.h"
#include "Application/Inspect/Controls/TreeWnd/TreeWndCtrl.h"

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API StripCanvasCtrl : public wxPanel
        {
        public:
            Canvas* m_Canvas;

            StripCanvasCtrl( wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER,
                const wxString& name = wxT( "StripCanvasCtrl" ) );

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

        class APPLICATION_API TreeCanvasCtrl : public Helium::TreeWndCtrl
        {
        public:
            Canvas* m_Canvas;

            TreeCanvasCtrl(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxScrolledWindowStyle,
                const wxString& name = wxT( "TreeCanvasCtrl" ),
                int treeStyle = ( wxTR_ALL_LINES | wxTR_HIDE_ROOT ),
                unsigned int columnSize = WXTWC_DEFAULT_COLUMN_SIZE,
                wxBitmap expandedBitmap = Helium::TreeWndCtrlDefaultExpand,
                wxBitmap collapsedBitmap = Helium::TreeWndCtrlDefaultCollapse,
                wxPen pen = Helium::TreeWndCtrlDefaultPen,
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
        typedef Helium::Signature<void, const CanvasShowArgs&> CanvasShowSignature;

        typedef std::map< tstring, bool > M_ExpandState;

        namespace ExpandStates
        {
            enum ExpandState
            {
                Expanded,
                Collapsed,
                Default
            };
        }
        typedef ExpandStates::ExpandState ExpandState;

        class APPLICATION_API Canvas : public Reflect::ConcreteInheritor<Canvas, Container>
        {
        public:
            Canvas();
            virtual ~Canvas();

            // actual window control
            TreeCanvasCtrl* GetControl();
            void SetControl(TreeCanvasCtrl* control);

            void Layout()
            {
                Realize( NULL );
            }

            // creation factory for child controls
            template<class T>
            Helium::SmartPtr<T> Create(Interpreter* interpreter = NULL)
            {
                return Reflect::ObjectCast<T>( Canvas::Create( Reflect::GetType<T>(), interpreter ) );
            }

            // actual creation factory
            ControlPtr Create(int type, Interpreter* interpreter = NULL);

            // Children
            virtual void RemoveControl(Control* control) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            // Realize
            virtual void Realize(Container* parent) HELIUM_OVERRIDE;

            // Expansion State
            ExpandState GetPanelExpandState( const tstring& panelName ) const;
            void SetPanelExpandState( const tstring& panelName, ExpandState state );

            // Lock - To lock the canvas means that none of the controls contained should be enabled for edit.
            bool IsLocked() const
            { 
                return m_IsLocked; 
            }
            void SetLocked( bool isLocked );

            // Expand Panels - Make all children panels expanded and hide their tree nodes
            bool ArePanelsExpanded() const
            {
                return m_PanelsExpanded;
            }
            void SetPanelsExpanded( bool panelsExpanded );

            // Metrics
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

            // Scrolling
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
        };

        typedef Helium::SmartPtr<Canvas> CanvasPtr;
    }
}
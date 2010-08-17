#pragma once

#include <hash_map>

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectContainer.h"

namespace Helium
{
    namespace Inspect
    {
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
            // list of panels that are expanded
            M_ExpandState m_PanelExpandState;

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
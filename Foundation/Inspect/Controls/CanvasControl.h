#pragma once

#include <hash_map>

#include "Foundation/API.h"
#include "Foundation/Inspect/Controls/ContainerControl.h"

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

        class FOUNDATION_API Canvas : public Reflect::ConcreteInheritor<Canvas, Container>
        {
        public:
            Canvas();
            virtual ~Canvas();

            virtual void RealizeControl(Control* control, Control* parent);

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
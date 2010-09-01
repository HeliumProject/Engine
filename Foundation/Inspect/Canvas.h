#pragma once

#include <hash_map>

#include "Foundation/API.h"
#include "Foundation/Inspect/Container.h"

namespace Helium
{
    namespace Inspect
    {
        struct CanvasShowArgs
        {
            CanvasShowArgs(bool show)
                : m_Show(show)
            {

            }

            bool m_Show;
        };
        typedef Helium::Signature<void, const CanvasShowArgs&> CanvasShowSignature;

        class FOUNDATION_API Canvas : public Reflect::AbstractInheritor<Canvas, Container>
        {
        public:
            Canvas();

            virtual void RealizeControl(Control* control, Control* parent) = 0;

            int GetStdSize(Math::Axis axis)
            {
                return m_StdSize[axis];
            }

            int GetBorder()
            {
                return m_Border;
            }

            int GetPad()
            {
                return m_Pad;
            }

            CanvasShowSignature::Event e_Show;

        protected:
            Math::Point m_StdSize;      // standard control size
            int m_Border;               // standard border width
            int m_Pad;                  // standard pad b/t controls
        };

        typedef Helium::SmartPtr<Canvas> CanvasPtr;
    }
}
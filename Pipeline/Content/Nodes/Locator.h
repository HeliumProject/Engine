#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/Instance.h"

namespace Helium
{
    namespace Content
    {
        namespace LocatorShapes
        {
            enum LocatorShape
            {
                Cross,
                Cube,
            };
            static void LocatorShapeEnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement(Cross, TXT( "Cross" ) );
                info->AddElement(Cube, TXT( "Cube" ) );
            }
        }

        typedef LocatorShapes::LocatorShape LocatorShape;

        class PIPELINE_API Locator : public Instance
        {
            //
            // Members
            //

        public:
            LocatorShape m_Shape;


            //
            // RTTI
            //

        public:
            REFLECT_DECLARE_CLASS(Locator, Instance);

            static void EnumerateClass( Reflect::Compositor<Locator>& comp );


            //
            // Implementation
            //

        public:
            Locator ()
                : m_Shape (LocatorShapes::Cross)
            {

            }

            Locator (LocatorShape shape)
                : m_Shape (shape)
            {

            }

            Locator (const Helium::TUID& id)
                : Instance (id)
                , m_Shape (LocatorShapes::Cross)
            {

            }

            Locator (const Helium::TUID& id, LocatorShape shape)
                : Instance (id)
                , m_Shape (shape)
            {

            }
        };

        typedef Helium::SmartPtr<Locator> LocatorPtr;
        typedef std::vector<LocatorPtr> V_Locator;
    }
}
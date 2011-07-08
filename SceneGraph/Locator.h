#pragma once

#include "Instance.h"

namespace Helium
{
    namespace SceneGraph
    {
        class LocatorType;

        class LocatorShape
        {
        public:
            enum Enum
            {
                Cross,
                Cube,
            };

            REFLECT_DECLARE_ENUMERATION( LocatorShape );

            static void EnumerateEnum( Reflect::Enumeration& info )
            {
                info.AddElement(Cross,  TXT( "Cross" ) );
                info.AddElement(Cube,   TXT( "Cube" ) );
            }
        };

        class Locator : public Instance
        {
        public:
            REFLECT_DECLARE_OBJECT( Locator, Instance );
            static void PopulateComposite( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

            Locator();
            ~Locator();

            LocatorShape GetShape() const;
            void SetShape( LocatorShape shape );

            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;
            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

        protected:
            LocatorShape m_Shape;
        };

        typedef Helium::SmartPtr<Locator> LLocatorPtr;
    }
}
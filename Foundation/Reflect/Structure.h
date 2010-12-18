#pragma once

#include "Composite.h"

namespace Helium
{
    namespace Reflect
    {
        class Field;
        class Structure;


        //
        // Structure (struct or class)
        //

        class FOUNDATION_API Structure : public Composite
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Structure );

        protected:
            Structure();
            virtual ~Structure();

        public:
            // protect external allocation to keep inlined code in this dll
            static Structure* Create();

            template< class StructureT >
            static Structure* Create ( Name name, Name baseName, AcceptVisitor accept )
            {
                Structure* info = Structure::Create();

                // populate reflection information
                CreateComposite< StructureT >( name, baseName, accept, info );

                return info;
            }
        };
    }
}
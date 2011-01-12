#pragma once

#include "Composite.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Structure (struct or class)
        //

        class FOUNDATION_API Structure : public Composite
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Structure, Structure, Composite );

        protected:
            Structure();
            virtual ~Structure();

        public:
            // protect external allocation to keep inlined code in this dll
            static Structure* Create();

            template< class StructureT >
            static Structure* Create( const tchar_t* name, const tchar_t* baseName, AcceptVisitor accept )
            {
                Structure* info = Structure::Create();

                // populate reflection information
                Composite::Create< StructureT >( name, baseName, accept, info );

                info->m_Default = new StructureT;

                return info;
            }
        };
    }
}
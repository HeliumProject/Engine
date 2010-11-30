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

            template<class T>
            static Structure* Create ( const tstring& name, const tstring& base )
            {
                Structure* info = Structure::Create();

                info->m_Size = sizeof(T);
                info->m_Name = name;
                info->m_Base = base;
                info->m_UIName = info->m_Name;
                info->m_Enumerator = (CompositeEnumerator)&Reflect::Enumerate<T>;

                // enumerate reflection data
                {
                    // create the default object
                    T temp;

                    // enumerate the fields in the structure
                    info->EnumerateInstance<T>(&temp);
                }

                return info;
            }
        };
    }
}
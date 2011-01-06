#pragma once

#include "Foundation/Reflect/Composite.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/Registry.h"

#include "Platform/String.h"

namespace Helium
{
    namespace Reflect
    {
        class Field;
        class Class;


        //
        // Class (struct or class)
        //

        class FOUNDATION_API Class : public Composite
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Class );

            CreateObjectFunc      m_Creator;             // factory function for creating instances of this class

        protected:
            Class();
            virtual ~Class();

        public:
            // protect external allocation to keep inlined code in this dll
            static Class* Create();

            template< class ClassT >
            static Class* Create( Name name, Name baseName, CreateObjectFunc creator = NULL )
            {
                Class* info = Class::Create();

                // populate reflection information
                Composite::Create< ClassT >( name, baseName, &ClassT::AcceptCompositeVisitor, info );

                // setup factory function
                info->m_Creator = creator;

                return info;
            }

            //
            // Clone makes a deep copy of the passed object, you can use FieldFlags::Share to flag fields
            //  to not be used when doing the traversal for deep copy cloning.  Just the reference will be copied
            //  in such cases.  Currently we don't provide support for containers of non-cloneable fields.
            //

            static ElementPtr Clone(Element* element);
        };

        typedef Helium::SmartPtr< Class > ClassPtr;
        typedef Helium::SmartPtr< const Class > ConstClassPtr;

        template<>
        Class* Class::Create< Object >( Name name, Name baseName, CreateObjectFunc creator );
    }
}

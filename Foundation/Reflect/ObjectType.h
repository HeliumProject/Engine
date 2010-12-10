#pragma once

#include "Foundation/Reflect/Type.h"

namespace Helium
{
    namespace Reflect
    {
        class ObjectType;
        typedef SmartPtr< ObjectType > ObjectTypePtr;

        //
        // Type information for Object classes.
        //

        class FOUNDATION_API ObjectType : public Type
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::ObjectType );

        protected:
            /// @name Construction/Destruction
            //@{
            ObjectType();
            virtual ~ObjectType();
            //@}

        public:
            /// Base type.
            ObjectTypePtr m_BaseType;
        };
    }
}

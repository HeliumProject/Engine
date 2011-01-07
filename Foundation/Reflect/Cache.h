#pragma once

#include <stack>
#include <hash_map>

#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/Reflect/API.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API Object;
        typedef Helium::StrongPtr<Object> ObjectPtr;
        typedef std::stack<ObjectPtr> S_Object;
        typedef stdext::hash_map<const Type*, S_Object> H_Object;

        class Cache
        {
        protected:
            // hash_map of stacks (the free list)
            H_Object m_Objects;

        public:
            // creator
            bool Create( const Class* type, ObjectPtr& element );

            // push into free list
            void Free( ObjectPtr element );
        };
    }
}
#pragma once

#include <stack>
#include <hash_map>

#include "Foundation/Memory/ReferenceCounting.h"
#include "Foundation/Name.h"

#include "API.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API Element;
        typedef Helium::StrongPtr<Element> ElementPtr;
        typedef std::stack<ElementPtr> S_Element;
        typedef stdext::hash_map<const Type*, S_Element> H_Element;

        class Cache
        {
        protected:
            // hash_map of stacks (the free list)
            H_Element m_Elements;

        public:
            // creator
            bool Create( const Class* type, ElementPtr& element );

            // push into free list
            void Free( ElementPtr element );
        };
    }
}
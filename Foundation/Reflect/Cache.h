#pragma once

#include <stack>
#include <hash_map>

#include "Foundation/Memory/SmartPtr.h"

#include "API.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API Element;
        typedef Helium::SmartPtr<Element> ElementPtr;
        typedef std::stack<ElementPtr> S_Element;
        typedef stdext::hash_map<int, S_Element> H_Element;

        class Cache
        {
        protected:
            // hash_map of stacks (the free list)
            H_Element m_Elements;

        public:
            // creator
            bool Create(int type, ElementPtr& element);

            // creator
            bool Create(const tstring& name, ElementPtr& element);

            // push into free list
            void Free(ElementPtr element);
        };
    }
}
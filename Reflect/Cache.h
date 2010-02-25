#pragma once

#include <stack>
#include <hash_map>

#include "Common/Memory/SmartPtr.h"

#include "API.h"

namespace Reflect
{
  class REFLECT_API Element;
  typedef Nocturnal::SmartPtr<Element> ElementPtr;
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
    bool Create(const std::string& shortName, ElementPtr& element);

    // push into free list
    void Free(ElementPtr element);
  };
}
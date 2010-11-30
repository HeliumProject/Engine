#include "Cache.h"
#include "Foundation/Reflect/Data/Data.h"
#include "Registry.h"
#include "Foundation/Container/Insert.h"

#include <memory>

using Helium::Insert; 
using namespace Helium;
using namespace Helium::Reflect;

// uncomment to disable caching completely
//#define REFLECT_DISABLE_CACHING

// uncomment to display the cache hit/miss info
//#define REFLECT_DISPLAY_CACHE_INFO

#ifdef REFLECT_DISPLAY_CACHE_INFO
static int g_HitCount = 0;
static int g_MissCount = 0;
#endif

static void CreateInstance( const Class* type, ElementPtr& element )
{
    ObjectPtr object = Registry::GetInstance()->CreateInstance(type);
    HELIUM_ASSERT( object.ReferencesObject() );

    element = AssertCast<Element>( object );

#ifdef REFLECT_DISPLAY_CACHE_INFO
    Log::Print("Cache miss %d on type '%s', short name '%s', id '%d'\n", ++g_MissCount, type->m_Name.c_str(), element->GetClass()->m_Name.c_str(), type);
#endif
}

bool Cache::Create( const Class* type, ElementPtr& element )
{
#ifdef REFLECT_DISABLE_CACHING
    ::CreateInstance(type, element);

    return true;
#else
    H_Element::iterator found = m_Elements.find(type);

    if (found == m_Elements.end())
    {
        ::CreateInstance(type, element);

        return true;
    }
    else
    {
        S_Element& stack (found->second);

        if (stack.size() == 0)
        {
            ::CreateInstance(type, element);
        }
        else
        {
            ElementPtr top = stack.top();

            stack.pop();

            element = top;

#ifdef REFLECT_DISPLAY_CACHE_INFO
            Log::Print("Cache hit %d on type '%s', short name '%s', id '%d'\n", ++g_HitCount, type->m_Name.c_str(), element->GetClass()->m_Name.c_str(), type);
#endif
        }

        return true;
    }
#endif
}

bool Cache::Create(const tstring& name, ElementPtr& element)
{
    const Class* type = Registry::GetInstance()->GetClass(name);

    if ( type )
    {
        return Create(type, element);
    }
    else
    {
        return false;
    }
}

void Cache::Free(ElementPtr element)
{
    if (!element->HasType(Reflect::GetType<Data>()))
        return;

    H_Element::iterator found = m_Elements.find(element->GetType());

    if (found == m_Elements.end())
    {
        S_Element stack;

        stack.push(element);

        Insert<H_Element>::Result result = m_Elements.insert(H_Element::value_type (element->GetType(), stack));

        HELIUM_ASSERT( result.second );
    }
    else
    {
        S_Element& stack (found->second);

        stack.push(element);
    }
}

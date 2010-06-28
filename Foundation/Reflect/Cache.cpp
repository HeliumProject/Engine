#include "Cache.h"
#include "Serializer.h"
#include "Registry.h"
#include "Foundation/Container/Insert.h"

#include <memory>

using Nocturnal::Insert; 
using namespace Reflect;

// uncomment to disable caching completely
//#define REFLECT_DISABLE_CACHING

// uncomment to display the cache hit/miss info
//#define REFLECT_DISPLAY_CACHE_INFO

#ifdef REFLECT_DISPLAY_CACHE_INFO
static int g_HitCount = 0;
static int g_MissCount = 0;
#endif

static void GetTypeName(int type, tstring& name)
{
    const Class* typeInfo = Registry::GetInstance()->GetClass( type );
    NOC_ASSERT( typeInfo );

    name = typeInfo->m_ShortName;
}

static void CreateInstance(int type, ElementPtr& element)
{
    ObjectPtr object = Registry::GetInstance()->CreateInstance(type);

    NOC_ASSERT( object.ReferencesObject() );

    element = AssertCast<Element>( object );

#ifdef REFLECT_DISPLAY_CACHE_INFO
    std::string name;
    GetTypeName(type, name);
    Log::Print("Cache miss %d on type '%s', short name '%s', id '%d'\n", ++g_MissCount, name.c_str(), element->GetClass()->m_ShortName.c_str(), type);
#endif
}

bool Cache::Create(int type, ElementPtr& element)
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
            std::string name;
            GetTypeName(type, name);
            Log::Print("Cache hit %d on type '%s', short name '%s', id '%d'\n", ++g_HitCount, name.c_str(), element->GetClass()->m_ShortName.c_str(), type);
#endif
        }

        return true;
    }
#endif
}

bool Cache::Create(const tstring& shortName, ElementPtr& element)
{
    const Class* typeInfo = Registry::GetInstance()->GetClass(shortName);

    if ( typeInfo )
    {
        return Create(typeInfo->m_TypeID, element);
    }
    else
    {
        return false;
    }
}

void Cache::Free(ElementPtr element)
{
    if (!element->HasType(Reflect::GetType<Serializer>()))
        return;

    H_Element::iterator found = m_Elements.find(element->GetType());

    if (found == m_Elements.end())
    {
        S_Element stack;

        stack.push(element);

        Insert<H_Element>::Result result = m_Elements.insert(H_Element::value_type (element->GetType(), stack));

        NOC_ASSERT( result.second );
    }
    else
    {
        S_Element& stack (found->second);

        stack.push(element);
    }
}

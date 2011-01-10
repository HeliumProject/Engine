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

static void CreateInstance( const Class* type, ObjectPtr& object )
{
    object = Registry::GetInstance()->CreateInstance(type);
    HELIUM_ASSERT( object.ReferencesObject() );

#ifdef REFLECT_DISPLAY_CACHE_INFO
    Log::Print("Cache miss %d on type '%s', name '%s', id '%d'\n", ++g_MissCount, type->m_Name, object->GetClass()->m_Name, type);
#endif
}

bool Cache::Create( const Class* type, ObjectPtr& object )
{
#ifdef REFLECT_DISABLE_CACHING
    ::CreateInstance(type, object);

    return true;
#else
    H_Object::iterator found = m_Objects.find(type);

    if (found == m_Objects.end())
    {
        ::CreateInstance(type, object);

        return true;
    }
    else
    {
        S_Object& stack (found->second);

        if (stack.size() == 0)
        {
            ::CreateInstance(type, object);
        }
        else
        {
            ObjectPtr top = stack.top();

            stack.pop();

            object = top;

#ifdef REFLECT_DISPLAY_CACHE_INFO
            Log::Print("Cache hit %d on type '%s', name '%s', id '%d'\n", ++g_HitCount, type->m_Name, object->GetClass()->m_Name, type);
#endif
        }

        return true;
    }
#endif
}

void Cache::Free(ObjectPtr object)
{
    if (!object->HasType(Reflect::GetType<Data>()))
    {
        return;
    }

    H_Object::iterator found = m_Objects.find(object->GetType());

    if (found == m_Objects.end())
    {
        S_Object stack;

        stack.push(object);

        StdInsert<H_Object>::Result result = m_Objects.insert(H_Object::value_type (object->GetType(), stack));

        HELIUM_ASSERT( result.second );
    }
    else
    {
        S_Object& stack (found->second);

        stack.push(object);
    }
}

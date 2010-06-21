#include "stdafx.h"
#include "Tracker.h"
#include "Element.h"
#include "Serializer.h"

#ifdef REFLECT_OBJECT_TRACKING

#include "Platform/Windows/Debug.h"
#include "Finder/Finder.h"
#include "Platform/Mutex.h"

using namespace Reflect;

const std::string& StackRecord::Convert()
{
    if ( !m_Converted )
    {
        m_Converted = true;

        Debug::Translate( m_Stack, m_String );
    }

    return m_String;
}

CreationRecord::CreationRecord()
: m_Address (0x0)
, m_Type (-1)
{

}

CreationRecord::CreationRecord(PointerSizedUInt ptr)
: m_Address (ptr)
, m_Type (-1)
{

}

void CreationRecord::Dump(FILE* f)
{
    fprintf(f, "\n\n");
    fprintf(f, "Addr: %p\n", m_Address);
    fprintf(f, "Name: %s\n", m_ShortName.c_str());
    fprintf(f, "Type: %i\n", m_Type);

#ifdef REFLECT_OBJECT_STACK_TRACKING
    fprintf(f, "Create Stack:\n%s\n", m_CreateStack.ReferencesObject() ? m_CreateStack->Convert().c_str() : "<none>" );
    fprintf(f, "Delete Stack:\n%s\n", m_DeleteStack.ReferencesObject() ? m_DeleteStack->Convert().c_str() : "<none>" );
#endif
}

Platform::Mutex g_TrackerMutex;

Tracker::Tracker()
{

}

Tracker::~Tracker()
{
    Dump();
}

StackRecordPtr Tracker::GetStack()
{
    Platform::TakeMutex mutex (g_TrackerMutex);

    StackRecordPtr ptr = new StackRecord();

    Debug::GetStackTrace( ptr->m_Stack );

    M_StackRecord::iterator iter = m_Stacks.find( ptr->m_Stack );
    if ( iter != m_Stacks.end() )
    {
        ptr = (*iter).second;
    }
    else
    {
        m_Stacks.insert( M_StackRecord::value_type( ptr->m_Stack, ptr ) );
    }

    return ptr;
}

void Tracker::Create(PointerSizedUInt ptr)
{
    Platform::TakeMutex mutex (g_TrackerMutex);

    M_CreationRecord::iterator create_iter = m_CreatedObjects.find( ptr );
    if ( create_iter == m_CreatedObjects.end() )
    {
#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
        fprintf(stderr, "%p: CREATE\n", ptr);
#endif

        CreationRecord cr (ptr);

#ifdef REFLECT_OBJECT_STACK_TRACKING
        cr.m_CreateStack = GetStack();
#endif

        // insert into the map of objects that are currently created
        create_iter = m_CreatedObjects.insert( M_CreationRecord::value_type( ptr, cr ) ).first;
        NOC_ASSERT( create_iter != m_CreatedObjects.end() );

        // potentially cleanup an old deletion for this pointer
        M_CreationRecord::iterator delete_iter = m_DeletedObjects.find( ptr );
        if ( delete_iter != m_DeletedObjects.end() )
        {
            m_DeletedObjects.erase( delete_iter );
        }
    }
    else
    {
        // why are we getting two creates w/o a delete?
        NOC_BREAK();
        (*create_iter).second.Dump( stderr );
    }
}

void Tracker::Delete(PointerSizedUInt ptr)
{
    Platform::TakeMutex mutex (g_TrackerMutex);

    M_CreationRecord::iterator iter = m_CreatedObjects.find(ptr);
    if ( iter != m_CreatedObjects.end())
    {
        CreationRecord& cr = (*iter).second;

#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
        fprintf(stderr, "%p: DELETE", ptr);
        if (!cr.m_ShortName.empty())
        {
            fprintf(stderr, ": %s\n", cr.m_ShortName.c_str());
        }
        else
        {
            fprintf(stderr, "\n");
        }
#endif

#ifdef REFLECT_OBJECT_STACK_TRACKING
        cr.m_DeleteStack = GetStack();
#endif

        // insert into the map of objects which are currently deleted
        M_CreationRecord::iterator delete_iter = 
            m_DeletedObjects.insert( M_CreationRecord::value_type( ptr, cr ) ).first;
        NOC_ASSERT( delete_iter != m_DeletedObjects.end() );

        // erase the entry from objects which are currently created
        m_CreatedObjects.erase(iter);
    }
    else
    {
        // we should have a creation record for everything that gets deleted
        NOC_BREAK();
    }
}

void Tracker::Check(PointerSizedUInt ptr)
{
    Platform::TakeMutex mutex (g_TrackerMutex);

    M_CreationRecord::iterator iter = m_CreatedObjects.find(ptr);
    if ( iter != m_CreatedObjects.end())
    {
        CreationRecord& cr = (*iter).second;

        if (cr.m_Type < 0)
        {
            Element* e = reinterpret_cast<Element*>(ptr);

            cr.m_ShortName = e->GetClass()->m_ShortName;
            cr.m_Type = e->GetType();

#ifdef REFLECT_OBJECT_VERBOSE_TRACKING
            fprintf(stderr, "%p: TRACKING", ptr);
            fprintf(stderr, ": %s \n", cr.m_ShortName.c_str());
#endif
        }
    }
    else 
    {
        // uh oh, no so cool, take dump!
        iter = m_DeletedObjects.find(ptr);
        if ( iter != m_DeletedObjects.end() )
        {
            (*iter).second.Dump( stderr );
        }

        // why are we checking something that isn't created???
        NOC_BREAK();
    }
}

void Tracker::Dump()
{
    Platform::TakeMutex mutex (g_TrackerMutex);

    FILE* f = fopen( (Finder::ProjectLog() + "ReflectDump.log").c_str(), "w");
    if ( f != NULL )
    {
        typedef std::map< u32, std::pair< std::string, u32 > > ObjectLogger;
        ObjectLogger ObjectLog;

        {
            M_CreationRecord::iterator c_current = m_CreatedObjects.begin();
            M_CreationRecord::iterator c_end = m_CreatedObjects.end();

            for ( ; c_current != c_end; ++c_current)
            {
                (*c_current).second.Dump( f );

                ObjectLogger::iterator iter = ObjectLog.find( (*c_current).second.m_Type );
                if ( iter == ObjectLog.end() )
                {
                    ObjectLog.insert( ObjectLogger::value_type( (*c_current).second.m_Type, 
                        std::pair< std::string, u32 >( (*c_current).second.m_ShortName, 1 ) ) );
                }
                else
                {
                    (*iter).second.second++;
                } 
            }
        }

        {
            M_CreationRecord::iterator d_current = m_DeletedObjects.begin();
            M_CreationRecord::iterator d_end = m_DeletedObjects.end();

            for ( ; d_current != d_end; ++d_current)
            {
                ObjectLogger::iterator iter = ObjectLog.find( (*d_current).second.m_Type );
                if ( iter == ObjectLog.end() )
                {
                    ObjectLog.insert( ObjectLogger::value_type( (*d_current).second.m_Type, 
                        std::pair< std::string, u32 >( (*d_current).second.m_ShortName, 1 ) ) );
                }
                else
                {
                    (*iter).second.second++;
                }
            }
        }

        {
            size_t max = 0;
            ObjectLogger::iterator iter = ObjectLog.begin();  
            ObjectLogger::iterator end = ObjectLog.end();  

            for ( ; iter != end; ++iter )
            {
                max = std::max( (*iter).second.first.length(), max );
            }

            char format[1024];
            sprintf( format, "\nType: %%%ds, Count: %%d", max );

            iter = ObjectLog.begin();
            for ( ; iter != end; ++iter )
            {
                fprintf( f, format, (*iter).second.first.c_str(), (*iter).second.second );
            }
        }

        fclose(f);
    }
}

#endif

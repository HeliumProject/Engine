#include "Type.h"
#include "Registry.h"
#include "Serializer.h"

#include "Platform/Mutex.h"

using namespace Helium::Reflect;

Helium::Mutex g_TypeMutex;

Type::Type ()
: m_TypeID (ReservedTypes::Invalid)
, m_Size (0)
{

}

Type::~Type ()
{
    {
        std::vector<void**>::const_iterator itr = m_Pointers.begin();
        std::vector<void**>::const_iterator end = m_Pointers.end();
        for ( ; itr != end; ++itr )
        {
            *(*itr) = NULL; // release the cached pointer
        }
    }

    {
        std::vector<int32_t*>::const_iterator itr = m_IDs.begin();
        std::vector<int32_t*>::const_iterator end = m_IDs.end();
        for ( ; itr != end; ++itr )
        {
            *(*itr) = ReservedTypes::Invalid; // release the cached id
        }
    }
}

int32_t Type::AssignTypeID()
{
    static int32_t nextID = ReservedTypes::First;

    return nextID++;
}

void Type::TrackPointer(void** pointer) const
{
    Helium::TakeMutex mutex (g_TypeMutex);

    m_Pointers.push_back( pointer );
}

void Type::RemovePointer(void** pointer) const
{
    Helium::TakeMutex mutex (g_TypeMutex);

    m_Pointers.erase( std::remove( m_Pointers.begin(), m_Pointers.end(), pointer ), m_Pointers.end() );
}

void Type::TrackID(int32_t* id) const
{
    Helium::TakeMutex mutex (g_TypeMutex);

    m_IDs.push_back( id );
}

void Type::RemoveID(int32_t* id) const
{
    Helium::TakeMutex mutex (g_TypeMutex);

    m_IDs.erase( std::remove( m_IDs.begin(), m_IDs.end(), id ), m_IDs.end() );
}
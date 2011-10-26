#include "FoundationPch.h"
#include "Type.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/Data.h"

#include "Platform/Mutex.h"

using namespace Helium::Reflect;

Helium::Mutex g_TypeMutex;

Type::Type()
: m_Name( NULL )
, m_Size( 0 )
{

}

Type::~Type()
{

}

void Type::Report() const
{
    const tchar_t* type = ReflectionTypes::Strings[ GetReflectionType() ];
    Log::Debug( TXT( "Reflect %s: 0x%p, Size: %4d, Name: %s (0x%08x)\n" ), type, this, m_Size, m_Name, Crc32( m_Name ) );
}

void Type::Unregister() const
{

}

TypeRegistrar* TypeRegistrar::s_Head[ RegistrarTypes::Count ] = { NULL, NULL, NULL };
TypeRegistrar* TypeRegistrar::s_Tail[ RegistrarTypes::Count ] = { NULL, NULL, NULL };

TypeRegistrar::TypeRegistrar( const tchar_t* name )
: m_Name( name )
, m_Next( NULL )
{

}

void TypeRegistrar::AddToList( RegistrarType type, TypeRegistrar* registrar )
{
    if ( s_Tail[ type ] )
    {
        s_Tail[ type ]->m_Next = registrar;
        s_Tail[ type ] = registrar;
    }
    else
    {
        s_Head[ type ] = registrar;
        s_Tail[ type ] = registrar;
    }
}

void TypeRegistrar::RemoveFromList( RegistrarType type, TypeRegistrar* registrar )
{
    if ( registrar == s_Head[ type ] )
    {
        s_Head[ type ] = s_Head[ type ]->m_Next;
    }
    else
    {
        for ( TypeRegistrar *r = s_Head[ type ], *p = NULL; r; p = r, r = r->m_Next )
        {
            if ( r == registrar )
            {
                if ( p )
                {
                    p->m_Next = r->m_Next;
                }

                if ( r == s_Tail[ type ] )
                {
                    s_Tail[ type ] = NULL;
                }
            }
        }
    }
}

void TypeRegistrar::RegisterTypes( RegistrarType type )
{
    for ( TypeRegistrar* r = s_Head[ type ]; r; r = r->m_Next )
    {
        r->Register();
    }
}

void TypeRegistrar::UnregisterTypes( RegistrarType type )
{
    for ( TypeRegistrar* r = s_Head[ type ]; r; r = r->m_Next )
    {
        r->Unregister();
    }
}

void TypeRegistrar::AddTypeToRegistry( Type* type )
{
    Reflect::Registry::GetInstance()->RegisterType( type );
}

void TypeRegistrar::RemoveTypeFromRegistry( const Type* type )
{
    Reflect::Registry::GetInstance()->UnregisterType( type );
}
#include "EnginePch.h"
#include "Engine/Package.h"

#include "Foundation/Reflect/Class.h"
#include "Engine/GameObjectType.h"

using namespace Helium;

HELIUM_IMPLEMENT_OBJECT_NOINITTYPE( Package, Engine );

/// Constructor.
Package::Package()
    : m_pLoader( NULL )
{
    // Set the package flag by default.
    SetFlags( FLAG_PACKAGE );
}

/// Destructor.
Package::~Package()
{
}

/// Initialize the static type information for the "Package" class.
///
/// @return  Static "Package" type.
const GameObjectType* Package::InitStaticType()
{
    HELIUM_ASSERT( s_Class )
    if ( !s_Class->m_Tag )
    {
        // Package type is registered manually during GameObject type initialization, so retrieve the type info from the
        // existing registered data.
        HELIUM_VERIFY( GameObject::InitStaticType() );
    }

    return static_cast< const Helium::GameObjectType* >( s_Class->m_Tag );
}

/// Set the loader for this package.
///
/// @param[in] pLoader  Package loader.
///
/// @see GetLoader()
void Package::SetLoader( PackageLoader* pLoader )
{
    m_pLoader = pLoader;
}

#include "Component.h"
#include "ComponentCollection.h"

using namespace Helium;
using namespace Helium::Component;

REFLECT_DEFINE_ABSTRACT(ComponentBase)

void ComponentBase::EnumerateClass( Reflect::Compositor<ComponentBase>& comp )
{
    Reflect::Field* fieldEnabled = comp.AddField( &ComponentBase::m_IsEnabled, "m_IsEnabled" );
}

ComponentBase::ComponentBase()
: m_Collection( NULL )
, m_IsEnabled( true )
{

}

ComponentBase::~ComponentBase()
{

}

ComponentCollection* ComponentBase::GetCollection() const
{
    return m_Collection;
}

void ComponentBase::SetCollection(ComponentCollection* collection)
{
    if ( m_Collection && collection && m_Collection != collection )
    {
        throw Helium::Exception ( TXT( "Cannot add attribute '%s' to collection '%s', it is already the member of another collection '%s'" ), GetClass()->m_Name.c_str(), collection->GetClass()->m_Name.c_str(), m_Collection->GetClass()->m_Name.c_str() );
    }

    m_Collection = collection;
}

bool ComponentBase::ValidateSibling( const ComponentBase* attribute, tstring& error ) const
{
    return true;
}
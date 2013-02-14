

#include "SceneGraphPch.h"
#include "Component.h"
#include "ComponentCollection.h"

#if 0

REFLECT_DEFINE_ABSTRACT( Helium::Component::ComponentBase )

using namespace Helium;
using namespace Helium::Component;

void ComponentBase::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &ComponentBase::m_IsEnabled, TXT( "m_IsEnabled" ) );
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
        throw Helium::Exception ( TXT( "Cannot add attribute '%s' to collection '%s', it is already the member of another collection '%s'" ), GetClass()->m_Name, collection->GetClass()->m_Name, m_Collection->GetClass()->m_Name );
    }

    m_Collection = collection;
}

bool ComponentBase::ValidateSibling( const ComponentBase* attribute, tstring& error ) const
{
    return true;
}

#endif
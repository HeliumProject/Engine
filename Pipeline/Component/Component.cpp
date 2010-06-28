#include "Component.h"
#include "ComponentCollection.h"
#include "ComponentCategory.h"
#include "ComponentCategories.h"

using namespace Component;

REFLECT_DEFINE_ABSTRACT(ComponentBase)

void ComponentBase::EnumerateClass( Reflect::Compositor<ComponentBase>& comp )
{
    Reflect::Field* fieldEnabled = comp.AddField( &ComponentBase::m_Enabled, "m_Enabled" );
}

ComponentBase::ComponentBase()
: m_Collection( NULL )
, m_Enabled( true )
{

}

ComponentBase::~ComponentBase()
{

}

const ComponentCategory& ComponentBase::GetCategory() const
{
    return *( ComponentCategories::GetInstance()->GetCategory( GetCategoryType() ) );
}

ComponentCollection* ComponentBase::GetCollection() const
{
    return m_Collection;
}

void ComponentBase::SetCollection(ComponentCollection* collection)
{
    if ( m_Collection && collection && m_Collection != collection )
    {
        throw Nocturnal::Exception ( TXT( "Cannot add attribute '%s' to collection '%s', it is already the member of another collection '%s'" ), GetClass()->m_ShortName.c_str(), collection->GetClass()->m_ShortName.c_str(), m_Collection->GetClass()->m_ShortName.c_str() );
    }

    m_Collection = collection;
}

bool ComponentBase::ValidateSibling( const ComponentBase* attribute, tstring& error ) const
{
    return true;
}

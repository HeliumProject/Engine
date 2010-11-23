#include "ComponentCollection.h"
#include "Foundation/Component/ComponentExceptions.h"

#include "Foundation/Container/Insert.h" 

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Component;
using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS(ComponentCollection)

void ComponentCollection::EnumerateClass( Reflect::Compositor<ComponentCollection>& comp )
{
    Reflect::Field* fieldComponentsByType = comp.AddField( &ComponentCollection::m_Components, "m_Components" );
}

ComponentCollection::ComponentCollection()
: m_Modified( false )
{
}

ComponentCollection::ComponentCollection( const ComponentPtr& component )
{
    HELIUM_ASSERT( component->GetSlot() != Reflect::ReservedTypes::Invalid );

    m_Components.insert( M_Component::value_type( component->GetSlot(), component ) );
    component->AddChangedListener( ElementChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );
    m_Modified = true;
}

ComponentCollection::~ComponentCollection()
{
    // it is possible for attributes to survive the collection
    // so we must remove our registered event handlers from the attributes
    //
    // we do it manually because i don't want to raise a whole lot of events
    // by calling Clear()
    // 
    M_Component::iterator attrItr = m_Components.begin(); 
    M_Component::iterator attrEnd = m_Components.end(); 

    for( ; attrItr != attrEnd; ++attrItr )
    {
        attrItr->second->RemoveChangedListener( ElementChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );
    }
}

void ComponentCollection::GatherSearchableProperties( Helium::SearchableProperties* properties ) const
{
    for( M_Component::const_iterator attrItr = m_Components.begin(), attrEnd = m_Components.end(); attrItr != attrEnd; ++attrItr )
    {
        if ( attrItr->second->m_IsEnabled )
        {
            attrItr->second->GatherSearchableProperties( properties );
        }
    }
}

void ComponentCollection::Clear()
{
    // Make a copy of the list that we are going to iterate over, since m_Components
    // is going to be changing as we iterate.
    M_Component copy = m_Components;
    M_Component::iterator attrItr = copy.begin();
    M_Component::iterator attrEnd = copy.end();
    for( ; attrItr != attrEnd; ++attrItr )
    {
        RemoveComponent( attrItr->second->GetSlot() );
    }
}

const M_Component& ComponentCollection::GetComponents() const
{
    return m_Components;
}

const ComponentPtr& ComponentCollection::GetComponent(int32_t slotID) const
{
    static const ComponentPtr kNull;
    const M_Component::const_iterator end = m_Components.end();
    M_Component::const_iterator found = m_Components.find( slotID );
    if ( found != end )
    {
        return found->second;
    }
    else
    {
        // Travel up the inheritance hierarchy looking for a base class slot within
        // this collection.
        Reflect::Registry* registry = Reflect::Registry::GetInstance();
        const Reflect::Class* type = registry->GetClass( slotID );
        type = registry->GetInstance()->GetClass( type->m_Base );

        // While we have base class type information, and we haven't hit the Component
        // base class, keep iterating.
        while ( type && ( type->m_TypeID != Reflect::GetType< ComponentBase >() ) )
        {
            // See if the base class has a slot in this collection.
            found = m_Components.find( type->m_TypeID );
            if ( found != end )
            {
                return found->second;
            }

            type = registry->GetInstance()->GetClass( type->m_Base );
        }
    }

    return kNull;
}

bool ComponentCollection::SetComponent(const ComponentPtr& component, bool validate, tstring* error )
{
    HELIUM_ASSERT( component->GetSlot() != Reflect::ReservedTypes::Invalid );

    M_Component::const_iterator found = m_Components.find( component->GetSlot() );
    if (found != m_Components.end() && found->second == component)
    {
        return true; // nothing to do, this is already in the collection
    }

    tstring errorMessage;
    if ( validate && !ValidateComponent( component, errorMessage ) )
    {
        if ( error )
        {
            *error = tstring( TXT( "Component '" ) ) + component->GetClass()->m_Name + TXT( "' is not valid for collection '" ) + GetClass()->m_Name + TXT( "': " ) + errorMessage;
        }
        
        return false;
    }

    // Event args
    ComponentCollectionChanged args ( this, component );

    // Set the component and connect the collection
    m_Components[ component->GetSlot() ] = component;
    component->SetCollection( this );

    // Start caring about change to the component
    component->AddChangedListener( ElementChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );

    // Raise event
    m_Modified = true;
    m_ComponentAdded.Raise( args );
    return true;
}

bool ComponentCollection::RemoveComponent( int32_t slotID )
{
    HELIUM_ASSERT( slotID != Reflect::ReservedTypes::Invalid );

    M_Component::iterator found = m_Components.find(slotID);
    if (found == m_Components.end())
    {
        return false;
    }

    // Hold a pointer to the component so that it doesn't get deleted immediately
    // when it's removed from the collection (so that listeners can inspect its
    // values in callbacks).
    Component::ComponentPtr component = found->second;

    // Event args
    ComponentCollectionChanged args ( this, component ); 

    // Stop caring about changes to the component
    component->RemoveChangedListener( ElementChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );

    // Remove component and reset collection pointer
    m_Components.erase( found );
    component->SetCollection( NULL );

    // Raise event
    m_Modified = true;
    m_ComponentRemoved.Raise( args );
    return true;
}

bool ComponentCollection::ContainsComponent( int32_t slotID ) const
{
    HELIUM_ASSERT( slotID != Reflect::ReservedTypes::Invalid );

    return ComponentCollection::GetComponent( slotID ).ReferencesObject();
}

bool ComponentCollection::ValidateComponent( const ComponentPtr &component, tstring& error ) const
{
    HELIUM_ASSERT( component->GetSlot() != Reflect::ReservedTypes::Invalid );

    // Check for duplicates.
    if ( ContainsComponent( component->GetSlot() ) )
    {
        error = tstring( TXT( "The component '" ) )+ component->GetClass()->m_UIName + TXT( "' is a duplicate (a component already occupies that slot in the collection)." );
        return false;
    }

    // Check to make sure this type of collection accepts this type of component.
    if ( !ValidateCompatible( component, error ) )
    {
        return false;
    }

    // Check to make sure that each component already within the collection is valid with the new one.
    M_Component::const_iterator itr = m_Components.begin();
    M_Component::const_iterator end = m_Components.end();
    for ( ; itr != end; ++itr )
    {
        // Check both directions so that the validation rule only has to be implemented in one place.
        if ( !itr->second->ValidateSibling( component, error ) || !component->ValidateSibling( itr->second.Ptr(), error ) )
        {
            return false;
        }
    }

    return true;
}

bool ComponentCollection::ValidateCompatible( const ComponentPtr& component, tstring& error ) const
{
    HELIUM_ASSERT( component->GetSlot() != Reflect::ReservedTypes::Invalid );

    if ( component->GetComponentBehavior() == ComponentBehaviors::Exclusive )
    {
        error = component->GetClass()->m_UIName + TXT( " cannot be added to a(n) " ) + GetClass()->m_UIName + TXT( " because it is an exclusive component." );
        return false;
    }

    return true;
}

bool ComponentCollection::ValidatePersistent( const ComponentPtr& component ) const
{
    HELIUM_ASSERT( component->GetSlot() != Reflect::ReservedTypes::Invalid );

    // by default, all attributes are persistent
    return true;
}

bool ComponentCollection::IsSubset( const ComponentCollection* collection ) const
{
    M_Component::const_iterator itr = collection->GetComponents().begin();
    M_Component::const_iterator end = collection->GetComponents().end();
    for ( ; itr != end; ++itr )
    {
        if ( itr->second->m_IsEnabled && !ContainsComponent( itr->second->GetSlot() ) )
        {
            return false;
        }
    }

    return true;
}

void ComponentCollection::ComponentChanged( const ComponentBase* component )
{
    ComponentCollectionChanged changed(this, component); 
    m_SingleComponentChanged.Raise(changed);

    // For now, changing an component will fire that the collection itself has changed.
    // This is because the Editor Scene UI exposes component members as part of the collection,
    // an there is not a persistent object wrapping the component.  Therefore, changes to the
    // component can only be detected on the collection itself.
    RaiseChanged( GetClass()->FindField( &ComponentCollection::m_Components ) );
}

bool ComponentCollection::ProcessComponent(ElementPtr element, const tstring& fieldName)
{
    if ( fieldName == TXT( "m_Components" ) )
    {
        V_Component attributes;
        Data::GetValue( Reflect::AssertCast<Reflect::Data>( element ), (std::vector< ElementPtr >&)attributes );

        for ( V_Component::const_iterator itr = attributes.begin(), end = attributes.end();
            itr != end;
            ++itr )
        {
            HELIUM_ASSERT( (*itr)->GetSlot() != Reflect::ReservedTypes::Invalid );

            if ( (*itr)->GetSlot() != Reflect::ReservedTypes::Invalid )
            {
                m_Components[ (*itr)->GetSlot() ] = *itr;
            }
        }

        return true;
    }

    return __super::ProcessComponent(element, fieldName);
}

void ComponentCollection::PreSerialize()
{
    __super::PreSerialize();

    // if you hit this somehow we inserted something into the component collection with the invalid type id, there is a bug somewhere
    HELIUM_ASSERT( m_Components.find( Reflect::ReservedTypes::Invalid ) == m_Components.end() ); 

    // this *must* be junk
    m_Components.erase( Reflect::ReservedTypes::Invalid );
}

void ComponentCollection::PostDeserialize()
{
    __super::PostDeserialize();

    // this *must* be junk
    m_Components.erase( Reflect::ReservedTypes::Invalid );

    M_Component::const_iterator itr = m_Components.begin();
    M_Component::const_iterator end = m_Components.end();
    for ( ; itr != end; ++itr )
    {
        itr->second->SetCollection( this );
        itr->second->AddChangedListener( ElementChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ElementChangeArgs& )> (this, &ComponentCollection::ComponentChanged));
    }
}

void ComponentCollection::CopyTo(const Reflect::ElementPtr& destination)
{
    __super::CopyTo( destination );

    ComponentCollection* destCollection = Reflect::ObjectCast< ComponentCollection >( destination );
    if ( destCollection )
    {
        // Remove all attributes, we're going to bring them over manually
        destCollection->Clear(); 

        // For each component in this component collection
        Reflect::Registry* registry = Reflect::Registry::GetInstance();
        M_Component::const_iterator attrItr = m_Components.begin();
        M_Component::const_iterator attrEnd = m_Components.end();
        for ( ; attrItr != attrEnd; ++attrItr )
        {
            // Create a new copy of the component and try to add it to the destination
            const ComponentPtr& attrib = attrItr->second;
            ComponentPtr destAttrib = Reflect::AssertCast< ComponentBase >( registry->CreateInstance( attrib->GetClass() ) );
            if ( !CopyComponentTo( *destCollection, destAttrib, attrib ) )
            {
                // Component could not be added to the destination collection, check sibling classes
                const std::set<tstring>& derived = ( registry->GetClass( attrib->GetClass()->m_Base ) )->m_Derived;
                std::set<tstring>::const_iterator derivedItr = derived.begin();
                std::set<tstring>::const_iterator derivedEnd = derived.end();
                for ( ; derivedItr != derivedEnd; ++derivedItr )
                {
                    const Reflect::Class* currentType = Reflect::Registry::GetInstance()->GetClass(*derivedItr);
                    if ( currentType->m_TypeID != attrib->GetType() )
                    {
                        destAttrib = Reflect::AssertCast< ComponentBase >( registry->CreateInstance( currentType ) );
                        if ( destAttrib.ReferencesObject() )
                        {
                            if ( CopyComponentTo( *destCollection, destAttrib, attrib ) )
                            {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

bool ComponentCollection::CopyComponentTo( ComponentCollection& destCollection, const ComponentPtr& destAttrib, const ComponentPtr& srcAttrib )
{
    bool inserted = false;
    Reflect::Registry* registry = Reflect::Registry::GetInstance();

    tstring unused;
    // If there is already an component in the destination slot, or the
    // component is not in the destination, but is allowed to be...
    if ( destCollection.ValidateComponent( destAttrib, unused ) )
    {
        // Component can be added to the destination collection, so do it!
        srcAttrib->CopyTo( destAttrib );
        destCollection.SetComponent( destAttrib, false );
        inserted = true;
    }
    else
    {
        ComponentPtr existing = destCollection.GetComponent( destAttrib->GetSlot() );
        if ( existing.ReferencesObject() )
        {
            destCollection.RemoveComponent( existing->GetSlot() );
            if ( destCollection.ValidateComponent( destAttrib, unused ) )
            {
                srcAttrib->CopyTo( destAttrib );
                destCollection.SetComponent( destAttrib, false );
                inserted = true;
            }
            else
            {
                destCollection.SetComponent( existing, false );
            }
        }
    }

    return inserted;
}

#include "FoundationPch.h"
#include "ComponentCollection.h"
#include "Foundation/Component/ComponentExceptions.h"

#include "Foundation/Container/Insert.h" 

using Helium::Insert; 

using namespace Helium;
using namespace Helium::Component;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT(ComponentCollection)

void ComponentCollection::PopulateComposite( Reflect::Composite& comp )
{
    comp.AddField( &ComponentCollection::m_Components, TXT( "m_Components" ) );
}

ComponentCollection::ComponentCollection()
: m_Modified( false )
{
}

ComponentCollection::ComponentCollection( const ComponentPtr& component )
{
    HELIUM_ASSERT( component->GetSlot() != NULL );

    m_Components.insert( M_Component::value_type( component->GetSlot(), component ) );
    component->e_Changed.Add( ObjectChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ObjectChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );
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
        attrItr->second->e_Changed.Remove( ObjectChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ObjectChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );
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

const ComponentPtr& ComponentCollection::GetComponent(const Reflect::Class* slotClass) const
{
    static const ComponentPtr kNull;
    const M_Component::const_iterator end = m_Components.end();
    M_Component::const_iterator found = m_Components.find( slotClass );
    if ( found != end )
    {
        return found->second;
    }
    else
    {
        // Travel up the inheritance hierarchy looking for a base class slot within
        // this collection.
        Reflect::Registry* registry = Reflect::Registry::GetInstance();
        const Reflect::Class* type = slotClass;
        type = Reflect::ReflectionCast< const Class >( type->m_Base );

        // While we have base class type information, and we haven't hit the Component
        // base class, keep iterating.
        while ( type && ( type != Reflect::GetClass< ComponentBase >() ) )
        {
            // See if the base class has a slot in this collection.
            found = m_Components.find( type );
            if ( found != end )
            {
                return found->second;
            }

            type = Reflect::ReflectionCast< const Class >( type->m_Base );
        }
    }

    return kNull;
}

bool ComponentCollection::SetComponent(const ComponentPtr& component, bool validate, tstring* error )
{
    HELIUM_ASSERT( component->GetSlot() != NULL );

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
            tstring componentName;
            Helium::ConvertString( component->GetClass()->m_Name, componentName );

            tstring collectionName;
            Helium::ConvertString( GetClass()->m_Name, collectionName );

            *error = tstring( TXT( "Component '" ) ) + componentName + TXT( "' is not valid for collection '" ) + collectionName + TXT( "': " ) + errorMessage;
        }
        
        return false;
    }

    // Event args
    ComponentCollectionChanged args ( this, component );

    // Set the component and connect the collection
    m_Components[ component->GetSlot() ] = component;
    component->SetCollection( this );

    // Start caring about change to the component
    component->e_Changed.Add( ObjectChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ObjectChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );

    // Raise event
    m_Modified = true;
    m_ComponentAdded.Raise( args );
    return true;
}

bool ComponentCollection::RemoveComponent( const Reflect::Class* slotClass )
{
    HELIUM_ASSERT( slotClass != NULL );

    M_Component::iterator found = m_Components.find(slotClass);
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
    component->e_Changed.Remove( ObjectChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ObjectChangeArgs& )> ( this, &ComponentCollection::ComponentChanged ) );

    // Remove component and reset collection pointer
    m_Components.erase( found );
    component->SetCollection( NULL );

    // Raise event
    m_Modified = true;
    m_ComponentRemoved.Raise( args );
    return true;
}

bool ComponentCollection::ContainsComponent( const Reflect::Class* slotClass ) const
{
    HELIUM_ASSERT( slotClass != NULL );

    return ComponentCollection::GetComponent( slotClass ).ReferencesObject();
}

bool ComponentCollection::ValidateComponent( const ComponentPtr &component, tstring& error ) const
{
    HELIUM_ASSERT( component->GetSlot() != NULL );

    // Check for duplicates.
    if ( ContainsComponent( component->GetSlot() ) )
    {
        tstring name;
        Helium::ConvertString( component->GetClass()->m_Name, name );
        error = tstring( TXT( "The component '" ) )+ name + TXT( "' is a duplicate (a component already occupies that slot in the collection)." );
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
    HELIUM_ASSERT( component->GetSlot() != NULL );

    if ( component->GetComponentBehavior() == ComponentBehaviors::Exclusive )
    {
        error = *component->GetClass()->m_Name;
        error += TXT( " cannot be added to a(n) " );
        error += *GetClass()->m_Name;
        error += TXT( " because it is an exclusive component." );
        return false;
    }

    return true;
}

bool ComponentCollection::ValidatePersistent( const ComponentPtr& component ) const
{
    HELIUM_ASSERT( component->GetSlot() != NULL );

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

void ComponentCollection::PreSerialize( const Reflect::Field* field )
{
    Base::PreSerialize( field );

    if ( field == NULL )
    {
        // if you hit this somehow we inserted something into the component collection with the invalid type id, there is a bug somewhere
        HELIUM_ASSERT( m_Components.find( NULL ) == m_Components.end() ); 

        // this *must* be junk
        m_Components.erase( Reflect::TypeID( NULL ) );
    }
}

void ComponentCollection::PostDeserialize( const Reflect::Field* field )
{
    Base::PostDeserialize( field );

    if ( field == NULL )
    {
        // this *must* be junk
        m_Components.erase( Reflect::TypeID( NULL ) );

        M_Component::const_iterator itr = m_Components.begin();
        M_Component::const_iterator end = m_Components.end();
        for ( ; itr != end; ++itr )
        {
            itr->second->SetCollection( this );
            itr->second->e_Changed.Add( ObjectChangeSignature::Delegate::Create<ComponentCollection, void (ComponentCollection::*)( const Reflect::ObjectChangeArgs& )> (this, &ComponentCollection::ComponentChanged));
        }
    }
}

void ComponentCollection::CopyTo(Reflect::Object* object)
{
    Base::CopyTo( object );

    ComponentCollection* collection = Reflect::SafeCast< ComponentCollection >( object );
    if ( collection )
    {
        // Remove all attributes, we're going to bring them over manually
        collection->Clear(); 

        // For each component in this component collection
        Reflect::Registry* registry = Reflect::Registry::GetInstance();
        M_Component::const_iterator attrItr = m_Components.begin();
        M_Component::const_iterator attrEnd = m_Components.end();
        for ( ; attrItr != attrEnd; ++attrItr )
        {
            // Create a new copy of the component and try to add it to the destination
            const ComponentPtr& attrib = attrItr->second;
            ComponentPtr destAttrib = Reflect::AssertCast< ComponentBase >( registry->CreateInstance( attrib->GetClass() ) );
            if ( !CopyComponentTo( *collection, destAttrib, attrib ) )
            {
                // Component could not be added to the destination collection, check sibling classes
                for ( const Composite* sibling = attrib->GetClass()->m_Base->m_FirstDerived; sibling; sibling = sibling->m_NextSibling )
                {
                    if ( sibling != attrib->GetClass() )
                    {
                        destAttrib = Reflect::AssertCast< ComponentBase >( registry->CreateInstance( Reflect::ReflectionCast< const Class >( sibling ) ) );
                        if ( destAttrib.ReferencesObject() )
                        {
                            if ( CopyComponentTo( *collection, destAttrib, attrib ) )
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

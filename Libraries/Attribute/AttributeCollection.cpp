#include "AttributeCollection.h"
#include "Exceptions.h"

#include "Foundation/Container/Insert.h" 

using Nocturnal::Insert; 

using namespace Reflect;
using namespace Attribute;

REFLECT_DEFINE_CLASS(AttributeCollection)

void AttributeCollection::EnumerateClass( Reflect::Compositor<AttributeCollection>& comp )
{
  Reflect::Field* fieldAttributesByType = comp.AddField( &AttributeCollection::m_Attributes, "m_Attributes" );
}

AttributeCollection::AttributeCollection()
: m_Modified( false )
{
}

AttributeCollection::AttributeCollection( const AttributePtr& attr )
{
  NOC_ASSERT( attr->GetSlot() != Reflect::ReservedTypes::Invalid );

  m_Attributes.insert( M_Attribute::value_type( attr->GetSlot(), attr ) );
  attr->AddChangedListener( ElementChangeSignature::Delegate::Create<AttributeCollection, void (AttributeCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &AttributeCollection::AttributeChanged ) );
  m_Modified = true;
}

AttributeCollection::~AttributeCollection()
{
  // it is possible for attributes to survive the collection
  // so we must remove our registered event handlers from the attributes
  //
  // we do it manually because i don't want to raise a whole lot of events
  // by calling Clear()
  // 
  M_Attribute::iterator attrItr = m_Attributes.begin(); 
  M_Attribute::iterator attrEnd = m_Attributes.end(); 

  for( ; attrItr != attrEnd; ++attrItr )
  {
    attrItr->second->RemoveChangedListener( ElementChangeSignature::Delegate::Create<AttributeCollection, void (AttributeCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &AttributeCollection::AttributeChanged ) );
  }
}

void AttributeCollection::Clear()
{
  // Make a copy of the list that we are going to iterate over, since m_Attributes
  // is going to be changing as we iterate.
  M_Attribute copy = m_Attributes;
  M_Attribute::iterator attrItr = copy.begin();
  M_Attribute::iterator attrEnd = copy.end();
  for( ; attrItr != attrEnd; ++attrItr )
  {
    RemoveAttribute( attrItr->second->GetSlot() );
  }
}

const M_Attribute& AttributeCollection::GetAttributes() const
{
  return m_Attributes;
}

const AttributePtr& AttributeCollection::GetAttribute(i32 slotID) const
{
  static const AttributePtr kNull;
  const M_Attribute::const_iterator end = m_Attributes.end();
  M_Attribute::const_iterator found = m_Attributes.find( slotID );
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

    // While we have base class type information, and we haven't hit the Attribute
    // base class, keep iterating.
    while ( type && ( type->m_TypeID != Reflect::GetType< AttributeBase >() ) )
    {
      // See if the base class has a slot in this collection.
      found = m_Attributes.find( type->m_TypeID );
      if ( found != end )
      {
        return found->second;
      }

      type = registry->GetInstance()->GetClass( type->m_Base );
    }
  }

  return kNull;
}

void AttributeCollection::SetAttribute(const AttributePtr& attr, bool validate)
{
  NOC_ASSERT( attr->GetSlot() != Reflect::ReservedTypes::Invalid );

  M_Attribute::const_iterator found = m_Attributes.find( attr->GetSlot() );
  if (found != m_Attributes.end() && found->second == attr)
  {
    return; // nothing to do, this is already in the collection
  }

  // this will prevent inappropriate attributes from being added by throwing an exception
  std::string error;
  if ( validate && !ValidateAttribute( attr, error ) )
  {
    if ( error.empty() )
    {
      throw Attribute::Exception( "Attribute '%s' is not valid for collection '%s'", attr->GetClass()->m_ShortName.c_str(), GetClass()->m_ShortName.c_str() );
    }
    else
    {
      throw Attribute::Exception( "Attribute '%s' is not valid for collection '%s': %s", attr->GetClass()->m_ShortName.c_str(), GetClass()->m_ShortName.c_str(), error.c_str() );
    }
  }

  // Event args
  AttributeCollectionChanged args ( this, attr );

  // Set the attribute and connect the collection
  m_Attributes[ attr->GetSlot() ] = attr;
  attr->SetCollection( this );

  // Start caring about change to the attribute
  attr->AddChangedListener( ElementChangeSignature::Delegate::Create<AttributeCollection, void (AttributeCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &AttributeCollection::AttributeChanged ) );

  // Raise event
  m_Modified = true;
  m_AttributeAdded.Raise( args ); 
}

void AttributeCollection::RemoveAttribute(i32 slotID)
{
  NOC_ASSERT( slotID != Reflect::ReservedTypes::Invalid );

  M_Attribute::iterator found = m_Attributes.find(slotID);
  if (found == m_Attributes.end())
  {
    return;
  }

  // Hold a pointer to the attribute so that it doesn't get deleted immediately
  // when it's removed from the collection (so that listeners can inspect its
  // values in callbacks).
  Attribute::AttributePtr attr = found->second;

  // Event args
  AttributeCollectionChanged args ( this, attr ); 

  // Stop caring about changes to the attribute
  attr->RemoveChangedListener( ElementChangeSignature::Delegate::Create<AttributeCollection, void (AttributeCollection::*)( const Reflect::ElementChangeArgs& )> ( this, &AttributeCollection::AttributeChanged ) );

  // Remove attribute and reset collection pointer
  m_Attributes.erase( found );
  attr->SetCollection( NULL );

  // Raise event
  m_Modified = true;
  m_AttributeRemoved.Raise( args );
}

bool AttributeCollection::ContainsAttribute( i32 slotID ) const
{
  NOC_ASSERT( slotID != Reflect::ReservedTypes::Invalid );

  return AttributeCollection::GetAttribute( slotID ).ReferencesObject();
}

bool AttributeCollection::ValidateAttribute( const AttributePtr &attr, std::string& error ) const
{
  NOC_ASSERT( attr->GetSlot() != Reflect::ReservedTypes::Invalid );

  // Check for duplicates.
  if ( ContainsAttribute( attr->GetSlot() ) )
  {
    error = "The attribute '" + attr->GetClass()->m_UIName + "' is a duplicate (an attribute already occupies that slot in the collection).";
    return false;
  }

  // Check to make sure this type of collection accepts this type of attribute.
  if ( !ValidateCompatible( attr, error ) )
  {
    return false;
  }

  // Check to make sure that each attribute already within the collection is valid with the new one.
  M_Attribute::const_iterator itr = m_Attributes.begin();
  M_Attribute::const_iterator end = m_Attributes.end();
  for ( ; itr != end; ++itr )
  {
    // Check both directions so that the validation rule only has to be implemented in one place.
    if ( !itr->second->ValidateSibling( attr, error ) || !attr->ValidateSibling( itr->second.Ptr(), error ) )
    {
      return false;
    }
  }

  return true;
}

bool AttributeCollection::ValidateCompatible( const AttributePtr& attr, std::string& error ) const
{
  NOC_ASSERT( attr->GetSlot() != Reflect::ReservedTypes::Invalid );

  if ( attr->GetAttributeBehavior() == AttributeBehaviors::Exclusive )
  {
    error = attr->GetClass()->m_UIName + " cannot be added to a " + GetClass()->m_UIName + " because it is an exclusive attribute.";
    return false;
  }

  return true;
}

bool AttributeCollection::ValidatePersistent( const AttributePtr& attr ) const
{
  NOC_ASSERT( attr->GetSlot() != Reflect::ReservedTypes::Invalid );

  // by default, all attributes are persistent
  return true;
}

bool AttributeCollection::IsSubset( const AttributeCollection* collection ) const
{
  M_Attribute::const_iterator itr = collection->GetAttributes().begin();
  M_Attribute::const_iterator end = collection->GetAttributes().end();
  for ( ; itr != end; ++itr )
  {
    if ( itr->second->m_Enabled && !ContainsAttribute( itr->second->GetSlot() ) )
    {
      return false;
    }
  }
  
  return true;
}

void AttributeCollection::AttributeChanged( const AttributeBase* attr )
{
  AttributeCollectionChanged changed(this, attr); 
  m_SingleAttributeChanged.Raise(changed);

  // For now, changing an attribute will fire that the collection itself has changed.
  // This is because the Luna Scene UI exposes attribute members as part of the collection,
  // an there is not a persistent object wrapping the attribute.  Therefore, changes to the
  // attribute can only be detected on the collection itself.
  RaiseChanged( GetClass()->FindField( &AttributeCollection::m_Attributes ) );
}

bool AttributeCollection::ProcessComponent(ElementPtr element, const std::string& fieldName)
{
  if ( fieldName == "m_Attributes" )
  {
    V_Attribute attributes;
    Serializer::GetValue( Reflect::AssertCast<Reflect::Serializer>( element ), (V_Element&)attributes );

    for ( V_Attribute::const_iterator itr = attributes.begin(), end = attributes.end();
          itr != end;
          ++itr )
    {
      NOC_ASSERT( (*itr)->GetSlot() != Reflect::ReservedTypes::Invalid );

      if ( (*itr)->GetSlot() != Reflect::ReservedTypes::Invalid )
      {
        m_Attributes[ (*itr)->GetSlot() ] = *itr;
      }
    }

    return true;
  }

  return __super::ProcessComponent(element, fieldName);
}

void AttributeCollection::PreSerialize()
{
  __super::PreSerialize();

  // if you hit this somehow we inserted something into the attribute collection with the invalid type id, there is a bug somewhere
  NOC_ASSERT( m_Attributes.find( Reflect::ReservedTypes::Invalid ) == m_Attributes.end() ); 

  // this *must* be junk
  m_Attributes.erase( Reflect::ReservedTypes::Invalid );
}

void AttributeCollection::PostDeserialize()
{
  __super::PostDeserialize();

  // this *must* be junk
  m_Attributes.erase( Reflect::ReservedTypes::Invalid );

  M_Attribute::const_iterator itr = m_Attributes.begin();
  M_Attribute::const_iterator end = m_Attributes.end();
  for ( ; itr != end; ++itr )
  {
    itr->second->SetCollection( this );
    itr->second->AddChangedListener( ElementChangeSignature::Delegate::Create<AttributeCollection, void (AttributeCollection::*)( const Reflect::ElementChangeArgs& )> (this, &AttributeCollection::AttributeChanged));
  }
}

void AttributeCollection::CopyTo(const Reflect::ElementPtr& destination)
{
  __super::CopyTo( destination );

  AttributeCollection* destCollection = Reflect::ObjectCast< AttributeCollection >( destination );
  if ( destCollection )
  {
    // Remove all attributes, we're going to bring them over manually
    destCollection->Clear(); 

    // For each attribute in this attribute collection
    Reflect::Registry* registry = Reflect::Registry::GetInstance();
    M_Attribute::const_iterator attrItr = m_Attributes.begin();
    M_Attribute::const_iterator attrEnd = m_Attributes.end();
    for ( ; attrItr != attrEnd; ++attrItr )
    {
      // Create a new copy of the attribute and try to add it to the destination
      const AttributePtr& attrib = attrItr->second;
      AttributePtr destAttrib = Reflect::AssertCast< AttributeBase >( registry->CreateInstance( attrib->GetClass() ) );
      if ( !CopyAttributeTo( destCollection, destAttrib, attrib ) )
      {
        // Attribute could not be added to the destination collection, check sibling classes
        const std::set<std::string>& derived = ( registry->GetClass( attrib->GetClass()->m_Base ) )->m_Derived;
        std::set<std::string>::const_iterator derivedItr = derived.begin();
        std::set<std::string>::const_iterator derivedEnd = derived.end();
        for ( ; derivedItr != derivedEnd; ++derivedItr )
        {
          const Reflect::Class* currentType = Reflect::Registry::GetInstance()->GetClass(*derivedItr);
          if ( currentType->m_TypeID != attrib->GetType() )
          {
            destAttrib = Reflect::AssertCast< AttributeBase >( registry->CreateInstance( currentType ) );
            if ( destAttrib.ReferencesObject() )
            {
              if ( CopyAttributeTo( destCollection, destAttrib, attrib ) )
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

bool AttributeCollection::CopyAttributeTo( const AttributeCollectionPtr& destCollection, const AttributePtr& destAttrib, const AttributePtr& srcAttrib )
{
  bool inserted = false;
  Reflect::Registry* registry = Reflect::Registry::GetInstance();

  std::string unused;
  // If there is already an attribute in the destination slot, or the
  // attribute is not in the destination, but is allowed to be...
  if ( destCollection->ValidateAttribute( destAttrib, unused ) )
  {
    // Attribute can be added to the destination collection, so do it!
    srcAttrib->CopyTo( destAttrib );
    destCollection->SetAttribute( destAttrib, false );
    inserted = true;
  }
  else
  {
    AttributePtr existing = destCollection->GetAttribute( destAttrib->GetSlot() );
    if ( existing.ReferencesObject() )
    {
      destCollection->RemoveAttribute( existing->GetSlot() );
      if ( destCollection->ValidateAttribute( destAttrib, unused ) )
      {
        srcAttrib->CopyTo( destAttrib );
        destCollection->SetAttribute( destAttrib, false );
        inserted = true;
      }
      else
      {
        destCollection->SetAttribute( existing, false );
      }
    }
  }

  return inserted;
}

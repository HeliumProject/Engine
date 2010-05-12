#include "Entity.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/AssetClass.h"
#include "Asset/EntityAsset.h"
#include "Asset/ArtFileAttribute.h"

#include "Console/Console.h"
#include "Content/Scene.h"
#include "File/Manager.h"
#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "FileSystem/FileSystem.h"
#include "RCS/RCS.h"

using namespace Asset;
using namespace Attribute;
using namespace Content;
using namespace File;

REFLECT_DEFINE_CLASS(Entity);

void Entity::EnumerateClass( Reflect::Compositor<Entity>& comp )
{
  Reflect::Field* fieldClassID = comp.AddField( &Entity::m_ClassID, "m_ClassID" );
  Reflect::Field* fieldShowPointer = comp.AddField( &Entity::m_ShowPointer, "m_ShowPointer" );
  Reflect::Field* fieldShowBounds = comp.AddField( &Entity::m_ShowBounds, "m_ShowBounds" );
  Reflect::Field* fieldShowGeometry = comp.AddField( &Entity::m_ShowGeometry, "m_ShowGeometry" );
}

Entity::Entity ()
: m_ClassID (TUID::Null)
, m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
{
  UseCachedClass(true);
}

Entity::Entity (const UniqueID::TUID& id)
: Instance (id)
, m_ClassID (TUID::Null)
, m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
{
  UseCachedClass(true);
}

Entity::Entity (const tuid& classID)
: m_ClassID (classID)
, m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
{
  UseCachedClass(true);
}

Entity::Entity (const UniqueID::TUID& id, const tuid& classID)
: Instance (id)
, m_ClassID (classID)
, m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
{
  UseCachedClass(true);
}

tuid Entity::GetEntityAssetID() const
{
  return m_ClassID;
}

void Entity::SetEntityAssetID( const tuid& newID )
{
  m_ClassID = newID;
  m_ClassPath.clear();
}

EntityAssetPtr Entity::GetEntityAsset() const
{
  if ( m_ClassID != TUID::Null )
  {
    return m_ClassFinder.Invoke( m_ClassID );
  }
  else
  {
    return NULL;
  }
}

const std::string& Entity::GetEntityAssetPath()
{
  if (m_ClassPath.empty())
  {
    if ( !File::GlobalManager().GetPath( m_ClassID, m_ClassPath ) )
    {
      throw Nocturnal::Exception( "Could not locate a file with id: "TUID_HEX_FORMAT" when attempting to get a class path.", m_ClassID );
    }
  }

  return m_ClassPath;
}

void Entity::UseCachedClass(bool useCache)
{
  if (useCache)
  {
    m_ClassFinder.Set( &AssetClass::GetCachedAssetClass<EntityAsset> );
  }
  else
  {
    m_ClassFinder.Set( &AssetClass::GetSavedAssetClass<EntityAsset> );
  }
}

bool Entity::ValidatePersistent( const Attribute::AttributePtr& attr ) const
{
  if ( m_ClassID != TUID::Null )
  {
    AssetClassPtr entityClass = GetEntityAsset();

    if ( entityClass.ReferencesObject() )
    {
      // if the value of the attribute we are setting to is the default value, don't set the attribute, and attempt to remove it if it exists in the Entity
      if ( attr->GetAttributeUsage() == AttributeUsages::Overridable )
      {
        AttributePtr classAttr = entityClass->GetAttribute( attr->GetType() );

        if ( attr->Equals( classAttr ) )
        {
          return false;
        }
      }
    }
  }

  return __super::ValidatePersistent(attr);
}

const AttributePtr& Entity::GetAttribute(i32 typeID) const
{
  // try to get the attribute from the Entity
  const AttributePtr &instAttr = __super::GetAttribute( typeID );

  if ( m_ClassID != TUID::Null )
  {
    if ( instAttr )
    {
      AttributeUsage usage = instAttr->GetAttributeUsage();

      // NOTE: this handles the case where the Overridable attribute has been removed from the AssetClass
      if ( usage == AttributeUsages::Overridable )
      {
        AssetClassPtr entityClass = GetEntityAsset();

        if ( entityClass.ReferencesObject() )
        {
          const AttributePtr &classAttr = entityClass->GetAttribute( typeID );

          if ( classAttr == NULL )
          {
            // Fan-fucking-tastic
            const_cast<Entity*>(this)->RemoveAttribute( typeID );
            return classAttr;
          }
        }
      }

      // it's either an Overridable attribute that has been over-riden in the Entity or it's an Instance attribute.
      return instAttr;
    }

    AssetClassPtr entityClass;

    try
    {
      entityClass = GetEntityAsset();
    }
    catch ( const Nocturnal::Exception& ex )
    {
      Console::Warning( "%s\n", ex.what() );
    }

    if ( entityClass.ReferencesObject() )
    {
      return entityClass->GetAttribute( typeID );
    }
  }

  return instAttr;
}

void Entity::SetAttribute(const AttributePtr& attr, bool validate)
{
  if ( m_ClassID != TUID::Null )
  {
    // NOTE: GetAssetClass should ensure that the AssetClass is valid, and throw an exception otherwise
    AssetClassPtr entityClass = GetEntityAsset();

    if ( entityClass.ReferencesObject() )
    {
      // find the attribute of the corresponding type in the asset class
      AttributePtr classAttr = entityClass->GetAttribute( attr->GetType() );

      // make sure we aren't attempting to pass in an attribute with the same address as the corresponding attribute in the EntityAsset attr
      if ( classAttr == attr )
      {
        return;
      }
    }
  }
  
  __super::SetAttribute( attr, validate );
}
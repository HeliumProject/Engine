#include "Entity.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/AssetClass.h"
#include "Asset/EntityAsset.h"
#include "Asset/ArtFileAttribute.h"

#include "Foundation/Log.h"
#include "Content/Scene.h"
#include "RCS/RCS.h"

using namespace Asset;
using namespace Attribute;
using namespace Content;

REFLECT_DEFINE_CLASS(Entity);

void Entity::EnumerateClass( Reflect::Compositor<Entity>& comp )
{
    Reflect::Field* fieldClassPath = comp.AddField( &Entity::m_Path, "m_Path", Reflect::FieldFlags::Path );
    Reflect::Field* fieldShowPointer = comp.AddField( &Entity::m_ShowPointer, "m_ShowPointer" );
    Reflect::Field* fieldShowBounds = comp.AddField( &Entity::m_ShowBounds, "m_ShowBounds" );
    Reflect::Field* fieldShowGeometry = comp.AddField( &Entity::m_ShowGeometry, "m_ShowGeometry" );
}

Entity::Entity( const std::string& assetPath )
: m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
, m_Path( assetPath )
{
}

EntityAssetPtr Entity::GetEntityAsset() const
{
    return AssetClass::LoadAssetClass< EntityAsset >( m_Path );
}

std::string Entity::GetEntityAssetPath()
{
    return m_Path.Get();
}

bool Entity::ValidatePersistent( const Attribute::AttributePtr& attr ) const
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

    return __super::ValidatePersistent(attr);
}

const AttributePtr& Entity::GetAttribute(i32 typeID) const
{
    // try to get the attribute from the Entity
    const AttributePtr &instAttr = __super::GetAttribute( typeID );

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

    AssetClassPtr entityClass = GetEntityAsset();

    if ( entityClass.ReferencesObject() )
    {
        return entityClass->GetAttribute( typeID );
    }

    return instAttr;
}

void Entity::SetAttribute(const AttributePtr& attr, bool validate)
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

    __super::SetAttribute( attr, validate );
}
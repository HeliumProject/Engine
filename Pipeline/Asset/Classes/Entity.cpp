#include "Entity.h"

#include "Pipeline/Component/ComponentHandle.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Pipeline/Asset/Components/ArtFileComponent.h"

#include "Foundation/Log.h"
#include "Pipeline/Content/Scene.h"

using namespace Asset;
using namespace Component;
using namespace Content;

REFLECT_DEFINE_CLASS(Entity);

void Entity::EnumerateClass( Reflect::Compositor<Entity>& comp )
{
    Reflect::Field* fieldClassPath = comp.AddField( &Entity::m_Path, "m_Path", Reflect::FieldFlags::Path );
    Reflect::Field* fieldShowPointer = comp.AddField( &Entity::m_ShowPointer, "m_ShowPointer" );
    Reflect::Field* fieldShowBounds = comp.AddField( &Entity::m_ShowBounds, "m_ShowBounds" );
    Reflect::Field* fieldShowGeometry = comp.AddField( &Entity::m_ShowGeometry, "m_ShowGeometry" );
}

Entity::Entity( const tstring& assetPath )
: m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
, m_Path( assetPath )
{
}

Entity::Entity( Nocturnal::TUID id )
: Content::Instance ( id )
, m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
{
}

EntityAssetPtr Entity::GetEntityAsset() const
{
    return AssetClass::LoadAssetClass< EntityAsset >( m_Path );
}

tstring Entity::GetEntityAssetPath()
{
    return m_Path.Get();
}

bool Entity::ValidatePersistent( const Component::ComponentPtr& attr ) const
{
    AssetClassPtr entityClass = GetEntityAsset();

    if ( entityClass.ReferencesObject() )
    {
        // if the value of the attribute we are setting to is the default value, don't set the attribute, and attempt to remove it if it exists in the Entity
        if ( attr->GetComponentUsage() == ComponentUsages::Overridable )
        {
            ComponentPtr classAttr = entityClass->GetComponent( attr->GetType() );

            if ( attr->Equals( classAttr ) )
            {
                return false;
            }
        }
    }

    return __super::ValidatePersistent(attr);
}

const ComponentPtr& Entity::GetComponent(i32 typeID) const
{
    // try to get the attribute from the Entity
    const ComponentPtr &instAttr = __super::GetComponent( typeID );

    if ( instAttr )
    {
        ComponentUsage usage = instAttr->GetComponentUsage();

        // NOTE: this handles the case where the Overridable attribute has been removed from the AssetClass
        if ( usage == ComponentUsages::Overridable )
        {
            AssetClassPtr entityClass = GetEntityAsset();

            if ( entityClass.ReferencesObject() )
            {
                const ComponentPtr &classAttr = entityClass->GetComponent( typeID );

                if ( classAttr == NULL )
                {
                    // Fan-fucking-tastic
                    const_cast<Entity*>(this)->RemoveComponent( typeID );
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
        return entityClass->GetComponent( typeID );
    }

    return instAttr;
}

void Entity::SetComponent(const ComponentPtr& attr, bool validate)
{
    // NOTE: GetAssetClass should ensure that the AssetClass is valid, and throw an exception otherwise
    AssetClassPtr entityClass = GetEntityAsset();

    if ( entityClass.ReferencesObject() )
    {
        // find the attribute of the corresponding type in the asset class
        ComponentPtr classAttr = entityClass->GetComponent( attr->GetType() );

        // make sure we aren't attempting to pass in an attribute with the same address as the corresponding attribute in the EntityAsset attr
        if ( classAttr == attr )
        {
            return;
        }
    }

    __super::SetComponent( attr, validate );
}
#include "EntityInstance.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/AssetClass.h"

#include "Foundation/Log.h"
#include "Core/Content/Scene.h"

using namespace Helium;
using namespace Helium::Asset;
using namespace Helium::Component;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(Entity);

void EntityInstance::EnumerateClass( Reflect::Compositor< EntityInstance >& comp )
{
    Reflect::Field* fieldClassPath = comp.AddField( &EntityInstance::m_Path, "m_Path", Reflect::FieldFlags::Path );
    Reflect::Field* fieldShowPointer = comp.AddField( &EntityInstance::m_ShowPointer, "m_ShowPointer" );
    Reflect::Field* fieldShowBounds = comp.AddField( &EntityInstance::m_ShowBounds, "m_ShowBounds" );
    Reflect::Field* fieldShowGeometry = comp.AddField( &EntityInstance::m_ShowGeometry, "m_ShowGeometry" );
}

EntityInstance::EntityInstance( const tstring& assetPath )
: m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
, m_Path( assetPath )
{
}

EntityInstance::EntityInstance( Helium::TUID id )
: Content::Instance ( id )
, m_ShowPointer (true)
, m_ShowBounds (true)
, m_ShowGeometry (false)
{
}

EntityPtr EntityInstance::GetEntity() const
{
    return AssetClass::LoadAssetClass< Entity >( m_Path );
}

tstring EntityInstance::GetEntityPath()
{
    return m_Path.Get();
}

bool EntityInstance::ValidatePersistent( const Component::ComponentPtr& attr ) const
{
    AssetClassPtr entityClass = GetEntity();

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

const ComponentPtr& EntityInstance::GetComponent(i32 typeID) const
{
    // try to get the attribute from the Entity
    const ComponentPtr &instAttr = __super::GetComponent( typeID );

    if ( instAttr )
    {
        ComponentUsage usage = instAttr->GetComponentUsage();

        // NOTE: this handles the case where the Overridable attribute has been removed from the AssetClass
        if ( usage == ComponentUsages::Overridable )
        {
            AssetClassPtr entityClass = GetEntity();

            if ( entityClass.ReferencesObject() )
            {
                const ComponentPtr &classAttr = entityClass->GetComponent( typeID );

                if ( classAttr == NULL )
                {
                    // Fan-fucking-tastic
                    const_cast<EntityInstance*>(this)->RemoveComponent( typeID );
                    return classAttr;
                }
            }
        }

        // it's either an Overridable attribute that has been over-riden in the Entity or it's an Instance attribute.
        return instAttr;
    }

    AssetClassPtr entityClass = GetEntity();

    if ( entityClass.ReferencesObject() )
    {
        return entityClass->GetComponent( typeID );
    }

    return instAttr;
}

bool EntityInstance::SetComponent( const ComponentPtr& attr, bool validate, tstring* error )
{
    // NOTE: GetAssetClass should ensure that the AssetClass is valid, and throw an exception otherwise
    AssetClassPtr entityClass = GetEntity();

    if ( entityClass.ReferencesObject() )
    {
        // find the attribute of the corresponding type in the asset class
        ComponentPtr classAttr = entityClass->GetComponent( attr->GetType() );

        // make sure we aren't attempting to pass in an attribute with the same address as the corresponding attribute in the EntityAsset attr
        if ( classAttr == attr )
        {
            return true;
        }
    }

    return __super::SetComponent( attr, validate, error );
}
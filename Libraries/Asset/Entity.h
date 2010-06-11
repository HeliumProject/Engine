#pragma once

#include "API.h"
#include "Content/Instance.h"
#include "Asset/EntityAsset.h"
#include "Foundation/Automation/Event.h"

namespace Asset
{
    class Entity;
    typedef Nocturnal::SmartPtr<Entity> EntityPtr;
    typedef std::vector<EntityPtr> V_Entity;

    class ASSET_API Entity : public Content::Instance
    {
    private:
        REFLECT_DECLARE_CLASS( Entity, Instance );
        static void EnumerateClass( Reflect::Compositor<Entity>& comp );

    public:
        Entity( const std::string& assetPath = "" );

        // get the path to the class
        std::string GetEntityAssetPath();

        // helper to find the entity class
        Asset::EntityAssetPtr GetEntityAsset() const;

        //
        // AttributeCollection overrides
        //

        virtual bool ValidatePersistent( const Attribute::AttributePtr& attr ) const NOC_OVERRIDE;
        virtual const Attribute::AttributePtr& GetAttribute( i32 typeID ) const NOC_OVERRIDE;
        virtual void SetAttribute( const Attribute::AttributePtr& attr, bool validate = true ) NOC_OVERRIDE;

    private:
        Nocturnal::Path m_Path;

    public:
        // draw bound in editor
        bool m_ShowPointer;

        // draw bound in editor
        bool m_ShowBounds;

        // show geometry in editor
        bool m_ShowGeometry;
    };
}
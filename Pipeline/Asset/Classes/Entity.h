#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/Instance.h"
#include "Pipeline/Asset/Classes/EntityAsset.h"
#include "Foundation/Automation/Event.h"

namespace Asset
{
    class Entity;
    typedef Nocturnal::SmartPtr<Entity> EntityPtr;
    typedef std::vector<EntityPtr> V_Entity;

    class PIPELINE_API Entity : public Content::Instance
    {
    private:
        REFLECT_DECLARE_CLASS( Entity, Instance );
        static void EnumerateClass( Reflect::Compositor<Entity>& comp );

    public:
        Entity( const tstring& assetPath = TXT( "" ) );
        Entity( Nocturnal::TUID id );

        // get the path to the class
        tstring GetEntityAssetPath();

        // helper to find the entity class
        Asset::EntityAssetPtr GetEntityAsset() const;

        //
        // ComponentCollection overrides
        //

        virtual bool ValidatePersistent( const Component::ComponentPtr& attr ) const NOC_OVERRIDE;
        virtual const Component::ComponentPtr& GetComponent( i32 typeID ) const NOC_OVERRIDE;
        virtual void SetComponent( const Component::ComponentPtr& component, bool validate = true ) NOC_OVERRIDE;

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
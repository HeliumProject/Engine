#include "Precompile.h"

#include "ExportEntity.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

using namespace Content;
using namespace MayaContent;

void ExportEntity::GatherMayaData( V_ExportBase &newExportObjects )
{
    MFnDagNode nodeFn (m_MayaObject);

    // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::Volume
    Asset::EntityInstance* entity = Reflect::DangerousCast< Asset::EntityInstance >( m_ContentObject );

    entity->m_DefaultName = nodeFn.name().asTChar();

    // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
    MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), entity->m_GlobalTransform );
    entity->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
    entity->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
    entity->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

    // link it into the hierarchy
    MObject parent = nodeFn.parent(0);
    entity->m_ParentID = Maya::GetNodeID(parent);

#if 0
    // this is a special case, because EntityNodes already have a valid Content object. grab it and add it directly to the content scene
    EntityNode* entityNode = (EntityNode*)(nodeFn.userNode());

    if ( entityNode )
    {
        Asset::Entity* backingEntity = entityNode->GetBackingEntity();
        if ( backingEntity )
        {
            Asset::EntityPtr entity = Reflect::ObjectCast<Asset::Entity>( backingEntity->Clone() );

            entity->PreSerialize();

            m_ExportScene.m_ContentScene.Add( entity );
        }
    }
#endif

    ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
    newExportObjects.push_back( parentTransform );

    __super::GatherMayaData( newExportObjects );
}

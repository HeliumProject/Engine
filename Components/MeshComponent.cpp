
#include "ComponentsPch.h"
#include "Components/MeshComponent.h"
#include "Framework/Entity.h"

using namespace Helium;

HELIUM_DEFINE_COMPONENT(Helium::MeshComponent, 32);

void MeshComponent::PopulateStructure( Reflect::Structure& comp )
{

}

void MeshComponent::Finalize( const MeshComponentDefinition* pDefinition )
{
    //Entity *pEntity = rHasComponents.GetOwningEntity();

    Entity *pEntity = this->m_OwningSet->GetOwningEntity();

    HELIUM_ASSERT(pEntity);
    HELIUM_ASSERT(pEntity->GetSlice());


    if (pDefinition->m_Mesh)
    {
        m_Mesh = pDefinition->m_Mesh;
        TransformComponent *transform = pEntity->FindOneComponent<TransformComponent>();

        if (transform)
        {
            Helium::World *pWorld = pEntity->GetSlice()->GetWorld();
            Attach(pWorld, transform);
        }
    }
}

HELIUM_IMPLEMENT_ASSET(Helium::MeshComponentDefinition, Components, 0);

void MeshComponentDefinition::PopulateStructure( Reflect::Structure& comp )
{
	comp.AddField(&MeshComponentDefinition::m_Mesh, "m_Mesh", Reflect::FieldFlags::Share);
	//comp.AddField(&MeshComponentDefinition::m_OverrideMaterials, "m_OverrideMaterials");
}

#include "Rendering/RVertexDescription.h"
#include "GraphicsTypes/VertexTypes.h"
#include "Graphics/GraphicsScene.h"
#include "Graphics/RenderResourceManager.h"
#include "Framework/World.h"



/// Constructor.
MeshComponent::MeshComponent()
: m_graphicsSceneObjectId( Invalid< size_t >() )
{
}

/// Destructor.
MeshComponent::~MeshComponent()
{

}

/// @copydoc Entity::Attach()
void MeshComponent::Attach(World *pWorld, TransformComponent *pTransformComponent)
{
    if (!pTransformComponent)
    {
        return;
    }

    HELIUM_ASSERT( pWorld );
    HELIUM_ASSERT( IsInvalid( m_graphicsSceneObjectId ) );

    Mesh* pMesh = m_Mesh;
    if( pMesh && pMesh->GetVertexBuffer() && pMesh->GetIndexBuffer() )
    {
        size_t meshSectionCount = pMesh->GetSectionCount();
        if( meshSectionCount != 0 )
        {
            GraphicsScene* pGraphicsScene = pWorld->GetGraphicsScene();
            HELIUM_ASSERT( pGraphicsScene );

            m_graphicsSceneObjectId = pGraphicsScene->AllocateSceneObject();
            HELIUM_ASSERT( IsValid( m_graphicsSceneObjectId ) );

            m_graphicsSceneObjectSubMeshDataIds.Reserve( meshSectionCount );
            m_graphicsSceneObjectSubMeshDataIds.Resize( meshSectionCount );

            for( size_t meshSectionIndex = 0; meshSectionIndex < meshSectionCount; ++meshSectionIndex )
            {
                size_t subMeshId = pGraphicsScene->AllocateSceneObjectSubMeshData( m_graphicsSceneObjectId );
                HELIUM_ASSERT( IsValid( subMeshId ) );
                m_graphicsSceneObjectSubMeshDataIds[ meshSectionIndex ] = subMeshId;
            }
            
            SetNeedsGraphicsSceneObjectUpdate(pTransformComponent);
        }
    }
}

/// @copydoc Entity::Detach()
void MeshComponent::Detach(World *pWorld)
{
    HELIUM_ASSERT( pWorld );
    GraphicsScene* pGraphicsScene = pWorld->GetGraphicsScene();
    HELIUM_ASSERT( pGraphicsScene );

    size_t subMeshIdCount = m_graphicsSceneObjectSubMeshDataIds.GetSize();
    for( size_t subMeshIndex = 0; subMeshIndex < subMeshIdCount; ++subMeshIndex )
    {
        size_t subMeshId = m_graphicsSceneObjectSubMeshDataIds[ subMeshIndex ];
        if( IsValid( subMeshId ) )
        {
            pGraphicsScene->ReleaseSceneObjectSubMeshData( subMeshId );
        }
    }

    m_graphicsSceneObjectSubMeshDataIds.Resize( 0 );

    if( IsValid( m_graphicsSceneObjectId ) )
    {
        pGraphicsScene->ReleaseSceneObject( m_graphicsSceneObjectId );
        SetInvalid( m_graphicsSceneObjectId );
    }
}

/// Set the mesh used by this entity.
///
/// @param[in] pMesh  Mesh to assign.
///
/// @see GetMesh()
void MeshComponent::SetMesh( Mesh* pMesh )
{
    if( m_Mesh.Get() != pMesh )
    {
        m_Mesh = pMesh;
        DeferredReattach();
    }
}

/// Flag the graphics scene object as requiring an update if one exists.
///
/// This is safe to call by an entity during its pre-update.  It should only ever be called by the entity itself.
///
/// @param[in] updateMode  Scene object update mode.
void MeshComponent::SetNeedsGraphicsSceneObjectUpdate(
    TransformComponent *pTransform,
    GraphicsSceneObject::EUpdate updateMode )
{
    if( IsValid( m_graphicsSceneObjectId ) )
    {
        MeshSceneObjectTransform *pSceneObjectTransform = Helium::Components::Allocate<MeshSceneObjectTransform>(*m_OwningSet);
        pSceneObjectTransform->Setup(pTransform, this, updateMode, m_graphicsSceneObjectId);
    }
}

/// Callback used to update graphics scene object information prior to the graphics scene update.
///
/// @param[in] pData         Callback data (pointer to the MeshEntity).
/// @param[in] pScene        Graphics scene to which the object is attached.
/// @param[in] pSceneObject  Graphics scene object to update.
void MeshComponent::GraphicsSceneObjectUpdate(
    MeshComponent* pThis,
    GraphicsScene* pScene,
    TransformComponent *pTransform,
    GraphicsSceneObject::EUpdate,
    size_t graphicsSceneObjectId)
{
    GraphicsSceneObject* pSceneObject = pScene->GetSceneObject( graphicsSceneObjectId );

    HELIUM_ASSERT( pScene );
    HELIUM_ASSERT( pSceneObject );
    
    const Simd::Vector3& rPosition = pTransform->GetPosition();
    Simd::Matrix44 transform(
        Simd::Matrix44::INIT_ROTATION_TRANSLATION,
        pTransform->GetRotation(),
        rPosition);
    pSceneObject->SetTransform( transform );

    Mesh* pMesh = pThis->m_Mesh;

    Simd::AaBox worldBounds( rPosition, rPosition );

    // Only thing remaining if this is a transform-only update is the world bounds, so update it and return.
    if( pSceneObject->GetUpdateMode() == GraphicsSceneObject::UPDATE_TRANSFORM_ONLY )
    {
        if( pMesh )
        {
            worldBounds = pMesh->GetBounds();
            worldBounds.TransformBy( transform );
        }

        pSceneObject->SetWorldBounds( worldBounds );

        return;
    }

    RVertexBuffer* pVertexBuffer = NULL;
    RIndexBuffer* pIndexBuffer = NULL;
    if( pMesh )
    {
        pVertexBuffer = pMesh->GetVertexBuffer();
        pIndexBuffer = pMesh->GetIndexBuffer();

        worldBounds = pMesh->GetBounds();
        worldBounds.TransformBy( transform );
    }

    pSceneObject->SetWorldBounds( worldBounds );

    const DynamicArray< size_t >& rSubMeshDataIds = pThis->m_graphicsSceneObjectSubMeshDataIds;
    size_t subMeshCount = rSubMeshDataIds.GetSize();
    size_t meshSectionCount = 0;

    if( !pVertexBuffer || !pIndexBuffer )
    {
        pSceneObject->SetVertexData( NULL, NULL, 0 );
        pSceneObject->SetIndexBuffer( NULL );
    }
    else
    {
        RenderResourceManager& rResourceManager = RenderResourceManager::GetStaticInstance();

        RVertexDescription* pVertexDescription;
        uint32_t vertexStride;
        if( pMesh->IsSkinned() )
        {
            pVertexDescription = rResourceManager.GetSkinnedMeshVertexDescription();
            vertexStride = static_cast< uint32_t >( sizeof( SkinnedMeshVertex ) );
        }
        else
        {
            pVertexDescription = rResourceManager.GetStaticMeshVertexDescription( 1 );
            vertexStride = static_cast< uint32_t >( sizeof( StaticMeshVertex< 1 > ) );
        }

        pSceneObject->SetVertexData( pVertexBuffer, pVertexDescription, vertexStride );
        pSceneObject->SetIndexBuffer( pIndexBuffer );

        meshSectionCount = pMesh->GetSectionCount();
        if( meshSectionCount > subMeshCount )
        {
            meshSectionCount = subMeshCount;
        }

        uint32_t sectionVertexOffset = 0;
        uint32_t sectionIndexOffset = 0;
        for( size_t meshSectionIndex = 0; meshSectionIndex < meshSectionCount; ++meshSectionIndex )
        {
            GraphicsSceneObject::SubMeshData* pSubMeshData = pScene->GetSceneObjectSubMeshData(
                rSubMeshDataIds[ meshSectionIndex ] );
            HELIUM_ASSERT( pSubMeshData );

            uint32_t vertexCount = pMesh->GetSectionVertexCount( meshSectionIndex );
            uint32_t triangleCount = pMesh->GetSectionTriangleCount( meshSectionIndex );

            pSubMeshData->SetMaterial( pThis->GetMaterial( meshSectionIndex ) );
            pSubMeshData->SetPrimitiveType( RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST );
            pSubMeshData->SetPrimitiveCount( triangleCount );
            pSubMeshData->SetStartVertex( sectionVertexOffset );
            pSubMeshData->SetVertexRange( vertexCount );
            pSubMeshData->SetStartIndex( sectionIndexOffset );

            sectionVertexOffset += vertexCount;
            sectionIndexOffset += triangleCount * 3;
        }
    }

    for( size_t unusedSubMeshIndex = meshSectionCount; unusedSubMeshIndex < subMeshCount; ++unusedSubMeshIndex )
    {
        GraphicsSceneObject::SubMeshData* pSubMeshData = pScene->GetSceneObjectSubMeshData(
            rSubMeshDataIds[ unusedSubMeshIndex ] );
        HELIUM_ASSERT( pSubMeshData );

        pSubMeshData->SetMaterial( NULL );
        pSubMeshData->SetPrimitiveType( RENDERER_PRIMITIVE_TYPE_TRIANGLE_LIST );
        pSubMeshData->SetPrimitiveCount( 0 );
        pSubMeshData->SetStartVertex( 0 );
        pSubMeshData->SetVertexRange( 0 );
        pSubMeshData->SetStartIndex( 0 );
    }
}

void Helium::MeshComponent::Update( World *pWorld, TransformComponent *pTransform )
{
    HELIUM_ASSERT(pWorld);

    if (m_NeedsReattach)
    {
        Detach(pWorld);
        Attach(pWorld, pTransform);
    }

    if (pTransform->IsDirty())
    {
       SetNeedsGraphicsSceneObjectUpdate( pTransform, GraphicsSceneObject::UPDATE_TRANSFORM_ONLY );
    }
}


HELIUM_DEFINE_COMPONENT(MeshSceneObjectTransform, 32);

Helium::MeshSceneObjectTransform::MeshSceneObjectTransform()
{

}

Helium::MeshSceneObjectTransform::MeshSceneObjectTransform( const MeshSceneObjectTransform &rRhs )
{
    m_TransformComponent = rRhs.m_TransformComponent;
    m_MeshComponent = rRhs.m_MeshComponent;
    m_UpdateMode = rRhs.m_UpdateMode;
    m_graphicsSceneObjectId = rRhs.m_graphicsSceneObjectId;
}

void Helium::MeshSceneObjectTransform::Setup( class TransformComponent *pTransform, class MeshComponent *pMesh, GraphicsSceneObject::EUpdate updateMode, size_t graphicsSceneObjectId )
{
    m_TransformComponent = pTransform;
    m_MeshComponent = pMesh;
    m_UpdateMode = updateMode;
    m_graphicsSceneObjectId = graphicsSceneObjectId;
}

void Helium::MeshSceneObjectTransform::GraphicsSceneObjectUpdate( GraphicsScene *pScene )
{
    if (m_MeshComponent.Get() && m_TransformComponent.Get())
    {
        MeshComponent::GraphicsSceneObjectUpdate(m_MeshComponent.Get(), pScene, m_TransformComponent.Get(), m_UpdateMode, m_graphicsSceneObjectId);
    }

    MarkForDeletion();
}

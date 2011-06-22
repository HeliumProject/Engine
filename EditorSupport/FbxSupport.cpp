//----------------------------------------------------------------------------------------------------------------------
// FbxSupport.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EditorSupportPch.h"

#if L_EDITOR

#include "EditorSupport/FbxSupport.h"

#include "Platform/Math/Simd/Vector2.h"
#include "Foundation/StringConverter.h"
#include "Rendering/Color.h"

using namespace Helium;

FbxSupport* FbxSupport::sm_pInstance = NULL;

/// Get the index into the direct array of the specified layer element for a vertex value based on how the layer is
/// mapped.
///
/// @param[in]  pLayerElement       Layer element.
/// @param[in]  controlPointIndex   Index of the control point for the vertex.
/// @param[in]  vertexIndex         Index of the vertex across all polygons.
/// @param[in]  polygonIndex        Index of the polygon to which the vertex belongs.
///
/// @return  Direct array index.
template< typename T >
static int GetLayerElementValueIndex(
     const KFbxLayerElementTemplate< T >* pLayerElement,
     int controlPointIndex,
     int vertexIndex,
     int polygonIndex )
{
    int valueIndex;
    switch( pLayerElement->GetMappingMode() )
    {
        case KFbxLayerElement::eBY_CONTROL_POINT:
        {
            valueIndex = controlPointIndex;
            break;
        }

        case KFbxLayerElement::eBY_POLYGON_VERTEX:
        {
            valueIndex = vertexIndex;
            break;
        }

        case KFbxLayerElement::eBY_POLYGON:
        {
            valueIndex = polygonIndex;
            break;
        }

        case KFbxLayerElement::eALL_SAME:
        {
            valueIndex = 0;
            break;
        }

        default:
        {
            return -1;
        }
    }

    KFbxLayerElement::EReferenceMode referenceMode = pLayerElement->GetReferenceMode();
    if( referenceMode == KFbxLayerElement::eINDEX || referenceMode == KFbxLayerElement::eINDEX_TO_DIRECT )
    {
        KFbxLayerElementArrayTemplate< int >& rIndexArray = pLayerElement->GetIndexArray();
        HELIUM_ASSERT(
            static_cast< unsigned int >( valueIndex ) < static_cast< unsigned int >( rIndexArray.GetCount() ) );
        valueIndex = rIndexArray[ valueIndex ];
    }

    return valueIndex;
}

/// Retrieve a value for a vertex from the specified layer element based on how the layer is mapped.
///
/// @param[in]  pLayerElement       Layer element.
/// @param[in]  controlPointIndex   Index of the control point for the vertex.
/// @param[in]  vertexIndex         Index of the vertex across all polygons.
/// @param[in]  polygonIndex        Index of the polygon to which the vertex belongs.
/// @param[out] rValue              Layer element value.  This will be left unchanged if the value could not be
///                                 read.
template< typename T >
static void GetLayerElementValue(
     const KFbxLayerElementTemplate< T >* pLayerElement,
     int controlPointIndex,
     int vertexIndex,
     int polygonIndex,
     T& rValue )
{
    int valueIndex = GetLayerElementValueIndex( pLayerElement, controlPointIndex, vertexIndex, polygonIndex );
    if( valueIndex != -1 )
    {
        KFbxLayerElementArrayTemplate< T >& rDirectArray = pLayerElement->GetDirectArray();

        HELIUM_ASSERT(
            static_cast< unsigned int >( valueIndex ) < static_cast< unsigned int >( rDirectArray.GetCount() ) );
        rValue = rDirectArray[ valueIndex ];
    }
}

#if L_ENABLE_FBX_MEMORY_ALLOCATOR
/// Constructor.
FbxMemoryAllocator::FbxMemoryAllocator()
    : KFbxMemoryAllocator(
        Malloc,
        Calloc,
        Realloc,
        Free,
        Msize,
        MallocDebug,
        CallocDebug,
        ReallocDebug,
        FreeDebug,
        MsizeDebug )
{
}

/// Memory allocation handler.
///
/// @param[in] size  Number of bytes to allocate.
///
/// @return  Pointer to the allocated memory.
void* FbxMemoryAllocator::Malloc( size_t size )
{
    DynamicMemoryHeap& rExternalHeap = HELIUM_EXTERNAL_HEAP;
    void* pMemory = rExternalHeap.Allocate( size );

    return pMemory;
}

/// Cleared memory allocation handler.
///
/// @param[in] count  Number of elements to allocate.
/// @param[in] size   Number of bytes in each element.
///
/// @return  Pointer to the allocated memory.
void* FbxMemoryAllocator::Calloc( size_t count, size_t size )
{
    size_t byteCount = count * size;

    DynamicMemoryHeap& rExternalHeap = HELIUM_EXTERNAL_HEAP;
    void* pMemory = rExternalHeap.Allocate( byteCount );
    if( pMemory )
    {
        MemoryZero( pMemory, byteCount );
    }

    return pMemory;
}

/// Memory reallocation handler.
///
/// @param[in] pMemory  Block of memory to reallocate.
/// @param[in] size     Number of bytes to which the memory should be resized.
///
/// @return  Pointer to the reallocated memory.
void* FbxMemoryAllocator::Realloc( void* pMemory, size_t size )
{
    DynamicMemoryHeap& rExternalHeap = HELIUM_EXTERNAL_HEAP;
    pMemory = rExternalHeap.Reallocate( pMemory, size );

    return pMemory;
}

/// Free memory handler.
///
/// @param[in] pMemory  Block of memory to free.
void FbxMemoryAllocator::Free( void* pMemory )
{
    DynamicMemoryHeap& rExternalHeap = HELIUM_EXTERNAL_HEAP;
    rExternalHeap.Free( pMemory );
}

/// Memory size handler.
///
/// @param[in] pMemory  Block of allocated memory.
///
/// @return  Number of usable bytes in the given block of memory.
size_t FbxMemoryAllocator::Msize( void* pMemory )
{
    DynamicMemoryHeap& rExternalHeap = HELIUM_EXTERNAL_HEAP;
    size_t size = rExternalHeap.GetMemorySize( pMemory );

    return size;
}

/// Debug memory allocation handler.
///
/// @param[in] size  Number of bytes to allocate.
///
/// @return  Pointer to the allocated memory.
void* FbxMemoryAllocator::MallocDebug( size_t size, int, const char*, int )
{
    return Malloc( size );
}

/// Debug cleared memory allocation handler.
///
/// @param[in] count  Number of elements to allocate.
/// @param[in] size   Number of bytes in each element.
///
/// @return  Pointer to the allocated memory.
void* FbxMemoryAllocator::CallocDebug( size_t count, size_t size, int, const char*, int )
{
    return Calloc( count, size );
}

/// Debug memory reallocation handler.
///
/// @param[in] pMemory  Block of memory to reallocate.
/// @param[in] size     Number of bytes to which the memory should be resized.
///
/// @return  Pointer to the reallocated memory.
void* FbxMemoryAllocator::ReallocDebug( void* pMemory, size_t size, int, const char*, int )
{
    return Realloc( pMemory, size );
}

/// Free memory handler.
///
/// @param[in] pMemory  Block of memory to free.
void FbxMemoryAllocator::FreeDebug( void* pMemory, int )
{
    Free( pMemory );
}

/// Memory size handler.
///
/// @param[in] pMemory  Block of allocated memory.
///
/// @return  Number of usable bytes in the given block of memory.
size_t FbxMemoryAllocator::MsizeDebug( void* pMemory, int )
{
    return Msize( pMemory );
}
#endif  // L_ENABLE_FBX_MEMORY_ALLOCATOR

/// Constructor.
FbxSupport::FbxSupport()
    : m_pSdkManager( NULL )
    , m_pIoSettings( NULL )
    , m_pImporter( NULL )
    , m_referenceCount( 1 )
{
}

/// Convert a matrix parsed from an FBX scene to the axis space of the engine.
///
/// @param[in,out] rTransform  Matrix to convert.
static void FlipTransform( fbxDouble44& rTransform )
{
    rTransform[ 0 ][ 1 ] = -rTransform[ 0 ][ 1 ];
    rTransform[ 0 ][ 2 ] = -rTransform[ 0 ][ 2 ];
    rTransform[ 0 ][ 3 ] = -rTransform[ 0 ][ 3 ];
    rTransform[ 1 ][ 0 ] = -rTransform[ 1 ][ 0 ];
    rTransform[ 2 ][ 0 ] = -rTransform[ 2 ][ 0 ];
    rTransform[ 3 ][ 0 ] = -rTransform[ 3 ][ 0 ];
}

/// Destructor.
FbxSupport::~FbxSupport()
{
    if( m_pSdkManager )
    {
        HELIUM_ASSERT( m_pIoSettings );
        m_pIoSettings->Destroy();

        m_pSdkManager->Destroy();
    }
}

/// Release a reference to the FbxSupport instance previously acquired using StaticAcquire().
///
/// @see StaticAcquire()
void FbxSupport::Release()
{
    int32_t newReferenceCount = AtomicDecrementRelease( m_referenceCount );
    if( newReferenceCount == 0 )
    {
        delete sm_pInstance;
        sm_pInstance = NULL;
    }
}

/// Load mesh data from a supported source file type (i.e. FBX, OBJ).
///
/// @param[in]  rSourceFilePath         Path name of the source file from which to load the mesh.
/// @param[out] rVertices               Mesh vertices.
/// @param[out] rIndices                Triangle vertex indices.
/// @param[in]  rSectionVertexCounts    Number of vertices addressed by each mesh section.
/// @param[out] rSectionTriangleCounts  Number of triangles per mesh section.
/// @param[out] rBones                  Information about each bone in the mesh (if the mesh contains skinning
///                                     data).
/// @param[out] rVertexBlendData        Blend weights and indices for each vertex if the mesh contains skinning
///                                     data.
/// @param[out] rSkinningPaletteMap     Array of maps between bone indices and GPU skinning palette indices for each
///                                     mesh section.
/// @param[in]  bStripNamespaces        True to detect and strip namespaces from the start of each bone name.
///
/// @return  True if loading was successful, false if not.
bool FbxSupport::LoadMesh(
                          const String& rSourceFilePath,
                          DynArray< StaticMeshVertex< 1 > >& rVertices,
                          DynArray< uint16_t >& rIndices,
                          DynArray< uint16_t >& rSectionVertexCounts,
                          DynArray< uint32_t >& rSectionTriangleCounts,
                          DynArray< BoneData >& rBones,
                          DynArray< BlendData >& rVertexBlendData,
                          DynArray< uint8_t >& rSkinningPaletteMap,
                          bool bStripNamespaces )
{
    LazyInitialize();

    // Convert the source file path to a UTF-8 string readable by the FBX SDK.
#if HELIUM_UNICODE
    KFBX_WIDECHAR_to_UTF8( *rSourceFilePath, pConvertedFilePath );
#else
    char* pConvertedFilePath = NULL;
    KFBX_ANSI_to_UTF8( *rSourceFilePath, pConvertedFilePath );
#endif
    if( !pConvertedFilePath )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "FbxSupport::LoadMesh(): Failed to convert source file path string \"%s\" to a UTF-8 string " )
            TXT( "for use with the FBX SDK.\n" ) ),
            *rSourceFilePath );

        return false;
    }

    // Import the mesh into a new scene.
    KFbxScene* pScene = KFbxScene::Create( m_pSdkManager, "Import Scene" );
    HELIUM_ASSERT( pScene );

    HELIUM_ASSERT( m_pImporter );
    HELIUM_ASSERT( m_pIoSettings );
    if( !m_pImporter->Initialize( pConvertedFilePath, -1, m_pIoSettings ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::LoadMesh(): Initialization of FBX importer for source file \"%s\" failed.\n" ),
            *rSourceFilePath );

        return false;
    }

    m_pIoSettings->SetBoolProp( IMP_FBX_MODEL, true );
    m_pIoSettings->SetBoolProp( IMP_FBX_ANIMATION, false );
    m_pIoSettings->SetBoolProp( IMP_FBX_MATERIAL, false );
    m_pIoSettings->SetBoolProp( IMP_FBX_TEXTURE, false );

    if( !m_pImporter->Import( pScene ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::LoadMesh(): Failed to import source file \"%s\".\n" ),
            *rSourceFilePath );

        pScene->Destroy();

        return false;
    }

    bool bParseSuccess = BuildMeshFromScene(
        pScene,
        rVertices,
        rIndices,
        rSectionVertexCounts,
        rSectionTriangleCounts,
        rBones,
        rVertexBlendData,
        rSkinningPaletteMap,
        bStripNamespaces );

    pScene->Destroy();

    if( !bParseSuccess )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "FbxSupport::LoadMesh(): Failed to extract mesh data from the scene stored in source file " )
            TXT( "\"%s\".\n" ) ),
            *rSourceFilePath );
    }

    return bParseSuccess;
}

/// Load animation data from a supported source file type.
///
/// @param[in]  rSourceFilePath    Path name of the source file from which to load the animation data.
/// @param[in]  oversampling       Amount of oversampling to perform when parsing the animation data.
/// @param[out] rTracks            Information about each track loaded from the animation, including transform
///                                keyframe data.
/// @param[out] rSamplesPerSecond  Number of animation key frame samples per second.
/// @param[in]  bStripNamespaces   True to detect and strip namespaces from the start of each track name.
///
/// @return  True if loading was successful, false if not.
bool FbxSupport::LoadAnimation(
                               const String& rSourceFilePath,
                               uint8_t oversampling,
                               DynArray< AnimTrackData >& rTracks,
                               uint_fast32_t& rSamplesPerSecond,
                               bool bStripNamespaces )
{
    LazyInitialize();

    // Convert the source file path to a UTF-8 string readable by the FBX SDK.
#if HELIUM_UNICODE
    KFBX_WIDECHAR_to_UTF8( *rSourceFilePath, pConvertedFilePath );
#else
    char* pConvertedFilePath = NULL;
    KFBX_ANSI_to_UTF8( *rSourceFilePath, pConvertedFilePath );
#endif
    if( !pConvertedFilePath )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "FbxSupport::LoadAnimation(): Failed to convert source file path string \"%s\" to a UTF-8 " )
            TXT( "string for use with the FBX SDK.\n" ) ),
            *rSourceFilePath );

        return false;
    }

    // Import the animation into a new scene.
    KFbxScene* pScene = KFbxScene::Create( m_pSdkManager, "Import Scene" );
    HELIUM_ASSERT( pScene );

    HELIUM_ASSERT( m_pImporter );
    HELIUM_ASSERT( m_pIoSettings );
    if( !m_pImporter->Initialize( pConvertedFilePath, -1, m_pIoSettings ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::LoadAnimation(): Initialization of FBX importer for source file \"%s\" failed.\n" ),
            *rSourceFilePath );

        return false;
    }

    m_pIoSettings->SetBoolProp( IMP_FBX_MODEL, false );
    m_pIoSettings->SetBoolProp( IMP_FBX_ANIMATION, true );
    m_pIoSettings->SetBoolProp( IMP_FBX_MATERIAL, false );
    m_pIoSettings->SetBoolProp( IMP_FBX_TEXTURE, false );

    if( !m_pImporter->Import( pScene ) )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::LoadAnimation(): Failed to import source file \"%s\".\n" ),
            *rSourceFilePath );

        pScene->Destroy();

        return false;
    }

    bool bParseSuccess = BuildAnimationFromScene(
        pScene,
        oversampling,
        rTracks,
        rSamplesPerSecond,
        bStripNamespaces );

    pScene->Destroy();

    if( !bParseSuccess )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            ( TXT( "FbxSupport::LoadAnimation(): Failed to extract animation data from the scene stored in " )
            TXT( "source file \"%s\".\n" ) ),
            *rSourceFilePath );
    }

    return bParseSuccess;
}

/// Acquire a reference to the static instance of this class, creating it if necessary.
///
/// When done using an FbxSupport instance, it should be released by calling Release() on the instance.
///
/// @return  Reference to the static FbxSupport instance.
///
/// @see Release()
FbxSupport& FbxSupport::StaticAcquire()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new FbxSupport;
        HELIUM_ASSERT( sm_pInstance );
    }
    else
    {
        AtomicIncrementAcquire( sm_pInstance->m_referenceCount );
    }

    return *sm_pInstance;
}

/// Initialize the FBX SDK manager if necessary.
void FbxSupport::LazyInitialize()
{
    // Check if already initialized.
    if( m_pSdkManager )
    {
        return;
    }

    HELIUM_ASSERT( !m_pIoSettings );
    HELIUM_ASSERT( !m_pImporter );

    HELIUM_TRACE( TRACE_INFO, TXT( "Initializing FBX support layer...\n" ) );

    m_pSdkManager = KFbxSdkManager::Create();
    HELIUM_ASSERT( m_pSdkManager );

#if L_ENABLE_FBX_MEMORY_ALLOCATOR
    m_pSdkManager->SetMemoryAllocator( &m_memoryAllocator );
#endif  // L_ENABLE_FBX_MEMORY_ALLOCATOR

    m_pIoSettings = KFbxIOSettings::Create( m_pSdkManager, IOSROOT );
    HELIUM_ASSERT( m_pIoSettings );
    m_pSdkManager->SetIOSettings( m_pIoSettings );

    m_pImporter = KFbxImporter::Create( m_pSdkManager, "FBX Importer" );
    HELIUM_ASSERT( m_pImporter );

    HELIUM_TRACE( TRACE_INFO, TXT( "FBX support layer initialized.\n" ) );
}

/// Build skinning information from the scene for use in runtime rendering.
///
/// @param[in]  pScene                Scene to parse.
/// @param[in]  pMesh                 Mesh for which to build skinning information.
/// @param[in]  pSkeletonRootNode     Root node of the mesh skeleton.
/// @param[in]  rControlPointIndices  Indices of the mesh control point corresponding to each vertex in the
///                                   preprocessed mesh.
/// @param[in]  rSectionVertexCounts  Number of vertices addressed by each mesh section.
/// @param[out] rBones                Array containing information about each mesh bone.
/// @param[out] rVertexBlendData      Blend weights and indices for each vertex.
/// @param[out] rSkinningPaletteMap   Array of maps between bone indices and GPU skinning palette indices for each
///                                   mesh section.
/// @param[in]  bStripNamespaces      True to detect and strip namespaces from the start of each bone name.
void FbxSupport::BuildSkinningInformation(
    KFbxScene* pScene,
    KFbxMesh* pMesh,
    KFbxNode* pSkeletonRootNode,
    const DynArray< int >& rControlPointIndices,
    const DynArray< uint16_t >& rSectionVertexCounts,
    DynArray< BoneData >& rBones,
    DynArray< BlendData >& rVertexBlendData,
    DynArray< uint8_t >& rSkinningPaletteMap,
    bool bStripNamespaces )
{
    HELIUM_ASSERT( pScene );
    HELIUM_ASSERT( pMesh );
    HELIUM_ASSERT( pSkeletonRootNode );
    HELIUM_ASSERT( rBones.IsEmpty() );
    HELIUM_ASSERT( rVertexBlendData.IsEmpty() );
    HELIUM_ASSERT( rSkinningPaletteMap.IsEmpty() );

    if( pMesh->GetDeformerCount( KFbxDeformer::eSKIN ) == 0 )
    {
        return;
    }

    KFbxSkin* pSkin = static_cast< KFbxSkin* >( pMesh->GetDeformer( 0, KFbxDeformer::eSKIN ) );
    HELIUM_ASSERT( pSkin );

    int clusterCount = pSkin->GetClusterCount();
    HELIUM_ASSERT( clusterCount >= 0 );
    if( clusterCount <= 0 )
    {
        return;
    }

    int poseCount = pScene->GetPoseCount();
    for( int poseIndex = 0; poseIndex < poseCount; ++poseIndex )
    {
        KFbxPose* pPose = pScene->GetPose( poseIndex );
        HELIUM_ASSERT( pPose );
        if( !pPose->IsBindPose() )
        {
            continue;
        }

        HELIUM_TRACE( TRACE_DEBUG, TXT( "FbxSupport::BuildSkinningInformation(): Parsing skinning data...\n" ) );

        // Assemble the bone hierarchy for the mesh.
        DynArray< WorkingBoneData > workingBones;
        RecursiveAddMeshSkeletonData(
            pSkeletonRootNode,
            Invalid< uint8_t >(),
            rBones,
            workingBones,
            bStripNamespaces );

        size_t boneCount = rBones.GetSize();
        HELIUM_ASSERT( workingBones.GetSize() == boneCount );

        // Retrieve the reference pose transforms.
        int poseNodeCount = pPose->GetCount();
        for( int poseNodeIndex = 0; poseNodeIndex < poseNodeCount; ++poseNodeIndex )
        {
            KFbxNode* pPoseNode = pPose->GetNode( poseNodeIndex );
            if( !pPoseNode )
            {
                HELIUM_TRACE( TRACE_DEBUG, TXT( "Bind pose node %d is null.\n" ), poseNodeIndex );

                continue;
            }

            size_t boneIndex;
            for( boneIndex = 0; boneIndex < boneCount; ++boneIndex )
            {
                if( workingBones[ boneIndex ].pNode == pPoseNode )
                {
                    break;
                }
            }

            if( boneIndex >= boneCount )
            {
#if HELIUM_ENABLE_TRACE
                String nodeNameString;
                const char* pNodeName = pPoseNode->GetName();
                if( pNodeName )
                {
                    StringConverter< char, tchar_t >::Convert( nodeNameString, pNodeName );
                }

                HELIUM_TRACE(
                    TRACE_DEBUG,
                    TXT( "Bind pose node %d (%s) not found in mesh skeleton.\n" ),
                    poseNodeIndex,
                    *nodeNameString );
#endif

                continue;
            }

            KFbxMatrix boneMatrix = pPose->GetMatrix( poseNodeIndex );
            //FlipTransform( boneMatrix );
            boneMatrix[ 0 ][ 0 ] = -boneMatrix[ 0 ][ 0 ];
            boneMatrix[ 0 ][ 1 ] = -boneMatrix[ 0 ][ 1 ];
            boneMatrix[ 0 ][ 2 ] = -boneMatrix[ 0 ][ 2 ];
            boneMatrix[ 0 ][ 3 ] = -boneMatrix[ 0 ][ 3 ];
            rBones[ boneIndex ].referenceTransform = Simd::Matrix44(
                static_cast< float32_t >( boneMatrix.Get( 0, 0 ) ),
                static_cast< float32_t >( boneMatrix.Get( 0, 1 ) ),
                static_cast< float32_t >( boneMatrix.Get( 0, 2 ) ),
                static_cast< float32_t >( boneMatrix.Get( 0, 3 ) ),
                static_cast< float32_t >( boneMatrix.Get( 1, 0 ) ),
                static_cast< float32_t >( boneMatrix.Get( 1, 1 ) ),
                static_cast< float32_t >( boneMatrix.Get( 1, 2 ) ),
                static_cast< float32_t >( boneMatrix.Get( 1, 3 ) ),
                static_cast< float32_t >( boneMatrix.Get( 2, 0 ) ),
                static_cast< float32_t >( boneMatrix.Get( 2, 1 ) ),
                static_cast< float32_t >( boneMatrix.Get( 2, 2 ) ),
                static_cast< float32_t >( boneMatrix.Get( 2, 3 ) ),
                static_cast< float32_t >( boneMatrix.Get( 3, 0 ) ),
                static_cast< float32_t >( boneMatrix.Get( 3, 1 ) ),
                static_cast< float32_t >( boneMatrix.Get( 3, 2 ) ),
                static_cast< float32_t >( boneMatrix.Get( 3, 3 ) ) );

            workingBones[ boneIndex ].bParentRelative = pPose->IsLocalMatrix( poseNodeIndex );
        }

        // Convert all pose transforms to mesh-local space and compute the inverse mesh-local transform.
        for( size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex )
        {
            BoneData& rBoneData = rBones[ boneIndex ];

            if( workingBones[ boneIndex ].bParentRelative )
            {
                uint8_t parentBoneIndex = rBoneData.parentIndex;
                if( IsValid( parentBoneIndex ) )
                {
                    rBoneData.referenceTransform.MultiplySet(
                        rBones[ parentBoneIndex ].referenceTransform,
                        rBoneData.referenceTransform );
                }
            }

            rBoneData.referenceTransform.GetInverse( rBoneData.inverseWorldTransform );
        }

        // Convert mesh-local space transforms to parent-relative space.  We iterate over the bone list in reverse
        // in order to guarantee we're still working with the parent bone's transform in mesh-local space.
        for( size_t boneIndex = boneCount; ( boneIndex-- ) != 0; )
        {
            BoneData& rBoneData = rBones[ boneIndex ];
            uint8_t parentBoneIndex = rBoneData.parentIndex;
            if( IsValid( parentBoneIndex ) )
            {
                Simd::Matrix44 inverseParent = rBones[ parentBoneIndex ].referenceTransform;
                inverseParent.Invert();
                rBoneData.referenceTransform.MultiplySet( inverseParent, rBoneData.referenceTransform );
            }
        }

        // Parse control point weighting data from each cluster.
        HELIUM_TRACE( TRACE_DEBUG, TXT( "FbxSupport::BuildSkinningInformation(): Parsing control point weights...\n" ) );

        int_fast32_t controlPointCount = pMesh->GetControlPointsCount();
        HELIUM_ASSERT( controlPointCount >= 0 );

        DynArray< BlendData > controlPointBlendData;
        controlPointBlendData.Reserve( controlPointCount );
        controlPointBlendData.Resize( controlPointCount );
        MemoryZero( controlPointBlendData.GetData(), controlPointCount * sizeof( BlendData ) );

        for( int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex )
        {
            const KFbxCluster* pCluster = pSkin->GetCluster( clusterIndex );
            HELIUM_ASSERT( pCluster );

            const KFbxNode* pClusterLink = pCluster->GetLink();
            if( !pClusterLink )
            {
                HELIUM_TRACE( TRACE_DEBUG, TXT( "Cluster %d has no link.\n" ), clusterIndex );

                continue;
            }

            int pointCount = pCluster->GetControlPointIndicesCount();
            if( pointCount == 0 )
            {
#if HELIUM_ENABLE_TRACE
                String linkNameString;
                const char* pLinkName = pClusterLink->GetName();
                if( pLinkName )
                {
                    StringConverter< char, tchar_t >::Convert( linkNameString, pLinkName );
                }

                HELIUM_TRACE(
                    TRACE_DEBUG,
                    TXT( "Cluster link %d (link: %s) influences no control points.\n" ),
                    clusterIndex,
                    *linkNameString );
#endif

                continue;
            }

            size_t boneIndex;
            for( boneIndex = 0; boneIndex < boneCount; ++boneIndex )
            {
                if( workingBones[ boneIndex ].pNode == pClusterLink )
                {
                    break;
                }
            }

            if( boneIndex >= boneCount )
            {
#if HELIUM_ENABLE_TRACE
                String linkNameString;
                const char* pLinkName = pClusterLink->GetName();
                if( pLinkName )
                {
                    StringConverter< char, tchar_t >::Convert( linkNameString, pLinkName );
                }

                HELIUM_TRACE(
                    TRACE_DEBUG,
                    TXT( "Cluster link %d (%s) not found in mesh skeleton.\n" ),
                    clusterIndex,
                    *linkNameString );
#endif

                continue;
            }

            const int* pPointIndices = pCluster->GetControlPointIndices();
            HELIUM_ASSERT( pPointIndices );
            const double* pPointWeights = pCluster->GetControlPointWeights();
            HELIUM_ASSERT( pPointWeights );

            for( int pointIndex = 0; pointIndex < pointCount; ++pointIndex )
            {
                int_fast32_t controlPointIndex = pPointIndices[ pointIndex ];
                HELIUM_ASSERT( controlPointIndex >= 0 );
                HELIUM_ASSERT( controlPointIndex < controlPointCount );

                float32_t weight = static_cast< float32_t >( pPointWeights[ pointIndex ] );

                BlendData& rBlendData = controlPointBlendData[ controlPointIndex ];
                if( weight <= rBlendData.weights[ 3 ] )
                {
                    continue;
                }

                if( weight <= rBlendData.weights[ 2 ] )
                {
                    rBlendData.weights[ 3 ] = weight;
                    rBlendData.indices[ 3 ] = static_cast< uint8_t >( boneIndex );

                    continue;
                }

                rBlendData.weights[ 3 ] = rBlendData.weights[ 2 ];
                rBlendData.indices[ 3 ] = rBlendData.indices[ 2 ];

                if( weight <= rBlendData.weights[ 1 ] )
                {
                    rBlendData.weights[ 2 ] = weight;
                    rBlendData.indices[ 2 ] = static_cast< uint8_t >( boneIndex );

                    continue;
                }

                rBlendData.weights[ 2 ] = rBlendData.weights[ 1 ];
                rBlendData.indices[ 2 ] = rBlendData.indices[ 1 ];

                if( weight <= rBlendData.weights[ 0 ] )
                {
                    rBlendData.weights[ 1 ] = weight;
                    rBlendData.indices[ 1 ] = static_cast< uint8_t >( boneIndex );

                    continue;
                }

                rBlendData.weights[ 1 ] = rBlendData.weights[ 0 ];
                rBlendData.indices[ 1 ] = rBlendData.indices[ 0 ];

                rBlendData.weights[ 0 ] = weight;
                rBlendData.indices[ 0 ] = static_cast< uint8_t >( boneIndex );
            }
        }

        // Store control point weighting data for each vertex.
        size_t vertexCount = rControlPointIndices.GetSize();
        rVertexBlendData.Reserve( vertexCount );
        for( size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex )
        {
            int controlPointIndex = rControlPointIndices[ vertexIndex ];
            HELIUM_ASSERT( controlPointIndex >= 0 );
            HELIUM_ASSERT( controlPointIndex < controlPointCount );

            rVertexBlendData.Push( controlPointBlendData[ controlPointIndex ] );
        }

        // Build the skinning palette maps for each mesh section.
        HELIUM_TRACE( TRACE_DEBUG, TXT( "FbxSupport::BuildSkinningInformation(): Building skinning palette maps...\n" ) );

        size_t meshSectionCount = rSectionVertexCounts.GetSize();
        rSkinningPaletteMap.Reserve( meshSectionCount * boneCount );

        size_t startVertexIndex = 0;
        for( size_t sectionIndex = 0; sectionIndex < meshSectionCount; ++sectionIndex )
        {
            size_t vertexEnd = startVertexIndex + static_cast< size_t >( rSectionVertexCounts[ sectionIndex ] );

            uint8_t paletteSize = 0;
            for( size_t boneIndex = 0; boneIndex < boneCount; ++boneIndex )
            {
                HELIUM_ASSERT( paletteSize < UINT8_MAX );

                uint_fast8_t boneIndexByte = static_cast< uint8_t >( boneIndex );

                uint8_t paletteIndex;
                SetInvalid( paletteIndex );

                for( size_t vertexIndex = startVertexIndex; vertexIndex < vertexEnd; ++vertexIndex )
                {
                    const BlendData& rBlendData = rVertexBlendData[ vertexIndex ];
                    if( rBlendData.indices[ 0 ] == boneIndexByte )
                    {
                        paletteIndex = paletteSize;
                        ++paletteSize;

                        break;
                    }

                    // We can use exact comparisons of whether the weight is zero here to save some time, since the
                    // weight will be exactly zero if a bone was never set in a weighting slot.
                    if( rBlendData.weights[ 1 ] != 0.0f )
                    {
                        if( rBlendData.indices[ 1 ] == boneIndexByte )
                        {
                            paletteIndex = paletteSize;
                            ++paletteSize;

                            break;
                        }

                        if( rBlendData.weights[ 2 ] != 0.0f )
                        {
                            if( rBlendData.indices[ 2 ] == boneIndexByte )
                            {
                                paletteIndex = paletteSize;
                                ++paletteSize;

                                break;
                            }

                            if( rBlendData.weights[ 3 ] != 0.0f )
                            {
                                if( rBlendData.indices[ 3 ] == boneIndexByte )
                                {
                                    paletteIndex = paletteSize;
                                    ++paletteSize;

                                    break;
                                }
                            }
                        }
                    }
                }

                rSkinningPaletteMap.Push( paletteIndex );
            }

            startVertexIndex = vertexEnd;
        }

        // Remap blend indices to match the skinning palette maps for each mesh section.
        HELIUM_TRACE( TRACE_DEBUG, TXT( "FbxSupport::BuildSkinningInformation(): Remapping bone indices...\n" ) );

        startVertexIndex = 0;
        for( size_t sectionIndex = 0; sectionIndex < meshSectionCount; ++sectionIndex )
        {
            size_t vertexEnd = startVertexIndex + static_cast< size_t >( rSectionVertexCounts[ sectionIndex ] );

            const uint8_t* pPaletteMap = &rSkinningPaletteMap[ sectionIndex * boneCount ];

            for( size_t vertexIndex = startVertexIndex; vertexIndex < vertexEnd; ++vertexIndex )
            {
                BlendData& rBlendData = rVertexBlendData[ vertexIndex ];

                uint8_t boneIndex = rBlendData.indices[ 0 ];
                HELIUM_ASSERT( boneIndex < boneCount );
                boneIndex = pPaletteMap[ boneIndex ];
                HELIUM_ASSERT( boneIndex < boneCount );
                rBlendData.indices[ 0 ] = boneIndex;

                // We can use exact comparisons of whether the weight is zero here to save some time, since the
                // weight will be exactly zero if a bone was never set in a weighting slot.
                if( rBlendData.weights[ 1 ] != 0.0f )
                {
                    boneIndex = rBlendData.indices[ 1 ];
                    HELIUM_ASSERT( boneIndex < boneCount );
                    boneIndex = pPaletteMap[ boneIndex ];
                    HELIUM_ASSERT( boneIndex < boneCount );
                    rBlendData.indices[ 1 ] = boneIndex;

                    if( rBlendData.weights[ 2 ] != 0.0f )
                    {
                        boneIndex = rBlendData.indices[ 2 ];
                        HELIUM_ASSERT( boneIndex < boneCount );
                        boneIndex = pPaletteMap[ boneIndex ];
                        HELIUM_ASSERT( boneIndex < boneCount );
                        rBlendData.indices[ 2 ] = boneIndex;

                        if( rBlendData.weights[ 3 ] != 0.0f )
                        {
                            boneIndex = rBlendData.indices[ 3 ];
                            HELIUM_ASSERT( boneIndex < boneCount );
                            boneIndex = pPaletteMap[ boneIndex ];
                            HELIUM_ASSERT( boneIndex < boneCount );
                            rBlendData.indices[ 3 ] = boneIndex;
                        }
                    }
                }
            }

            startVertexIndex = vertexEnd;
        }

        // Normalize blend weights to account for dropped bone influences and other inconsistencies.
        HELIUM_TRACE( TRACE_DEBUG, TXT( "FbxSupport::BuildSkinningInformation(): Normalizing blend weights...\n" ) );

        for( size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex )
        {
            BlendData& rBlendData = rVertexBlendData[ vertexIndex ];

            float32_t totalWeight =
                rBlendData.weights[ 0 ] + rBlendData.weights[ 1 ] + rBlendData.weights[ 2 ] +
                rBlendData.weights[ 3 ];
            if( totalWeight < HELIUM_EPSILON )
            {
                MemoryZero( rBlendData.weights, sizeof( rBlendData.weights ) );
            }
            else
            {
                float32_t weightScale = 1.0f / totalWeight;
                rBlendData.weights[ 0 ] *= weightScale;
                rBlendData.weights[ 1 ] *= weightScale;
                rBlendData.weights[ 2 ] *= weightScale;
                rBlendData.weights[ 3 ] *= weightScale;
            }
        }

        return;
    }
}

/// Recursively add the given bone and its children to the output bone arrays.
///
/// @param[in]  pCurrentBoneNode  Scene node for the current skeleton bone.
/// @param[in]  parentBoneIndex   Index of the parent bone for the current skeleton bone.
/// @param[out] rBones            Array containing information about each mesh bone.
/// @param[out] rWorkingBones     Temporary working data relevant for each bone while building the skinning
///                               information.
/// @param[in]  bStripNamespaces  True to detect and strip namespaces from the start of each bone name.
void FbxSupport::RecursiveAddMeshSkeletonData(
    const KFbxNode* pCurrentBoneNode,
    uint8_t parentBoneIndex,
    DynArray< BoneData >& rBones,
    DynArray< WorkingBoneData >& rWorkingBones,
    bool bStripNamespaces )
{
    HELIUM_ASSERT( pCurrentBoneNode );
    HELIUM_ASSERT( pCurrentBoneNode->GetNodeAttribute() );
    HELIUM_ASSERT( pCurrentBoneNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON );

    const char* pBoneName = pCurrentBoneNode->GetName();
    if( bStripNamespaces )
    {
        pBoneName = StripNamespace( pBoneName );
    }

    String boneNameString;
    if( pBoneName )
    {
        StringConverter< char, tchar_t >::Convert( boneNameString, pBoneName );
    }

    BoneData* pBoneData = rBones.New();
    HELIUM_ASSERT( pBoneData );
    pBoneData->name = Name( boneNameString );
    pBoneData->parentIndex = parentBoneIndex;
    pBoneData->referenceTransform = Simd::Matrix44::IDENTITY;

    WorkingBoneData* pWorkingData = rWorkingBones.New();
    HELIUM_ASSERT( pWorkingData );
    pWorkingData->pNode = pCurrentBoneNode;
    pWorkingData->bParentRelative = true;

    uint8_t currentBoneIndex = static_cast< uint8_t >( rBones.GetSize() - 1 );
    HELIUM_ASSERT( IsValid( currentBoneIndex ) );
    HELIUM_ASSERT( rWorkingBones.GetSize() - 1 == currentBoneIndex );

    int childCount = pCurrentBoneNode->GetChildCount();
    for( int childIndex = 0; childIndex < childCount; ++childIndex )
    {
        const KFbxNode* pChildNode = pCurrentBoneNode->GetChild( childIndex );
        HELIUM_ASSERT( pChildNode );
        RecursiveAddMeshSkeletonData( pChildNode, currentBoneIndex, rBones, rWorkingBones, bStripNamespaces );
    }
}

/// Recursively add the given bone and its children to the output animation track arrays.
///
/// @param[in]  pCurrentBoneNode  Scene node for the current skeleton bone.
/// @param[in]  parentTrackIndex  Index of the parent track for the current animation track.
/// @param[out] rTracks           Array containing information about each animation track.
/// @param[out] rWorkingTracks    Temporary working data for each track while building the animation data.
/// @param[in]  bStripNamespaces  True to detect and strip namespaces from the start of each track name.
void FbxSupport::RecursiveAddAnimationSkeletonData(
    KFbxNode* pCurrentBoneNode,
    uint8_t parentTrackIndex,
    DynArray< AnimTrackData >& rTracks,
    DynArray< WorkingTrackData >& rWorkingTracks,
    bool bStripNamespaces )
{
    HELIUM_ASSERT( pCurrentBoneNode );
    HELIUM_ASSERT( pCurrentBoneNode->GetNodeAttribute() );
    HELIUM_ASSERT( pCurrentBoneNode->GetNodeAttribute()->GetAttributeType() == KFbxNodeAttribute::eSKELETON );

    const char* pTrackName = pCurrentBoneNode->GetName();
    if( bStripNamespaces )
    {
        pTrackName = StripNamespace( pTrackName );
    }

    String trackNameString;
    if( pTrackName )
    {
        StringConverter< char, tchar_t >::Convert( trackNameString, pTrackName );
    }

    AnimTrackData* pTrackData = rTracks.New();
    HELIUM_ASSERT( pTrackData );
    pTrackData->name = Name( trackNameString );

    WorkingTrackData* pWorkingData = rWorkingTracks.New();
    HELIUM_ASSERT( pWorkingData );
    pWorkingData->pNode = pCurrentBoneNode;
    pWorkingData->parentIndex = parentTrackIndex;

    uint8_t currentTrackIndex = static_cast< uint8_t >( rTracks.GetSize() - 1 );
    HELIUM_ASSERT( IsValid( currentTrackIndex ) );
    HELIUM_ASSERT( rWorkingTracks.GetSize() - 1 == currentTrackIndex );

    int childCount = pCurrentBoneNode->GetChildCount();
    for( int childIndex = 0; childIndex < childCount; ++childIndex )
    {
        KFbxNode* pChildNode = pCurrentBoneNode->GetChild( childIndex );
        HELIUM_ASSERT( pChildNode );
        KFbxNodeAttribute* pNodeAttribute = pChildNode->GetNodeAttribute();
        if( pNodeAttribute && pNodeAttribute->GetAttributeType() == KFbxNodeAttribute::eSKELETON )
        {
            RecursiveAddAnimationSkeletonData(
                pChildNode,
                currentTrackIndex,
                rTracks,
                rWorkingTracks,
                bStripNamespaces );
        }
    }
}

/// Parse the specified scene and extract the mesh data from it.
///
/// @param[in]  pScene                  Scene to parse.
/// @param[out] rVertices               Mesh vertices.
/// @param[out] rIndices                Mesh vertex indices (three per triangle).
/// @param[out] rSectionVertexCounts    Number of vertices addressed by each mesh section.
/// @param[out] rSectionTriangleCounts  Number of triangles per mesh section.
/// @param[out] rBones                  Information about each bone in the mesh (if the mesh contains skinning
///                                     data).
/// @param[out] rVertexBlendData        Blend weights and indices for each vertex (if the mesh contains skinning
///                                     data).
/// @param[out] rSkinningPaletteMap     Array of maps between bone indices and GPU skinning palette indices for each
///                                     mesh section.
/// @param[in]  bStripNamespaces        True to detect and strip namespaces from the start of each bone name.
///
/// @return  True if processing of the scene was successful, false if an error occurred.
bool FbxSupport::BuildMeshFromScene(
                                    KFbxScene* pScene,
                                    DynArray< StaticMeshVertex< 1 > >& rVertices,
                                    DynArray< uint16_t >& rIndices,
                                    DynArray< uint16_t >& rSectionVertexCounts,
                                    DynArray< uint32_t >& rSectionTriangleCounts,
                                    DynArray< BoneData >& rBones,
                                    DynArray< BlendData >& rVertexBlendData,
                                    DynArray< uint8_t >& rSkinningPaletteMap,
                                    bool bStripNamespaces )
{
    HELIUM_ASSERT( pScene );

    // Convert the scene to the engine coordinate system (Y-up, Z-forward, X-right).
    KFbxAxisSystem axisSystem( KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::LeftHanded );
    axisSystem.ConvertScene( pScene );

    KFbxNode* pRoot = pScene->GetRootNode();
    if( !pRoot )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "FbxSupport::BuildMeshFromScene(): No root node found in the FBX scene.\n" ) );

        return false;
    }

    int childCount = pRoot->GetChildCount();
    if( childCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::BuildMeshFromScene(): FBX scene root node does not have any children.\n" ) );

        return false;
    }

    // Locate the first mesh and skeleton nodes in the scene.
    KFbxMesh* pMesh = NULL;
    KFbxNode* pSkeletonRootNode = NULL;
    for( int childIndex = 0; childIndex < childCount && ( !pMesh || !pSkeletonRootNode ); ++childIndex )
    {
        KFbxNode* pNode = pRoot->GetChild( childIndex );
        HELIUM_ASSERT( pNode );

        KFbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
        if( !pNodeAttribute )
        {
            continue;
        }

        KFbxNodeAttribute::EAttributeType attributeType = pNodeAttribute->GetAttributeType();
        if( !pMesh && attributeType == KFbxNodeAttribute::eMESH )
        {
            pMesh = static_cast< KFbxMesh* >( pNodeAttribute );
        }
        else if( !pSkeletonRootNode && attributeType == KFbxNodeAttribute::eSKELETON )
        {
            pSkeletonRootNode = pNode;
        }
    }

    if( !pMesh )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::BuildMeshFromScene(): Failed to locate mesh attribute node in the FBX scene.\n" ) );

        return false;
    }

    int layerCount;

    layerCount = pMesh->GetLayerCount( KFbxLayerElement::eNORMAL );
    if( layerCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::BuildMeshFromScene(): No normal layers found in FBX mesh.\n" ) );

        return false;
    }

    if( static_cast< unsigned int >( layerCount ) > 1 )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            ( TXT( "FbxSupport::BuildMeshFromScene(): Mesh has %d normal layers.  Only the first layer will be " )
            TXT( "used.\n" ) ),
            layerCount );
    }

    KFbxLayerElementUV* pUvLayer = NULL;
    int uvSetCount = pMesh->GetLayerCount( KFbxLayerElement::eUV );
    if( uvSetCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_WARNING,
            ( TXT( "FbxSupport::BuildMeshFromScene(): No UV coordinate layers found in FBX mesh.  Mesh will be " )
            TXT( "cached with one UV set with all UV coordinates set to zero.\n" ) ) );
    }
    else
    {
        if( static_cast< unsigned int >( uvSetCount ) > 1 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "FbxSupport::BuildMeshFromScene(): Mesh has %d UV coordinate set layers.  Only the first " )
                TXT( "layer will be used.\n" ) ),
                uvSetCount );
        }

        KFbxLayer* pLayer = pMesh->GetLayer( 0, KFbxLayerElement::eUV );
        HELIUM_ASSERT( pLayer );
        pUvLayer = pLayer->GetUVs();
        if( !pUvLayer )
        {
            HELIUM_TRACE( TRACE_ERROR, TXT( "FbxSupport::BuildMeshFromScene(): Failed to acquire UV layer.\n" ) );
        }
    }

    KFbxLayerElementVertexColor* pVertexColorLayer = NULL;
    int vertexColorLayerCount = pMesh->GetLayerCount( KFbxLayerElement::eVERTEX_COLOR );
    if( vertexColorLayerCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_INFO,
            ( TXT( "FbxSupport::BuildMeshFromScene(): No vertex color layers found.  Mesh will be cached with " )
            TXT( "all vertex colors set to full white.\n" ) ) );
    }
    else
    {
        if( static_cast< unsigned int >( vertexColorLayerCount ) > 1 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "FbxSupport::BuildMeshFromScene(): Mesh has %d vertex color layers.  Only the first layer " )
                TXT( "will be used.\n" ) ),
                vertexColorLayerCount );
        }

        KFbxLayer* pLayer = pMesh->GetLayer( 0, KFbxLayerElement::eVERTEX_COLOR );
        HELIUM_ASSERT( pLayer );
        pVertexColorLayer = pLayer->GetVertexColors();
        HELIUM_ASSERT( pVertexColorLayer );
    }

    KFbxLayerElementMaterial* pMaterialLayer = NULL;
    int materialLayerCount = pMesh->GetLayerCount( KFbxLayerElement::eMATERIAL );
    if( materialLayerCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_INFO,
            ( TXT( "FbxSupport::BuildMeshFromScene(): No material layers found.  Mesh will be cached with " )
            TXT( "support for only one material.\n" ) ) );
    }
    else
    {
        if( static_cast< unsigned int >( materialLayerCount ) > 1 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "FbxSupport::BuildMeshFromScene(): Mesh has %d material layers.  Only the first layer " )
                TXT( "will be used.\n" ) ),
                materialLayerCount );
        }

        KFbxLayer* pLayer = pMesh->GetLayer( 0, KFbxLayerElement::eMATERIAL );
        HELIUM_ASSERT( pLayer );
        pMaterialLayer = pLayer->GetMaterials();
        HELIUM_ASSERT( pMaterialLayer );
    }

    DynArray< DynArray< StaticMeshVertex< 1 > > > sectionVertices;
    DynArray< DynArray< uint16_t > > sectionVertexIndices;
    DynArray< DynArray< int > > sectionControlPointIndices;

    size_t totalVertexCount = 0;
    size_t totalTriangleCount = 0;

    StaticMeshVertex< 1 > vertex;

    int meshVertexIndex = 0;
    Float32 packedFloat;

    int_fast32_t polygonCount = pMesh->GetPolygonCount();
    for( int_fast32_t polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex )
    {
        int_fast32_t polygonVertexCount = pMesh->GetPolygonSize( polygonIndex );
        if( polygonVertexCount < 3 )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                ( TXT( "FbxSupport::BuildMeshFromScene(): Invalid number of polygon vertices (%" ) TPRIdFAST32
                TXT( ") specified for polygon %" ) TPRIdFAST32 TXT( ".\n" ) ),
                polygonVertexCount,
                polygonIndex );

            meshVertexIndex += static_cast< int >( polygonVertexCount );

            continue;
        }

        if( polygonVertexCount > 3 )
        {
            HELIUM_TRACE(
                TRACE_WARNING,
                ( TXT( "FbxSupport::BuildMeshFromScene(): Polygon %" ) TPRIuFAST32 TXT( " has %" ) TPRIuFAST32
                TXT( " vertices, will be split into triangles automatically.\n" ) ),
                polygonIndex,
                polygonVertexCount );
        }

        uint16_t vertexIndex0 = 0;
        uint16_t vertexIndexPrev = 0;

        // Only use the material from the first vertex for the whole polygon.
        int sectionIndex = 0;
        if( pMaterialLayer )
        {
            sectionIndex = GetLayerElementValueIndex(
                pMaterialLayer,
                pMesh->GetPolygonVertex( polygonIndex, 0 ),
                meshVertexIndex,
                polygonIndex );
            HELIUM_ASSERT( sectionIndex != -1 );
            if( sectionIndex == -1 )
            {
                sectionIndex = 0;
            }
        }

        size_t meshSectionCount = sectionVertexIndices.GetSize();
        if( static_cast< size_t >( sectionIndex ) >= meshSectionCount )
        {
            size_t newSectionCount = static_cast< size_t >( sectionIndex ) + 1;
            sectionVertices.Resize( newSectionCount );
            sectionVertexIndices.Resize( newSectionCount );
            sectionControlPointIndices.Resize( newSectionCount );
        }

        DynArray< StaticMeshVertex< 1 > >& rCurrentSectionVertices = sectionVertices[ sectionIndex ];
        DynArray< uint16_t >& rCurrentSectionIndices = sectionVertexIndices[ sectionIndex ];
        DynArray< int >& rCurrentSectionControlPointIndices = sectionControlPointIndices[ sectionIndex ];

        for( int_fast32_t polygonVertexIndex = 0;
            polygonVertexIndex < polygonVertexCount;
            ++polygonVertexIndex, ++meshVertexIndex )
        {
            int controlPointIndex = pMesh->GetPolygonVertex( polygonIndex, polygonVertexIndex );
            HELIUM_ASSERT( controlPointIndex >= 0 );

            KFbxVector4 normal( 0.0f, 0.0f, 1.0f, 0.0f );
            pMesh->GetPolygonVertexNormal( polygonIndex, polygonVertexIndex, normal );
            vertex.normal[ 0 ] = static_cast< uint8_t >( Clamp(
                static_cast< float32_t >( normal[ 0 ] ) * 127.5f + 128.0f,
                0.0f,
                255.0f ) );
            vertex.normal[ 1 ] = static_cast< uint8_t >( Clamp(
                static_cast< float32_t >( normal[ 1 ] ) * 127.5f + 128.0f,
                0.0f,
                255.0f ) );
            vertex.normal[ 2 ] = static_cast< uint8_t >( Clamp(
                static_cast< float32_t >( normal[ 2 ] ) * 127.5f + 128.0f,
                0.0f,
                255.0f ) );
            vertex.normal[ 3 ] = 0;

            vertex.color[ 0 ] = 0xff;
            vertex.color[ 1 ] = 0xff;
            vertex.color[ 2 ] = 0xff;
            vertex.color[ 3 ] = 0xff;
            if( pVertexColorLayer )
            {
                KFbxColor color( 1.0, 1.0, 1.0, 1.0 );
                GetLayerElementValue( pVertexColorLayer, controlPointIndex, meshVertexIndex, polygonIndex, color );

                Color packedColor(
                    static_cast< float32_t >( color.mRed ),
                    static_cast< float32_t >( color.mGreen ),
                    static_cast< float32_t >( color.mBlue ),
                    static_cast< float32_t >( color.mAlpha ) );
                vertex.color[ 0 ] = packedColor.GetR();
                vertex.color[ 1 ] = packedColor.GetG();
                vertex.color[ 2 ] = packedColor.GetB();
                vertex.color[ 3 ] = packedColor.GetA();
            }

            vertex.texCoords[ 0 ][ 0 ].packed = 0;
            vertex.texCoords[ 0 ][ 1 ].packed = 0;
            if( pUvLayer )
            {
                KFbxVector2 uv( 0.0, 0.0 );
                GetLayerElementValue( pUvLayer, controlPointIndex, meshVertexIndex, polygonIndex, uv );

                packedFloat.value = static_cast< float32_t >( uv[ 0 ] );
                vertex.texCoords[ 0 ][ 0 ] = Float32To16( packedFloat );
                // Flip UVs vertically to place the UV origin at the bottom-left instead of the top-left.
                // XXX TMC TODO: Find out if the UV origin can be parsed from the FBX scene/mesh/etc.
                packedFloat.value = 1.0f - static_cast< float32_t >( uv[ 1 ] );
                vertex.texCoords[ 0 ][ 1 ] = Float32To16( packedFloat );
            }

            size_t vertexCount = rCurrentSectionVertices.GetSize();
            size_t vertexIndex;
            for( vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex )
            {
                const StaticMeshVertex< 1 >& rVertex = rCurrentSectionVertices[ vertexIndex ];
                if( rCurrentSectionControlPointIndices[ vertexIndex ] == controlPointIndex &&
                    rVertex.normal[ 0 ] == vertex.normal[ 0 ] &&
                    rVertex.normal[ 1 ] == vertex.normal[ 1 ] &&
                    rVertex.normal[ 2 ] == vertex.normal[ 2 ] &&
                    rVertex.color == vertex.color &&
                    rVertex.texCoords[ 0 ][ 0 ].packed == vertex.texCoords[ 0 ][ 0 ].packed &&
                    rVertex.texCoords[ 0 ][ 1 ].packed == vertex.texCoords[ 0 ][ 1 ].packed )
                {
                    break;
                }
            }

            HELIUM_ASSERT( vertexIndex <= vertexCount );
            HELIUM_ASSERT( vertexIndex <= UINT16_MAX );
            if( vertexIndex >= vertexCount )
            {
                // Note that when getting the position, we need to flip vertices across the x-axis manually since
                // KFbxAxisSystem::ConvertScene() doesn't actually modify the mesh data.
                KFbxVector4 position = pMesh->GetControlPointAt( controlPointIndex );
                vertex.position[ 0 ] = -static_cast< float32_t >( position[ 0 ] );
                vertex.position[ 1 ] = static_cast< float32_t >( position[ 1 ] );
                vertex.position[ 2 ] = static_cast< float32_t >( position[ 2 ] );

                rCurrentSectionControlPointIndices.Push( controlPointIndex );
                rCurrentSectionVertices.Push( vertex );

                ++totalVertexCount;
            }

            uint16_t vertexIndex16 = static_cast< uint16_t >( vertexIndex );

            if( polygonVertexIndex > 1 )
            {
                // Reverse the triangle ordering when building the index list since we flipped the mesh across the
                // x-axis.
                rCurrentSectionIndices.Push( vertexIndex0 );
                rCurrentSectionIndices.Push( vertexIndex16 );
                rCurrentSectionIndices.Push( vertexIndexPrev );

                vertexIndexPrev = vertexIndex16;

                ++totalTriangleCount;
            }
            else if( polygonVertexIndex == 0 )
            {
                vertexIndex0 = vertexIndex16;
            }
            else
            {
                HELIUM_ASSERT( polygonVertexIndex == 1 );
                vertexIndexPrev = vertexIndex16;
            }
        }
    }

    rVertices.Clear();
    rIndices.Clear();
    rSectionVertexCounts.Clear();
    rSectionTriangleCounts.Clear();

    rVertices.Reserve( totalVertexCount );
    rIndices.Reserve( totalTriangleCount * 3 );

    DynArray< int > controlPointIndices;
    controlPointIndices.Reserve( totalVertexCount );

    size_t meshSectionCount = sectionVertexIndices.GetSize();
    rSectionVertexCounts.Reserve( meshSectionCount );
    rSectionTriangleCounts.Reserve( meshSectionCount );

    for( size_t sectionIndex = 0; sectionIndex < meshSectionCount; ++sectionIndex )
    {
        const DynArray< StaticMeshVertex< 1 > >& rCurrentSectionVertices = sectionVertices[ sectionIndex ];
        const DynArray< uint16_t >& rCurrentSectionIndices = sectionVertexIndices[ sectionIndex ];
        const DynArray< int >& rCurrentSectionControlPointIndices = sectionControlPointIndices[ sectionIndex ];

        size_t sectionVertexCount = rCurrentSectionVertices.GetSize();
        HELIUM_ASSERT( rCurrentSectionControlPointIndices.GetSize() == sectionVertexCount );
        rVertices.AddArray( rCurrentSectionVertices.GetData(), sectionVertexCount );
        controlPointIndices.AddArray( rCurrentSectionControlPointIndices.GetData(), sectionVertexCount );
        HELIUM_ASSERT( sectionVertexCount <= UINT16_MAX );
        rSectionVertexCounts.Push( static_cast< uint16_t >( sectionVertexCount ) );

        size_t sectionIndexCount = rCurrentSectionIndices.GetSize();
        rIndices.AddArray( rCurrentSectionIndices.GetData(), sectionIndexCount );
        HELIUM_ASSERT( sectionIndexCount % 3 == 0 );
        HELIUM_ASSERT( sectionIndexCount / 3 <= UINT32_MAX );
        rSectionTriangleCounts.Push( static_cast< uint32_t >( sectionIndexCount / 3 ) );
    }

    sectionVertices.Clear();
    sectionVertexIndices.Clear();
    sectionControlPointIndices.Clear();

    HELIUM_ASSERT( rVertices.GetSize() == totalVertexCount );
    HELIUM_ASSERT( rIndices.GetSize() == totalTriangleCount * 3 );
    HELIUM_ASSERT( rSectionVertexCounts.GetSize() == meshSectionCount );
    HELIUM_ASSERT( rSectionTriangleCounts.GetSize() == meshSectionCount );

    // Process skinned mesh data if the mesh has skinning information.
    rBones.Remove( 0, rBones.GetSize() );
    rVertexBlendData.Remove( 0, rVertexBlendData.GetSize() );
    rSkinningPaletteMap.Remove( 0, rSkinningPaletteMap.GetSize() );

    if( pSkeletonRootNode )
    {
        BuildSkinningInformation(
            pScene,
            pMesh,
            pSkeletonRootNode,
            controlPointIndices,
            rSectionVertexCounts,
            rBones,
            rVertexBlendData,
            rSkinningPaletteMap,
            bStripNamespaces );
    }

    HELIUM_TRACE(
        TRACE_DEBUG,
        TXT( "FbxSupport::BuildMeshFromScene(): Computing tangent vectors...\n" ) );

    size_t vertexCount = rVertices.GetSize();

    DynArray< Simd::Vector3 > intermediateTangents;
    intermediateTangents.Reserve( vertexCount );
    intermediateTangents.Add( Simd::Vector3( 0.0f ), vertexCount );

    DynArray< Simd::Vector3 > intermediateBinormals;
    intermediateBinormals.Reserve( vertexCount );
    intermediateBinormals.Add( Simd::Vector3( 0.0f ), vertexCount );

    Simd::Vector3 tangentFallback( 1.0f, 0.0f, 0.0f );
    Simd::Vector3 binormalFallback( 0.0f, 1.0f, 0.0f );

    size_t indexCount = rIndices.GetSize();
    for( size_t indexIndex = 0; indexIndex < indexCount; indexIndex += 3 )
    {
        size_t vertexIndex0 = rIndices[ indexIndex ];
        size_t vertexIndex1 = rIndices[ indexIndex + 1 ];
        size_t vertexIndex2 = rIndices[ indexIndex + 2 ];

        const StaticMeshVertex< 1 >& rVertex0 = rVertices[ vertexIndex0 ];
        const StaticMeshVertex< 1 >& rVertex1 = rVertices[ vertexIndex1 ];
        const StaticMeshVertex< 1 >& rVertex2 = rVertices[ vertexIndex2 ];

        Simd::Vector2 uv0(
            Float16To32( rVertex0.texCoords[ 0 ][ 0 ] ).value,
            Float16To32( rVertex0.texCoords[ 0 ][ 1 ] ).value );
        Simd::Vector2 uv1(
            Float16To32( rVertex1.texCoords[ 0 ][ 0 ] ).value,
            Float16To32( rVertex1.texCoords[ 0 ][ 1 ] ).value );
        Simd::Vector2 uv2(
            Float16To32( rVertex2.texCoords[ 0 ][ 0 ] ).value,
            Float16To32( rVertex2.texCoords[ 0 ][ 1 ] ).value );

        Simd::Vector2 toUv1 = uv1 - uv0;
        Simd::Vector2 toUv2 = uv2 - uv0;

        Simd::Vector3 tangent, binormal;

        float32_t scale = toUv1.Cross( toUv2 );
        if( Abs( scale ) < HELIUM_EPSILON )
        {
            tangent = tangentFallback;
            binormal = binormalFallback;
        }
        else
        {
            Simd::Vector3 scaleSplat( 1.0f / scale );

            Simd::Vector3 position0( rVertex0.position[ 0 ], rVertex0.position[ 1 ], rVertex0.position[ 2 ] );
            Simd::Vector3 position1( rVertex1.position[ 0 ], rVertex1.position[ 1 ], rVertex1.position[ 2 ] );
            Simd::Vector3 position2( rVertex2.position[ 0 ], rVertex2.position[ 1 ], rVertex2.position[ 2 ] );

            Simd::Vector3 toVertex1 = position1 - position0;
            Simd::Vector3 toVertex2 = position2 - position0;

            Simd::Vector2 tangentDot( toUv2.GetY(), -toUv1.GetY() );
            Simd::Vector2 binormalDot( -toUv2.GetX(), toUv1.GetX() );
            Simd::Vector2 xDot( toVertex1.GetElement( 0 ), toVertex2.GetElement( 0 ) );
            Simd::Vector2 yDot( toVertex1.GetElement( 1 ), toVertex2.GetElement( 1 ) );
            Simd::Vector2 zDot( toVertex1.GetElement( 2 ), toVertex2.GetElement( 2 ) );

            tangent = Simd::Vector3( tangentDot.Dot( xDot ), tangentDot.Dot( yDot ), tangentDot.Dot( zDot ) );
            binormal = Simd::Vector3( binormalDot.Dot( xDot ), binormalDot.Dot( yDot ), binormalDot.Dot( zDot ) );

            tangent *= scaleSplat;
            binormal *= scaleSplat;
        }

        intermediateTangents[ vertexIndex0 ] += tangent;
        intermediateTangents[ vertexIndex1 ] += tangent;
        intermediateTangents[ vertexIndex2 ] += tangent;

        intermediateBinormals[ vertexIndex0 ] += binormal;
        intermediateBinormals[ vertexIndex1 ] += binormal;
        intermediateBinormals[ vertexIndex2 ] += binormal;
    }

    Simd::Vector3 tangentScale( 127.5f );
    Simd::Vector3 tangentBias( 128.0f );

    for( size_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex )
    {
        StaticMeshVertex< 1 >& rVertex = rVertices[ vertexIndex ];

        Simd::Vector3 normal(
            static_cast< float32_t >( rVertex.normal[ 0 ] ) * ( 2.0f / 255.0f ) - 1.0f,
            static_cast< float32_t >( rVertex.normal[ 1 ] ) * ( 2.0f / 255.0f ) - 1.0f,
            static_cast< float32_t >( rVertex.normal[ 2 ] ) * ( 2.0f / 255.0f ) - 1.0f );

        const Simd::Vector3& rTangentRaw = intermediateTangents[ vertexIndex ];

        Simd::Vector3 tangent = rTangentRaw - normal * Simd::Vector3( normal.Dot( rTangentRaw ) );
        tangent.Normalize();

        Simd::Vector3 binormalInferred;
        binormalInferred.CrossSet( normal, tangent );

        tangent *= tangentScale;
        tangent += tangentBias;
        rVertex.tangent[ 0 ] = static_cast< uint8_t >( Clamp( tangent.GetElement( 0 ), 0.0f, 255.0f ) );
        rVertex.tangent[ 1 ] = static_cast< uint8_t >( Clamp( tangent.GetElement( 1 ), 0.0f, 255.0f ) );
        rVertex.tangent[ 2 ] = static_cast< uint8_t >( Clamp( tangent.GetElement( 2 ), 0.0f, 255.0f ) );

        rVertex.tangent[ 3 ] = ( binormalInferred.Dot( intermediateBinormals[ vertexIndex ] ) < 0.0f ? 0 : 255 );
    }

    return true;
}

/// Parse the specified scene and extract the animation data from it.
///
/// @param[in]  pScene             Scene to parse.
/// @param[in]  oversampling       Amount of oversampling to perform when parsing the animation data.
/// @param[out] rTracks            Information about each track loaded from the animation, including transform
///                                keyframe data.
/// @param[out] rSamplesPerSecond  Number of animation key frame samples per second.
/// @param[in]  bStripNamespaces   True to detect and strip namespaces from the start of each track name.
///
/// @return  True if processing of the scene was successful, false if an error occurred.
bool FbxSupport::BuildAnimationFromScene(
    KFbxScene* pScene,
    uint_fast32_t oversampling,
    DynArray< AnimTrackData >& rTracks,
    uint_fast32_t& rSamplesPerSecond,
    bool bStripNamespaces )
{
    HELIUM_ASSERT( pScene );

    // Convert the scene to the engine coordinate system (Y-up, Z-forward, X-right).
    KFbxAxisSystem axisSystem( KFbxAxisSystem::YAxis, KFbxAxisSystem::ParityOdd, KFbxAxisSystem::LeftHanded );
    axisSystem.ConvertScene( pScene );

    KFbxNode* pRoot = pScene->GetRootNode();
    if( !pRoot )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "FbxSupport::BuildAnimationFromScene(): No root node found in the FBX scene.\n" ) );

        return false;
    }

    int childCount = pRoot->GetChildCount();
    if( childCount == 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::BuildAnimationFromScene(): FBX scene root node does not have any children.\n" ) );

        return false;
    }

    // Locate the first skeleton root node in the scene.
    KFbxNode* pSkeletonRootNode = NULL;
    for( int childIndex = 0; childIndex < childCount; ++childIndex )
    {
        KFbxNode* pNode = pRoot->GetChild( childIndex );
        HELIUM_ASSERT( pNode );

        KFbxNodeAttribute* pNodeAttribute = pNode->GetNodeAttribute();
        if( !pNodeAttribute )
        {
            continue;
        }

        KFbxNodeAttribute::EAttributeType attributeType = pNodeAttribute->GetAttributeType();
        if( attributeType == KFbxNodeAttribute::eSKELETON )
        {
            pSkeletonRootNode = pNode;

            break;
        }
    }

    if( !pSkeletonRootNode )
    {
        HELIUM_TRACE( TRACE_ERROR, TXT( "FbxSupport::BuildAnimationFromScene(): Failed to locate skeleton root node.\n" ) );

        return false;
    }

    // Locate the first animation stack in the scene.
    if( pScene->GetMemberCount( FBX_TYPE( KFbxAnimStack ) ) == 0 )
    {
        HELIUM_TRACE(
            TRACE_ERROR,
            TXT( "FbxSupport::BuildAnimationFromScene(): Scene does not contain any animation stacks.\n" ) );

        return false;
    }

    KFbxAnimStack* pAnimStack = pScene->GetMember( FBX_TYPE( KFbxAnimStack ), 0 );
    HELIUM_ASSERT( pAnimStack );

    // Initialize the track array with the names of each bone node in the skeleton.
    rTracks.Remove( 0, rTracks.GetSize() );
    DynArray< WorkingTrackData > workingTracks;

    RecursiveAddAnimationSkeletonData(
        pSkeletonRootNode,
        Invalid< uint8_t >(),
        rTracks,
        workingTracks,
        bStripNamespaces );

    size_t trackCount = rTracks.GetSize();
    HELIUM_ASSERT( workingTracks.GetSize() == trackCount );

    // Make sure the animation is active.
    KFbxAnimEvaluator* pEvaluator = pScene->GetEvaluator();
    HELIUM_ASSERT( pEvaluator );
    pEvaluator->SetContext( pAnimStack );

    // Get the start and end times from the take information for the animation stack.  If there is no take
    // information, use the default timeline settings for the scene.
    const KFbxGlobalSettings& rGlobalSettings = pScene->GetGlobalSettings();

    KTimeSpan timeSpan = pAnimStack->GetLocalTimeSpan();
    KTime startTime = timeSpan.GetStart();
    KTime duration = timeSpan.GetDuration();

    oversampling = Max< uint_fast32_t >( oversampling, 1 );
    float64_t residualPerSample = 100.0 / static_cast< float64_t >( oversampling );

    KTime::ETimeMode timeMode = rGlobalSettings.GetTimeMode();
    float64_t frameRate = KTime::GetFrameRate( timeMode );
    rSamplesPerSecond = static_cast< uint_fast32_t >( frameRate ) * oversampling;

    kLongLong sampleCountActual = ( duration.GetFrame( true ) * oversampling ) + 1;
    HELIUM_ASSERT( sampleCountActual <= UINT32_MAX );
    uint_fast32_t sampleCount = static_cast< uint_fast32_t >( sampleCountActual );

    // Sample each track pose for the duration of the animation in global space (computing the proper transforms
    // from certain node inheritance types can be tricky, so it's easier to get the global transforms first and
    // convert to local-space later).
    for( uint_fast32_t sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex )
    {
        uint_fast32_t frameIndex = sampleIndex / oversampling;

        uint_fast32_t sampleRemainder = sampleIndex % oversampling;
        int residual = static_cast< int >( static_cast< float64_t >( sampleRemainder ) * residualPerSample );

        KTime timeOffset;
        timeOffset.SetTime( 0, 0, 0, static_cast< int >( frameIndex ), 0, residual, timeMode );
        KTime currentTime = startTime + timeOffset;

        for( size_t trackIndex = 0; trackIndex < trackCount; ++trackIndex )
        {
            WorkingTrackData& rWorkingData = workingTracks[ trackIndex ];

            KFbxNode* pNode = rWorkingData.pNode;
            HELIUM_ASSERT( pNode );

            rWorkingData.modelSpaceTransform = pEvaluator->GetNodeGlobalTransform( pNode, currentTime );
            FlipTransform( rWorkingData.modelSpaceTransform );
        }

        for( size_t trackIndex = 0; trackIndex < trackCount; ++trackIndex )
        {
            WorkingTrackData& rWorkingData = workingTracks[ trackIndex ];

            KFbxXMatrix parentRelativeTransform = rWorkingData.modelSpaceTransform;

            uint8_t parentTrackIndex = rWorkingData.parentIndex;
            if( IsValid( parentTrackIndex ) )
            {
                KFbxXMatrix inverseParentTransform =
                    workingTracks[ parentTrackIndex ].modelSpaceTransform.Inverse();
                parentRelativeTransform = inverseParentTransform * parentRelativeTransform;
            }

            KFbxVector4 scale = parentRelativeTransform.GetS();

            double inverseScale;
            inverseScale = 1.0 / scale[ 0 ];
            parentRelativeTransform[ 0 ][ 0 ] *= inverseScale;
            parentRelativeTransform[ 0 ][ 1 ] *= inverseScale;
            parentRelativeTransform[ 0 ][ 2 ] *= inverseScale;
            inverseScale = 1.0 / scale[ 1 ];
            parentRelativeTransform[ 1 ][ 0 ] *= inverseScale;
            parentRelativeTransform[ 1 ][ 1 ] *= inverseScale;
            parentRelativeTransform[ 1 ][ 2 ] *= inverseScale;
            inverseScale = 1.0 / scale[ 2 ];
            parentRelativeTransform[ 2 ][ 0 ] *= inverseScale;
            parentRelativeTransform[ 2 ][ 1 ] *= inverseScale;
            parentRelativeTransform[ 2 ][ 2 ] *= inverseScale;

            KFbxQuaternion rotation = parentRelativeTransform.GetQ();
            KFbxVector4 translation = parentRelativeTransform.GetT();
            if( IsInvalid( parentTrackIndex ) )
            {
                // XXX TMC: Seems we still need to flip the translation of the root bone across the x-axis.
                translation[ 0 ] = -translation[ 0 ];
            }

            Key* pKey = rTracks[ trackIndex ].keys.New();
            HELIUM_ASSERT( pKey );

            pKey->translation.SetElement( 0, static_cast< float32_t >( translation.mData[ 0 ] ) );
            pKey->translation.SetElement( 1, static_cast< float32_t >( translation.mData[ 1 ] ) );
            pKey->translation.SetElement( 2, static_cast< float32_t >( translation.mData[ 2 ] ) );

            pKey->rotation.SetElement( 0, static_cast< float32_t >( rotation.mData[ 0 ] ) );
            pKey->rotation.SetElement( 1, static_cast< float32_t >( rotation.mData[ 1 ] ) );
            pKey->rotation.SetElement( 2, static_cast< float32_t >( rotation.mData[ 2 ] ) );
            pKey->rotation.SetElement( 3, static_cast< float32_t >( rotation.mData[ 3 ] ) );

            pKey->scale.SetElement( 0, static_cast< float32_t >( scale.mData[ 0 ] ) );
            pKey->scale.SetElement( 1, static_cast< float32_t >( scale.mData[ 1 ] ) );
            pKey->scale.SetElement( 2, static_cast< float32_t >( scale.mData[ 2 ] ) );
        }
    }

    return true;
}

/// Get the starting location in the given string immediately after any namespace information for stripping
/// namespaces during preprocessing.
///
/// Namespaces are separated by a colon when loaded from an FBX file.  This will search for the last instance of a
/// colon in the given string and return the first character immediately following the colon if found.  If there is
/// no colon in the given string, or there are no more characters following the colon, the entire string is
/// returned.
///
/// @param[in] pString  String to parse.
///
/// @return  Pointer to the starting character in the given string after any namespace information.
const char* FbxSupport::StripNamespace( const char* pString )
{
    if( pString )
    {
        const char* pLastColon = NULL;

        const char* pTestCharacter = pString;
        for( ; ; )
        {
            char character = *pTestCharacter;
            if( character == '\0' )
            {
                break;
            }

            if( character == ':' )
            {
                pLastColon = pTestCharacter;
            }

            ++pTestCharacter;
        }

        if( pLastColon && pLastColon + 1 != pTestCharacter )
        {
            pString = pLastColon + 1;
        }
    }

    return pString;
}

#endif  // L_EDITOR

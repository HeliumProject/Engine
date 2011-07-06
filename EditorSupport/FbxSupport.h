//----------------------------------------------------------------------------------------------------------------------
// FbxSupport.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_EDITOR_SUPPORT_FBX_SUPPORT_H
#define HELIUM_EDITOR_SUPPORT_FBX_SUPPORT_H

#include "EditorSupport/EditorSupport.h"

#if HELIUM_EDITOR

#include "Math/SimdMatrix44.h"
#include "Math/SimdQuat.h"
#include "GraphicsTypes/VertexTypes.h"

#if HELIUM_CC_MSC
#pragma warning( push )
#pragma warning( disable : 4100 ) // 'identifier' : unreferenced formal parameter
#pragma warning( disable : 4512 ) // 'class' : assignment operator could not be generated
#endif

#include <fbxsdk.h>

#if HELIUM_CC_MSC
#pragma warning( pop )
#endif

// Non-zero to enable wrapping of the FBX memory allocator with our own allocator (currently disabled due to
// long-standing bugs with the FBX SDK attempting to free allocations returned by CRT functions, i.e. strdup()).
#define HELIUM_ENABLE_FBX_MEMORY_ALLOCATOR 0

namespace Helium
{
#if HELIUM_ENABLE_FBX_MEMORY_ALLOCATOR
    /// Custom FBX memory allocator.
    class FbxMemoryAllocator : public KFbxMemoryAllocator
    {
    public:
        /// @name Construction/Destruction
        //@{
        FbxMemoryAllocator();
        //@}

    protected:
        /// @name Memory Routines
        //@{
        static void* Malloc( size_t size );
        static void* Calloc( size_t count, size_t size );
        static void* Realloc( void* pMemory, size_t size );
        static void Free( void* pMemory );
        static size_t Msize( void* pMemory );
        static void* MallocDebug( size_t size, int, const char*, int );
        static void* CallocDebug( size_t count, size_t size, int, const char*, int );
        static void* ReallocDebug( void* pMemory, size_t size, int, const char*, int );
        static void FreeDebug( void* pMemory, int );
        static size_t MsizeDebug( void* pMemory, int );
        //@}
    };
#endif  // HELIUM_ENABLE_FBX_MEMORY_ALLOCATOR

    /// FBX SDK support.
    class FbxSupport : NonCopyable
    {
    public:
        /// Information about a given bone in the skeleton when building the mesh skeleton data.
        struct BoneData
        {
            /// Bone reference pose transform (relative to parent bone).
            Simd::Matrix44 referenceTransform;
            /// Inverse mesh-space transform for the bone in its reference pose.
            Simd::Matrix44 inverseWorldTransform;
            /// Bone name.
            Name name;
            /// Parent bone index (invalid index if this is a root bone).
            uint8_t parentIndex;
        };

        /// Per-vertex skinning information.
        struct BlendData
        {
            /// Blend weights.
            float32_t weights[ 4 ];
            /// Blend indices.
            uint8_t indices[ 4 ];
        };

        /// Animation key frame data.
        struct Key
        {
            /// Translation data.
            Simd::Vector3 translation;
            /// Rotation data.
            Simd::Quat rotation;
            /// Scaling data.
            Simd::Vector3 scale;
        };

        /// Animation track information.
        struct AnimTrackData
        {
            /// Track name.
            Name name;
            /// Bone transform key frames.
            DynArray< Key > keys;
        };

        /// @name Access Reference Counting
        //@{
        void Release();
        //@}

        /// @name Resource Loading
        //@{
        bool LoadMesh(
            const String& rSourceFilePath, DynArray< StaticMeshVertex< 1 > >& rVertices, DynArray< uint16_t >& rIndices,
            DynArray< uint16_t >& rSectionVertexCounts, DynArray< uint32_t >& rSectionTriangleCounts,
            DynArray< BoneData >& rBones, DynArray< BlendData >& rVertexBlendData,
            DynArray< uint8_t >& rSkinningPaletteMap, bool bStripNamespaces = true );
        bool LoadAnimation(
            const String& rSourceFilePath, uint8_t oversampling, DynArray< AnimTrackData >& rTracks,
            uint_fast32_t& rSamplesPerSecond, bool bStripNamespaces = true );
        //@}

        /// @name Static Access
        //@{
        static FbxSupport& StaticAcquire();
        //@}

    private:
        /// Information about a given bone in the skeleton relevant only while building the skinning data during mesh
        /// loading.
        struct WorkingBoneData
        {
            /// Scene node for the skeleton bone.
            const KFbxNode* pNode;
            /// True if the node transform is in space relative to its parent bone, false if it is in global space and
            /// needs to be converted.
            bool bParentRelative;
        };

        /// Information about a given animation track relevant only during animation loading.
        struct WorkingTrackData
        {
            /// Model-space bone transform data for a single key frame.
            KFbxXMatrix modelSpaceTransform;
            /// Scene node for the skeleton bone.
            KFbxNode* pNode;
            /// Parent bone index (invalid index if this is a root bone).
            uint8_t parentIndex;
        };

        /// FBX SDK manager instance.
        KFbxSdkManager* m_pSdkManager;
        /// IO settings instance.
        KFbxIOSettings* m_pIoSettings;
        /// Import handler.
        KFbxImporter* m_pImporter;

#if HELIUM_ENABLE_FBX_MEMORY_ALLOCATOR
        /// Memory allocation handler.
        FbxMemoryAllocator m_memoryAllocator;
#endif  // HELIUM_ENABLE_FBX_MEMORY_ALLOCATOR

        /// Reference count.
        volatile int32_t m_referenceCount;

        /// Singleton instance.
        static FbxSupport* sm_pInstance;

        /// @name Construction/Destruction
        //@{
        FbxSupport();
        ~FbxSupport();
        //@}

        /// @name Private Utility Functions
        //@{
        void LazyInitialize();

        void BuildSkinningInformation(
            KFbxScene* pScene, KFbxMesh* pMesh, KFbxNode* pSkeletonRootNode,
            const DynArray< int >& rControlPointIndices, const DynArray< uint16_t >& rSectionVertexCounts,
            DynArray< BoneData >& rBones, DynArray< BlendData >& rVertexBlendData,
            DynArray< uint8_t >& rSkinningPaletteMap, bool bStripNamespaces );

        void RecursiveAddMeshSkeletonData(
            const KFbxNode* pCurrentBoneNode, uint8_t parentBoneIndex, DynArray< BoneData >& rBones,
            DynArray< WorkingBoneData >& rWorkingBones, bool bStripNamespaces );

        void RecursiveAddAnimationSkeletonData(
            KFbxNode* pCurrentBoneNode, uint8_t parentTrackIndex, DynArray< AnimTrackData >& rTracks,
            DynArray< WorkingTrackData >& rWorkingTracks, bool bStripNamespaces );

        bool BuildMeshFromScene(
            KFbxScene* pScene, DynArray< StaticMeshVertex< 1 > >& rVertices, DynArray< uint16_t >& rIndices,
            DynArray< uint16_t >& rSectionVertexCounts, DynArray< uint32_t >& rSectionTriangleCounts,
            DynArray< BoneData >& rBones, DynArray< BlendData >& rVertexBlendData,
            DynArray< uint8_t >& rSkinningPaletteMap, bool bStripNamespaces );
        bool BuildAnimationFromScene(
            KFbxScene* pScene, uint_fast32_t oversampling, DynArray< AnimTrackData >& rTracks,
            uint_fast32_t& rSamplesPerSecond, bool bStripNamespaces );
        //@}

        /// @name Static Private Utility Functions
        //@{
        static const char* StripNamespace( const char* pString );
        //@}
    };
}

#endif  // HELIUM_EDITOR

#endif  // HELIUM_EDITOR_SUPPORT_FBX_SUPPORT_H

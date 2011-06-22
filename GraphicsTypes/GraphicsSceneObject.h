//----------------------------------------------------------------------------------------------------------------------
// GraphicsSceneObject.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_TYPES_GRAPHICS_SCENE_OBJECT_H
#define LUNAR_GRAPHICS_TYPES_GRAPHICS_SCENE_OBJECT_H

#include "GraphicsTypes/GraphicsTypes.h"

#include "Platform/Math/Simd/AaBox.h"
#include "Platform/Math/Simd/Matrix44.h"
#include "Platform/Math/Simd/Sphere.h"
#include "Foundation/Memory/ReferenceCounting.h"
#include "Rendering/RendererTypes.h"
#include "Rendering/RRenderResource.h"

namespace Helium
{
    class GraphicsScene;

    HELIUM_DECLARE_PTR( Material );

    L_DECLARE_RPTR( RVertexBuffer );
    L_DECLARE_RPTR( RIndexBuffer );
    L_DECLARE_RPTR( RVertexDescription );

    /// Information related to a single object attached to the graphics scene.
    HELIUM_SIMD_ALIGN_PRE class LUNAR_GRAPHICS_TYPES_API GraphicsSceneObject
    {
    public:
        /// Update callback function type.
        typedef void ( UPDATE_FUNC )( void* pData, GraphicsScene* pScene, GraphicsSceneObject* pSceneObject );

        /// Graphics scene object update identifiers.
        enum EUpdate
        {
            UPDATE_FIRST   =  0,
            UPDATE_INVALID = -1,

            /// Full update required.
            UPDATE_FULL,
            /// Transform-only update required.
            UPDATE_TRANSFORM_ONLY,

            UPDATE_MAX,
            UPDATE_LAST = UPDATE_MAX - 1
        };

        /// Data specific to sub-meshes.
        class LUNAR_GRAPHICS_TYPES_API SubMeshData
        {
        public:
            /// @name Construction/Destruction
            //@{
            explicit SubMeshData( size_t sceneObjectId );
            //@}

            /// @name Data Access
            //@{
            void SetMaterial( Material* pMaterial );
            void SetSkinningPaletteMap( const uint8_t* pMap );
            void SetPrimitiveType( ERendererPrimitiveType type );
            void SetPrimitiveCount( uint32_t count );
            void SetStartVertex( uint32_t startVertex );
            void SetVertexRange( uint32_t count );
            void SetStartIndex( uint32_t startIndex );

            inline size_t GetSceneObjectId() const;

            inline const MaterialPtr& GetMaterial() const;
            inline const uint8_t* GetSkinningPaletteMap() const;
            inline ERendererPrimitiveType GetPrimitiveType() const;
            inline uint32_t GetPrimitiveCount() const;
            inline uint32_t GetStartVertex() const;
            inline uint32_t GetVertexRange() const;
            inline uint32_t GetStartIndex() const;
            //@}

        private:
            /// ID of the parent graphics scene object controlling placement and providing the vertex data.
            size_t m_sceneObjectId;

            /// Render material.
            MaterialPtr m_spMaterial;
            /// Mapping of bone indices to skinning palette indices (null if no skinning should be performed).
            const uint8_t* m_pSkinningPaletteMap;
            /// Primitive type.
            ERendererPrimitiveType m_primitiveType;
            /// Number of primitives to render.
            uint32_t m_primitiveCount;
            /// Offset of the first vertex to use within the vertex buffer.
            uint32_t m_startVertex;
            /// Total number of vertices potentially addressed, starting from the initial vertex.
            uint32_t m_vertexRange;
            /// Offset of the first index to use within the index buffer.
            uint32_t m_startIndex;
        };

        /// @name Construction/Destruction
        //@{
        GraphicsSceneObject();
        //@}

        /// @name Data Access
        //@{
        void SetTransform( const Simd::Matrix44& rTransform );
        void SetWorldBounds( const Simd::AaBox& rBox );
        void SetVertexData( RVertexBuffer* pVertexBuffer, RVertexDescription* pVertexDescription, uint32_t vertexStride );
        void SetIndexBuffer( RIndexBuffer* pIndexBuffer );

#if L_USE_GRANNY_ANIMATION
        void SetBoneData( const void* pBoneData, uint8_t boneCount );
#else
        void SetBoneData( const Simd::Matrix44* pInverseReferencePose, uint8_t boneCount );
#endif
        void SetBonePalette( const Simd::Matrix44* pTransforms );

        inline const Simd::Matrix44& GetTransform() const;
        inline const Simd::AaBox& GetWorldBox() const;
        inline const Simd::Sphere& GetWorldSphere() const;
        inline RVertexBuffer* GetVertexBuffer() const;
        inline RVertexDescription* GetVertexDescription() const;
        inline uint32_t GetVertexStride() const;
        inline RIndexBuffer* GetIndexBuffer() const;

#if L_USE_GRANNY_ANIMATION
        inline const void* GetBoneData() const;
#else
        inline const Simd::Matrix44* GetInverseReferencePose() const;
#endif
        inline uint8_t GetBoneCount() const;
        inline const Simd::Matrix44* GetBonePalette() const;
        //@}

        /// @name Updating
        //@{
        void SetUpdateCallback( UPDATE_FUNC* pCallback, void* pData );

        void SetNeedsUpdate( EUpdate updateMode = UPDATE_FULL );
        inline bool GetNeedsUpdate() const;
        inline EUpdate GetUpdateMode() const;

        inline void ConditionalUpdate( GraphicsScene* pScene );
        //@}

    private:
        /// Scene transform.
        Simd::Matrix44 m_transform;
        /// World-space axis-aligned bounding box.
        Simd::AaBox m_worldBox;
        /// World-space bounding sphere.
        Simd::Sphere m_worldSphere;

        /// Vertex buffer.
        RVertexBufferPtr m_spVertexBuffer;
        /// Vertex description.
        RVertexDescriptionPtr m_spVertexDescription;
        /// Index buffer.
        RIndexBufferPtr m_spIndexBuffer;

#if L_USE_GRANNY_ANIMATION
        /// Mesh bone data.
        const void* m_pBoneData;
#else
        /// Inverse transform for each bone's reference pose.
        const Simd::Matrix44* m_pInverseReferencePose;
#endif
        /// Bone palette.
        const Simd::Matrix44* m_pBonePalette;

        /// Update callback.
        UPDATE_FUNC* m_pUpdateCallback;
        /// Update callback data.
        void* m_pUpdateCallbackData;

        /// Vertex stride, in bytes.
        uint32_t m_vertexStride;

        /// Number of bones in the bone palette.
        uint8_t m_boneCount;

        /// Update mode.
        uint8_t m_updateMode;
    } HELIUM_SIMD_ALIGN_POST;
}

#include "GraphicsTypes/GraphicsSceneObject.inl"

#endif  // LUNAR_GRAPHICS_TYPES_GRAPHICS_SCENE_OBJECT_H

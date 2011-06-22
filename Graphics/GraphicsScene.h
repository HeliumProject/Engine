//----------------------------------------------------------------------------------------------------------------------
// GraphicsScene.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_GRAPHICS_GRAPHICS_SCENE_H
#define LUNAR_GRAPHICS_GRAPHICS_SCENE_H

#include "Graphics/Graphics.h"
#include "Engine/GameObject.h"

#include "Foundation/Container/BitArray.h"
#include "Rendering/RRenderResource.h"
#include "GraphicsTypes/GraphicsSceneObject.h"
#include "GraphicsTypes/GraphicsSceneView.h"

#if !HELIUM_RELEASE && !HELIUM_PROFILE
#include "Foundation/Container/ObjectPool.h"
#include "Graphics/BufferedDrawer.h"
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

namespace Helium
{
    L_DECLARE_RPTR( RConstantBuffer );

    /// Manager for a graphics scene.
    class LUNAR_GRAPHICS_API GraphicsScene : public GameObject
    {
        L_DECLARE_OBJECT( GraphicsScene, GameObject );

    public:
        /// @name Construction/Destruction
        //@{
        GraphicsScene();
        virtual ~GraphicsScene();
        //@}

        /// @name Updating
        //@{
        virtual void Update();
        //@}

        /// @name Scene View Management
        //@{
        uint32_t AllocateSceneView();
        void ReleaseSceneView( uint32_t id );
        inline GraphicsSceneView* GetSceneView( uint32_t id );

        void SetActiveSceneView( uint32_t id );
        //@}

        /// @name Scene GameObject Allocation
        //@{
        size_t AllocateSceneObject();
        void ReleaseSceneObject( size_t id );
        inline GraphicsSceneObject* GetSceneObject( size_t id );
        //@}

        /// @name Scene GameObject Sub-mesh Allocation
        //@{
        size_t AllocateSceneObjectSubMeshData( size_t sceneObjectId );
        void ReleaseSceneObjectSubMeshData( size_t id );
        inline GraphicsSceneObject::SubMeshData* GetSceneObjectSubMeshData( size_t id );
        //@}

        /// @name Lighting
        //@{
        void SetAmbientLight(
            const Color& rTopColor, float32_t topBrightness, const Color& rBottomColor, float32_t bottomBrightness );
        inline const Color& GetAmbientLightTopColor() const;
        inline float32_t GetAmbientLightTopBrightness() const;
        inline const Color& GetAmbientLightBottomColor() const;
        inline float32_t GetAmbientLightBottomBrightness() const;

        void SetDirectionalLight( const Simd::Vector3& rDirection, const Color& rColor, float32_t brightness );
        inline const Simd::Vector3& GetDirectionalLightDirection() const;
        inline const Color& GetDirectionalLightColor() const;
        inline float32_t GetDirectionalLightBrightness() const;
        //@}

#if !HELIUM_RELEASE && !HELIUM_PROFILE
        /// @name Buffered Drawing Support
        //@{
        inline BufferedDrawer& GetSceneBufferedDrawer();
        BufferedDrawer* GetSceneViewBufferedDrawer( uint32_t id );
        //@}
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

        /// @name Static Reserved Names
        //@{
        static Name GetDefaultSamplerStateName();
        static Name GetShadowSamplerStateName();
        static Name GetShadowMapTextureName();
        //@}

    private:
        /// Front-to-back sub-mesh sort comparison function
        class LUNAR_GRAPHICS_API SubMeshFrontToBackCompare
        {
        public:
            /// @name Construction/Destruction
            //@{
            SubMeshFrontToBackCompare();
            SubMeshFrontToBackCompare(
                const Simd::Vector3& rCameraDirection, const SparseArray< GraphicsSceneObject >& rSceneObjects,
                const SparseArray< GraphicsSceneObject::SubMeshData >& rSubMeshes );
            //@}

            /// @name Overloaded Operators
            //@{
            bool operator()( size_t subMeshIndex0, size_t subMeshIndex1 ) const;
            //@}

        private:
            /// Camera direction.
            Simd::Vector3 m_cameraDirection;
            /// Scene object list.
            const SparseArray< GraphicsSceneObject >* m_pSceneObjects;
            /// Scene object sub-mesh list.
            const SparseArray< GraphicsSceneObject::SubMeshData >* m_pSubMeshes;
        };

        /// Material-based sub-mesh sort comparison function
        class LUNAR_GRAPHICS_API SubMeshMaterialCompare
        {
        public:
            /// @name Construction/Destruction
            //@{
            SubMeshMaterialCompare();
            explicit SubMeshMaterialCompare( const SparseArray< GraphicsSceneObject::SubMeshData >& rSubMeshes );
            //@}

            /// @name Overloaded Operators
            //@{
            bool operator()( size_t subMeshIndex0, size_t subMeshIndex1 ) const;
            //@}

        private:
            /// Scene object sub-mesh list.
            const SparseArray< GraphicsSceneObject::SubMeshData >* m_pSubMeshes;
        };

        /// Scene view list.
        SparseArray< GraphicsSceneView > m_sceneViews;
        /// Scene object list.
        SparseArray< GraphicsSceneObject > m_sceneObjects;
        /// Scene object sub-data list.
        SparseArray< GraphicsSceneObject::SubMeshData > m_sceneObjectSubMeshes;

#if !HELIUM_RELEASE && !HELIUM_PROFILE
        /// Buffered drawing support for the entire scene (presented in all views).
        BufferedDrawer m_sceneBufferedDrawer;
        /// Pool of buffered drawing objects for various scene views.
        ObjectPool< BufferedDrawer > m_viewBufferedDrawerPool;
        /// Buffered drawing objects for each scene view.
        DynArray< BufferedDrawer* > m_viewBufferedDrawers;
#endif  // !HELIUM_RELEASE && !HELIUM_PROFILE

        /// Visible scene objects for the current view.
        BitArray<> m_visibleSceneObjects;
        /// Scene object sub-data index list (for sorting during rendering).
        DynArray< size_t > m_sceneObjectSubMeshIndices;

        /// Ambient light top color.
        Color m_ambientLightTopColor;
        /// Ambient light top brightness.
        float32_t m_ambientLightTopBrightness;
        /// Ambient light bottom color.
        Color m_ambientLightBottomColor;
        /// Ambient light bottom brightness.
        float32_t m_ambientLightBottomBrightness;

        /// Directional light direction.
        Simd::Vector3 m_directionalLightDirection;
        /// Directional light color.
        Color m_directionalLightColor;
        /// Directional light brightness.
        float32_t m_directionalLightBrightness;

        /// ID of the currently active scene view.
        uint32_t m_activeViewId;

        /// Pre-computed shadow depth pass inverse view/projection matrices.
        DynArray< Simd::Matrix44 > m_shadowViewInverseViewProjectionMatrices;

        /// Per-view global vertex constant buffers.
        DynArray< RConstantBufferPtr > m_viewVertexGlobalDataBuffers[ 2 ];
        /// Per-view base-pass vertex constant buffers.
        DynArray< RConstantBufferPtr > m_viewVertexBasePassDataBuffers[ 2 ];
        /// Per-view screen-space vertex constant buffers.
        DynArray< RConstantBufferPtr > m_viewVertexScreenDataBuffers[ 2 ];

        /// Per-view base-pass pixel constant buffers.
        DynArray< RConstantBufferPtr > m_viewPixelBasePassDataBuffers[ 2 ];

        /// Per-view vertex constant buffers for shadow depth rendering.
        DynArray< RConstantBufferPtr > m_shadowViewVertexDataBuffers[ 2 ];

        /// Pool of per-instance vertex constant buffers for non-skinned meshes.
        DynArray< RConstantBufferPtr > m_staticInstanceVertexGlobalDataBufferPool[ 2 ];
        /// Pool of per-instance vertex constant buffers for skinned meshes.
        DynArray< RConstantBufferPtr > m_skinnedInstanceVertexGlobalDataBufferPool[ 2 ];

        /// Scene object global vertex constant buffers.
        DynArray< RConstantBuffer* > m_objectVertexGlobalDataBuffers;
        /// Mapped scene object global vertex constant buffer addresses.
        DynArray< float32_t* > m_mappedObjectVertexGlobalDataBuffers;

        /// Sub-mesh global vertex constant buffers.
        DynArray< RConstantBuffer* > m_subMeshVertexGlobalDataBuffers;
        /// Mapped sub-mesh global veretex constant buffer addresses.
        DynArray< float32_t* > m_mappedSubMeshVertexGlobalDataBuffers;

        /// Current dynamic constant buffer set index.
        size_t m_constantBufferSetIndex;

        /// @name Rendering
        //@{
        void UpdateShadowInverseViewProjectionMatrixSimple( size_t viewIndex );
        void UpdateShadowInverseViewProjectionMatrixLspsm( size_t viewIndex );

        void SwapDynamicConstantBuffers();

        void DrawSceneView( uint_fast32_t viewIndex );

        void DrawShadowDepthPass( uint_fast32_t viewIndex );
        void DrawDepthPrePass( uint_fast32_t viewIndex );
        void DrawBasePass( uint_fast32_t viewIndex );
        //@}

        /// @name Private Static Utility Functions
        //@{
        static Name GetNoneOptionName();

        static Name GetSkinningSysSelectName();
        static Name GetSkinningSmoothOptionName();
        static Name GetSkinningRigidOptionName();
        //@}
    };
}

#include "Graphics/GraphicsScene.inl"

#endif  // LUNAR_GRAPHICS_GRAPHICS_SCENE_H

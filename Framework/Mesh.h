//----------------------------------------------------------------------------------------------------------------------
// Mesh.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_MESH_H
#define HELIUM_FRAMEWORK_MESH_H

#include "Framework/Framework.h"
#include "Engine/Resource.h"

#include "Foundation/Math/SimdAaBox.h"
#include "GraphicsTypes/GraphicsTypes.h"
#include "Graphics/Material.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannyMeshInterface.h"
#endif

namespace Helium
{
    class Simd::Matrix44;
}

namespace Helium
{
    HELIUM_DECLARE_RPTR( RVertexBuffer );
    HELIUM_DECLARE_RPTR( RIndexBuffer );

    HELIUM_DECLARE_PTR( Material );

    /// Mesh resource type.
    class HELIUM_FRAMEWORK_API Mesh : public Resource
    {
        HELIUM_DECLARE_OBJECT( Mesh, Resource );

    public:

        struct HELIUM_FRAMEWORK_API PersistentResourceData : public Object
        {
            REFLECT_DECLARE_OBJECT(Mesh::PersistentResourceData, Reflect::Object);

            PersistentResourceData();
            static void PopulateComposite( Reflect::Composite& comp );
            
            /// Number of vertices used by each mesh section.
            DynArray< uint16_t > m_sectionVertexCounts;
            /// Number of triangles in each mesh section.
            DynArray< uint32_t > m_sectionTriangleCounts;
            /// Skinning palette map (split by mesh section).
            DynArray< uint8_t > m_skinningPaletteMap;
        
            /// Vertex count.
            uint32_t m_vertexCount;
            /// Triangle count.
            uint32_t m_triangleCount;
        
            /// Mesh bounds.
            Simd::AaBox m_bounds;
        
#if !HELIUM_USE_GRANNY_ANIMATION
            /// Bone count (if the mesh is a skinned mesh).  Note we place this variable separate from the other skinned
            /// mesh data in order to reduce overhead from padding member variables.
            uint8_t m_boneCount;
        
            /// Bone names (if the mesh is a skinned mesh).
            DynArray<Name> m_pBoneNames;
            /// Parent bone indices (if the mesh is a skinned mesh).
            DynArray<uint8_t> m_pParentBoneIndices;
            /// Reference pose bone transforms (if the mesh is a skinned mesh).
            DynArray<Simd::Matrix44> m_pReferencePose;
#endif

        };
        
        /// Persistent mesh resource data.
        PersistentResourceData m_persistentResourceData;

        /// @name Construction/Destruction
        //@{
        Mesh();
        virtual ~Mesh();
        //@}

        /// @name GameObject Interface
        //@{
        virtual void PreDestroy();
        //@}

        /// @name Serialization
        //@{
        //virtual void Serialize( Serializer& s );

        virtual bool NeedsPrecacheResourceData() const;
        virtual bool BeginPrecacheResourceData();
        virtual bool TryFinishPrecacheResourceData();
        //@}

        /// @name Resource Serialization
        //@{
        //virtual void SerializePersistentResourceData( Serializer& s );
        virtual bool LoadPersistentResourceObject(Reflect::ObjectPtr &_object);
        //@}

        /// @name Resource Caching Support
        //@{
        virtual Name GetCacheName() const;
        //@}

        /// @name Data Access
        //@{
        inline size_t GetSectionCount() const;
        inline uint32_t GetSectionVertexCount( size_t sectionIndex ) const;
        inline uint32_t GetSectionTriangleCount( size_t sectionIndex ) const;
        const uint8_t* GetSectionSkinningPaletteMap( size_t sectionIndex ) const;

        inline bool IsSkinned() const;
#if HELIUM_USE_GRANNY_ANIMATION
        inline const Granny::MeshData& GetGrannyData() const;
#else
        inline uint8_t GetBoneCount() const;
        inline const Name* GetBoneNames() const;
        inline const uint8_t* GetParentBoneIndices() const;
        inline const Simd::Matrix44* GetReferencePose() const;
#endif

        inline size_t GetMaterialCount() const;
        inline Material* GetMaterial( size_t index ) const;

        inline uint32_t GetVertexCount() const;
        inline uint32_t GetTriangleCount() const;

        inline const Simd::AaBox& GetBounds() const;

        inline RVertexBuffer* GetVertexBuffer() const;
        inline RIndexBuffer* GetIndexBuffer() const;
        //@}

    private:
        
#if HELIUM_USE_GRANNY_ANIMATION
        /// Granny-specific mesh data.
        Granny::MeshData m_grannyData;
#endif

        /// Default material set.
        DynArray< MaterialPtr > m_materials;
        
        /// Vertex buffer.
        RVertexBufferPtr m_spVertexBuffer;
        /// Index buffer.
        RIndexBufferPtr m_spIndexBuffer;

        /// Asynchronous load ID for the vertex buffer data.
        size_t m_vertexBufferLoadId;
        /// Asynchronous load ID for the index buffer data.
        size_t m_indexBufferLoadId;

    };
}

#include "Framework/Mesh.inl"

#endif  // HELIUM_FRAMEWORK_MESH_H

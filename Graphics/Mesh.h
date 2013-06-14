#pragma once

#include "Framework/Framework.h"
#include "Engine/Resource.h"
#include "MathSimd/Matrix44.h"

#include "MathSimd/AaBox.h"
#include "GraphicsTypes/GraphicsTypes.h"
#include "Graphics/Material.h"

#if HELIUM_USE_GRANNY_ANIMATION
#include "GrannyMeshInterface.h"
#endif

namespace Helium
{
    namespace Simd
    {
        struct Matrix44;
    }
}

namespace Helium
{
    HELIUM_DECLARE_RPTR( RVertexBuffer );
    HELIUM_DECLARE_RPTR( RIndexBuffer );

    class Material;
    typedef Helium::StrongPtr< Material > MaterialPtr;
    typedef Helium::StrongPtr< const Material > ConstMaterialPtr;

    /// Mesh resource type.
    class HELIUM_GRAPHICS_API Mesh : public Resource
    {
        HELIUM_DECLARE_ASSET( Mesh, Resource );
        static void PopulateStructure( Reflect::Structure& comp );

    public:

        struct HELIUM_GRAPHICS_API PersistentResourceData : public Object
        {
            REFLECT_DECLARE_OBJECT(Mesh::PersistentResourceData, Reflect::Object);

            PersistentResourceData();
            static void PopulateStructure( Reflect::Structure& comp );
            
            /// Number of vertices used by each mesh section.
            DynamicArray< uint16_t > m_sectionVertexCounts;
            /// Number of triangles in each mesh section.
            DynamicArray< uint32_t > m_sectionTriangleCounts;
            /// Skinning palette map (split by mesh section).
            DynamicArray< uint8_t > m_skinningPaletteMap;
        
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
            DynamicArray<Name> m_pBoneNames;
            /// Parent bone indices (if the mesh is a skinned mesh).
            DynamicArray<uint8_t> m_pParentBoneIndices;
            /// Reference pose bone transforms (if the mesh is a skinned mesh).
            DynamicArray<Simd::Matrix44> m_pReferencePose;
#endif

        };
        
        /// Persistent mesh resource data.
        PersistentResourceData m_persistentResourceData;

        /// @name Construction/Destruction
        //@{
        Mesh();
        virtual ~Mesh();
        //@}

        /// @name Asset Interface
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
        DynamicArray< MaterialPtr > m_materials;
        
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

#include "Graphics/Mesh.inl"

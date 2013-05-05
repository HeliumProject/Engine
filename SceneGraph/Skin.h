#pragma once

#include "Math/Matrix4.h"

#include "SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Mesh;
        class Scene;
        class Transform;
        typedef std::vector< Transform* > V_TransformDumbPtr;

        // Influence stores the object ID indices and weights of transforms that influence or deform a vertex
        class HELIUM_SCENE_GRAPH_API Influence : public Reflect::Object
        {
        public:
            // The object ID indices of the transforms that influence this vertex
            std::vector< uint32_t > m_Objects;

            // The weights of the transforms that influence this vertex (CORRESPONDS TO objects)
            std::vector< float32_t > m_Weights;

            REFLECT_DECLARE_OBJECT(Influence, Reflect::Object);

            static void PopulateStructure( Reflect::Structure& comp );
        };

        typedef Helium::StrongPtr<Influence> InfluencePtr;
        typedef std::vector<InfluencePtr> V_Influence;

        class Skin : public SceneNode
        {
        public:
            REFLECT_DECLARE_OBJECT( Skin, SceneNode );
            static void PopulateStructure( Reflect::Structure& comp );
            static void InitializeType();
            static void CleanupType();

            Skin();
            ~Skin();

            virtual void Initialize() HELIUM_OVERRIDE;
            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

        private:
            void BlendMatrix(const Transform* transform, const Influence* influence, Matrix4& matrix);

        protected:
            // Reflected
            Helium::TUID        m_MeshID;               // The mesh we are skinning
            V_Influence         m_Influences;           // The influence data for each vertex of the mesh
            Helium::std::vector<TUID>      m_InfluenceObjectIDs;   // The influence objects required to deform this mesh, indexed by each influence item above
            std::vector< uint32_t >  m_InfluenceIndices;     // The influence to use for each vertex (CORRESPONDS TO m_Positions Indices in the referenced mesh)

            // Non-reflected
            Mesh*               m_Mesh;
            V_TransformDumbPtr  m_InfluenceObjects;
            V_Matrix4     m_DeformMatrices;
            V_Matrix4     m_SkinMatrices;
        };
    }
}
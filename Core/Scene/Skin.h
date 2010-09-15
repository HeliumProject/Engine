#pragma once

#include "Core/Scene/SceneNode.h"

namespace Helium
{
    // Forwards
    namespace Content
    {
        class Skin;
        class Influence;
    }

    namespace Core
    {
        class Scene;
        class Transform;
        class Mesh;

        typedef std::vector< Core::Transform* > V_TransformDumbPtr;

        class Skin : public Core::SceneNode
        {
            //
            // Member variables
            //

        protected:
            Core::Mesh* m_Mesh;
            V_TransformDumbPtr m_InfluenceObjects;

            Math::V_Matrix4 m_DeformMatrices;
            Math::V_Matrix4 m_SkinMatrices;

            //
            // Runtime Type Info
            //

            REFLECT_DECLARE_ABSTRACT( Skin, Core::SceneNode );
            static void InitializeType();
            static void CleanupType();

            // 
            // Member functions
            // 
        public:
            Skin( Core::Scene* scene, Content::Skin* skin );
            virtual ~Skin();
            virtual void Initialize() HELIUM_OVERRIDE;
            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

        private:
            void BlendMatrix(const Core::Transform* transform, const Content::Influence* influence, Math::Matrix4& matrix);
        };
    }
}
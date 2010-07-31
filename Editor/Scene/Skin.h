#pragma once

#include "SceneNode.h"

namespace Helium
{
    // Forwards
    namespace Content
    {
        class Skin;
        class Influence;
    }

    namespace Editor
    {
        class Scene;
        class Transform;
        class Mesh;

        typedef std::vector< Editor::Transform* > V_TransformDumbPtr;

        class Skin : public Editor::SceneNode
        {
            //
            // Member variables
            //

        protected:
            Editor::Mesh* m_Mesh;
            V_TransformDumbPtr m_InfluenceObjects;

            Math::V_Matrix4 m_DeformMatrices;
            Math::V_Matrix4 m_SkinMatrices;

            //
            // Runtime Type Info
            //

            EDITOR_DECLARE_TYPE( Skin, Editor::SceneNode );
            static void InitializeType();
            static void CleanupType();

            // 
            // Member functions
            // 
        public:
            Skin( Editor::Scene* scene, Content::Skin* skin );
            virtual ~Skin();
            virtual void Initialize() HELIUM_OVERRIDE;
            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

        private:
            void BlendMatrix(const Editor::Transform* transform, const Content::Influence* influence, Math::Matrix4& matrix);
        };
    }
}
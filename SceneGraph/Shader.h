#pragma once

#include <d3d9.h>

#include "Math/FpuColor4.h"

#include "SceneGraph/API.h"
#include "SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Scene;

        class HELIUM_SCENE_GRAPH_API Shader : public SceneNode
        {
        public:
            REFLECT_DECLARE_OBJECT( Shader, SceneNode );
            static void PopulateComposite( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Shader();
            ~Shader();

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            bool GetAlpha() const;
            IDirect3DTexture9* GetBaseTexture() const;

        public:
            // Reflected
            bool                m_WrapU;
            bool                m_WrapV;
            float               m_RepeatU;
            float               m_RepeatV;
            Color4              m_BaseColor;
            FilePath                m_AssetPath;

        protected:
            // Non-reflected
            bool                m_Alpha;
            uint32_t            m_BaseTextureSize;
            IDirect3DTexture9*  m_BaseTexture;
        };
    }
}
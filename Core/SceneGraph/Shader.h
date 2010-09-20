#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Core/API.h"
#include "Core/SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Scene;

        class Shader : public SceneNode
        {
        public:
            REFLECT_DECLARE_ABSTRACT( Shader, SceneNode );
            static void EnumerateClass( Reflect::Compositor<Shader>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Shader();
            ~Shader();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

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
            Math::Vector4       m_BaseColor;
            Helium::Path        m_AssetPath;

        protected:
            // Non-reflected
            bool                m_Alpha;
            u32                 m_BaseTextureSize;
            IDirect3DTexture9*  m_BaseTexture;
        };
    }
}
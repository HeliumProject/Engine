#pragma once

// Include our Windows header data first to make sure everything is set up properly before the Direct3D includes pull it
// in.
#include "Platform/Windows/Windows.h"

#include <d3d9.h>
#include <d3dx9.h>

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/SceneNode.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Scene;

        class Shader : public SceneNode
        {
        public:
            REFLECT_DECLARE_CLASS( Shader, SceneNode );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            Shader();
            ~Shader();

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
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
            Vector4       m_BaseColor;
            Helium::Path        m_AssetPath;

        protected:
            // Non-reflected
            bool                m_Alpha;
            uint32_t                 m_BaseTextureSize;
            IDirect3DTexture9*  m_BaseTexture;
        };
    }
}
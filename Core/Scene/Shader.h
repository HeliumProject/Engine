#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Core/API.h"
#include "Core/Scene/SceneNode.h"

namespace Helium
{
    // Forwards
    namespace Content
    {
        class Shader;
    }

    namespace Core
    {
        // Forwards
        class Scene;

        class Shader : public Core::SceneNode
        {
            // 
            // Member variables
            // 
        private:
            bool m_Alpha;
            u32 m_BaseTextureSize;
            IDirect3DTexture9* m_BaseTexture;

            //
            // Runtime Type Info
            //

            REFLECT_DECLARE_ABSTRACT( Shader, Core::SceneNode );
            static void InitializeType();
            static void CleanupType();

        public:
            Shader( Core::Scene* s, Content::Shader* shader );

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;

            bool GetAlpha() const;
            IDirect3DTexture9* GetBaseTexture() const;

        private:
            bool LoadTexture();
        };
    }
}
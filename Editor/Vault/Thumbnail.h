#pragma once

#include <vector>

#include "Foundation/Memory/SmartPtr.h"

#include "Core/Render/DeviceManager.h"

namespace Helium
{
    namespace Editor
    {
        class Thumbnail : public Helium::RefCountBase< Thumbnail >
        {
        public:
            Thumbnail( Render::DeviceManager* d3dManager );
            Thumbnail( Render::DeviceManager* d3dManager, IDirect3DTexture9* texture );
            virtual ~Thumbnail();

            bool FromIcon( HICON icon );
            bool IsFromIcon() const;

            inline IDirect3DTexture9* GetTexture() const
            {
                return m_Texture;
            }

        private:
            Render::DeviceManager* m_DeviceManager;
            IDirect3DTexture9* m_Texture;
            bool m_IsFromIcon;
        };
        typedef Helium::SmartPtr< Thumbnail > ThumbnailPtr;
        typedef std::vector< ThumbnailPtr > V_ThumbnailPtr;
    }
}
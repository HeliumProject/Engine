#pragma once

#include <vector>

#include "Foundation/SmartPtr.h"

#include "SceneGraph/DeviceManager.h"

namespace Helium
{
    namespace Editor
    {
        class Thumbnail : public Helium::RefCountBase< Thumbnail >
        {
        public:
            Thumbnail( DeviceManager* d3dManager );
#ifdef VIEWPORT_REFACTOR
            Thumbnail( DeviceManager* d3dManager, IDirect3DTexture9* texture );
#endif
            virtual ~Thumbnail();

#ifdef VIEWPORT_REFACTOR
            inline IDirect3DTexture9* GetTexture() const
            {
                return m_Texture;
            }

            bool FromIcon( HICON icon );
#endif
            bool IsFromIcon() const
            {
                return m_IsFromIcon;
            }

        private:
            DeviceManager* m_DeviceManager;

#ifdef VIEWPORT_REFACTOR
            IDirect3DTexture9* m_Texture;
#endif
            bool m_IsFromIcon;
        };
        typedef Helium::SmartPtr< Thumbnail > ThumbnailPtr;
        typedef std::vector< ThumbnailPtr > V_ThumbnailPtr;
    }
}
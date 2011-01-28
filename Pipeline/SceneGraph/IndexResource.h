#pragma once

#include "Resource.h" 

namespace Lunar
{
    L_DECLARE_RPTR( RVertexBuffer );
}

namespace Helium
{
    namespace SceneGraph
    {

        class IndexResource : public Resource
        {
        public:
            static Profile::MemoryPoolHandle s_MemoryPool;
            IndexElementType m_ElementType;
            Lunar::RIndexBufferPtr m_Buffer; 

            IndexResource( ResourceTracker* tracker ); 

            IndexElementType GetElementType() const
            {
                return m_ElementType;
            }

            void SetElementType( IndexElementType type )
            {
                m_ElementType = type;
                m_IsDirty = true;
            }

            IDirect3DIndexBuffer9* GetBuffer() const
            {
                return m_Buffer; 
            }

            virtual uint32_t GetElementSize() const HELIUM_OVERRIDE;

            virtual uint8_t* Lock() HELIUM_OVERRIDE; 
            virtual void Unlock() HELIUM_OVERRIDE; 
            virtual bool SetState() const HELIUM_OVERRIDE; 

        protected:
            virtual bool Allocate() HELIUM_OVERRIDE; 
            virtual void Release() HELIUM_OVERRIDE; 
        };

        typedef Helium::StrongPtr<IndexResource> IndexResourcePtr;

    }
}
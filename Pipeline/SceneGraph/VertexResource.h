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

        class VertexResource : public Resource
        {
        public:
            static Profile::MemoryPoolHandle s_MemoryPool;
            VertexElementType m_ElementType;
            Lunar::RVertexBufferPtr m_Buffer;

            VertexResource( ResourceTracker* tracker ); 

            VertexElementType GetElementType() const
            {
                return m_ElementType;
            }

            void SetElementType( VertexElementType type )
            {
                m_ElementType = type;
                m_IsDirty = true;
            }

            Lunar::RVertexBuffer* GetBuffer() const
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

            uint8_t* m_LockedVerts; 

        };

        typedef Helium::StrongPtr<VertexResource> VertexResourcePtr;

    }
}
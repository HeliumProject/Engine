#pragma once

#include "Resource.h" 

namespace Helium
{
    namespace Core
    {

        class VertexResource : public Resource
        {
        public:
            static Profile::MemoryPoolHandle s_MemoryPool;
            IDirect3DVertexBuffer9*          m_Buffer; 

            VertexResource( ResourceTracker* tracker ); 

            IDirect3DVertexBuffer9* GetBuffer() const
            {
                return m_Buffer;
            }

            virtual u8* Lock() HELIUM_OVERRIDE; 
            virtual void Unlock() HELIUM_OVERRIDE; 
            virtual bool SetState() const HELIUM_OVERRIDE; 

        protected:
            virtual bool Allocate() HELIUM_OVERRIDE;
            virtual void Release() HELIUM_OVERRIDE;

            u8* m_LockedVerts; 

        };

        typedef Helium::SmartPtr<VertexResource> VertexResourcePtr;

    }
}
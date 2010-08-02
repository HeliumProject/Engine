#pragma once

#include <hash_map>

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Editor/API.h"
#include "Render.h"

// this enables stream source and fvf transition printing
//#define EDITOR_DEBUG_RESOURCES

namespace Helium
{
    namespace Editor
    { 
        //
        // Resource Tracker
        //

        class Resource;
        typedef std::set<Resource*> S_Resource;

        class ResourceTracker
        {
        private:
            // the device those resources are using
            IDirect3DDevice9* m_Device;

            // all the resources we are tracking
            S_Resource m_Resources;

            // state caching
            IDirect3DIndexBuffer9*  m_Indices;
            IDirect3DVertexBuffer9* m_Vertices;
            DWORD                   m_VertexFormat;

        public:
            // create a resource manager on a device
            ResourceTracker(IDirect3DDevice9* device);

            // delete the manager and free all allocated resources
            ~ResourceTracker();

            // device access
            IDirect3DDevice9* GetDevice()
            {
                return m_Device;
            }

            // index stream state
            IDirect3DIndexBuffer9* GetIndices()
            {
                return m_Indices;
            }
            void SetIndices(IDirect3DIndexBuffer9* indices)
            {
                m_Indices = indices;
            }

            // vertex stream state
            IDirect3DVertexBuffer9* GetVertices()
            {
                return m_Vertices;
            }
            void SetVertices(IDirect3DVertexBuffer9* vertices)
            {
                m_Vertices = vertices;
            }

            // vertex format state
            DWORD GetVertexFormat()
            {
                return m_VertexFormat;
            }
            void SetVertexFormat(DWORD format)
            {
                m_VertexFormat = format;
            }

            // reset state
            void ResetState();

            // registers a resource, adds it to the m_ResourceTracker list
            void Register(Resource* resource);

            // deregisters a resource from the m_AllocatedResources list, eg on delete or dispose
            void Release(Resource* resource);

            // free all default resources
            void DeviceLost();

            // reallocate all default resources
            void DeviceReset();
        };

        //
        // Enumeration of compatible info types
        //

        namespace ResourceTypes
        {
            enum ResourceType
            {
                Index,
                Vertex,
            };
        }

        typedef ResourceTypes::ResourceType ResourceType;

        //
        // Populate arguments, for calling back into objects
        //

        struct PopulateArgs
        {
            PopulateArgs(ResourceType t, u32 o, u8* b)
                : m_Type (t)
                , m_Buffer (b)
                , m_Offset (o)
            {

            }

            ResourceType m_Type;
            u32 m_Offset;
            u8* m_Buffer;
        };
        typedef Helium::Signature<void, PopulateArgs*> PopulateSignature;

        //
        // Resource object
        //

        class ResourceTracker;

        class Resource : public Object
        {
        protected:
            // type
            ResourceType m_Type;

            // tracker
            ResourceTracker* m_Tracker;

            // device
            IDirect3DDevice9* m_Device;

            // populator
            PopulateSignature::Delegate m_Populator;

        private:
            // state
            bool m_IsDirty;
            bool m_IsCreated;

            // flags
            bool m_IsManaged;
            bool m_IsDynamic;

            // data
            u32 m_BaseIndex;
            u32 m_ElementCount;
            ElementType m_ElementType;

        public:
            Resource(ResourceType type, ResourceTracker* tracker)
                : m_Type ( type )
                , m_Tracker ( tracker )
                , m_Device ( tracker->GetDevice() )
                , m_IsDirty ( true )
                , m_IsCreated ( false )
                , m_IsManaged ( true )
                , m_IsDynamic ( false )
                , m_BaseIndex ( 0 )
                , m_ElementCount ( 0 )
                , m_ElementType ( ElementTypes::Unknown )
            {
                m_Tracker->Register( this );
            }

            virtual ~Resource()
            {
                m_Tracker->Release( this );
            }

            ResourceType GetResourceType()
            {
                return m_Type;
            }

            ResourceTracker* GetResourceTracker()
            {
                return m_Tracker;
            }

            bool IsManaged()
            {
                return m_IsManaged;
            }
            void SetManaged(bool managed)
            {
                m_IsManaged = managed;
                m_IsDirty = true;
            }

            bool IsDynamic()
            {
                return m_IsDynamic;
            }
            void SetDynamic(bool dynamic)
            {
                if (!m_IsManaged)
                {
                    m_IsDynamic = dynamic;
                    m_IsDirty = true;
                }
                else
                {
                    HELIUM_BREAK();
                }
            }

            u32 GetBaseIndex() const
            {
                return m_BaseIndex;
            }

            u32 GetElementCount() const
            {
                return m_ElementCount;
            }
            void SetElementCount(u32 size)
            {
                m_ElementCount = size;
                m_IsDirty = true;
            }

            ElementType GetElementType() const
            {
                return m_ElementType;
            }
            void SetElementType(ElementType type)
            {
                m_ElementType = type;
                m_IsDirty = true;
            }

            void SetPopulator( const PopulateSignature::Delegate& populator )
            {
                m_Populator = populator;
                m_IsDirty = true;
            }

            // create the actual resource data
            virtual void Create();

            // delete the actual resource data
            virtual void Delete();

            // reallocate and repopulate the data from the populator
            virtual void Update();

            // populate the allocated resource data from the populator delegate
            virtual void Populate();

            // set the runtime stream and fvf state for drawing using this resource
            virtual bool SetState() const = 0;

        protected:
            // lock resource memory
            virtual u8* Lock() = 0;

            // unlock resource memory
            virtual void Unlock() = 0;

            // do runtime allocation of resource
            virtual bool Allocate() = 0;

            // release runtime allocation
            virtual void Release() = 0;
        };

    }
}
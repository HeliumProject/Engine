#pragma once

#include <set>
#include <hash_map>

#include "Platform/Types.h"
#include "Platform/Assert.h"
#include "Foundation/Memory/SmartPtr.h"

#include "Pipeline/API.h"
#include "Render.h"

// this enables stream source and fvf transition printing
//#define SCENE_DEBUG_RESOURCES

namespace Helium
{
    namespace SceneGraph
    { 
        //
        // Resource Tracker
        //

        class Resource;
        typedef std::set<Resource*> S_Resource;

        class ResourceTracker
        {
        private:
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
            PopulateArgs(ResourceType t, uint32_t o, uint8_t* b)
                : m_Type (t)
                , m_Buffer (b)
                , m_Offset (o)
            {

            }

            ResourceType m_Type;
            uint32_t m_Offset;
            uint8_t* m_Buffer;
        };
        typedef Helium::Signature< PopulateArgs*> PopulateSignature;

        //
        // Resource object
        //

        class ResourceTracker;

        class Resource : public Reflect::Object
        {
        protected:
            // type
            ResourceType m_Type;

            // tracker
            ResourceTracker* m_Tracker;

            // populator
            PopulateSignature::Delegate m_Populator;

        private:
            // state
            bool m_IsDirty;
            bool m_IsCreated;

            // flags
            bool m_IsDynamic;

            // data
            uint32_t m_BaseIndex;
            uint32_t m_ElementCount;

        public:
            Resource(ResourceType type, ResourceTracker* tracker)
                : m_Type ( type )
                , m_Tracker ( tracker )
                , m_IsDirty ( true )
                , m_IsCreated ( false )
                , m_IsDynamic ( false )
                , m_BaseIndex ( 0 )
                , m_ElementCount ( 0 )
            {
            }

            virtual ~Resource()
            {
            }

            ResourceType GetResourceType()
            {
                return m_Type;
            }

            ResourceTracker* GetResourceTracker()
            {
                return m_Tracker;
            }

            bool IsDynamic()
            {
                return m_IsDynamic;
            }

            void SetDynamic(bool dynamic)
            {
                m_IsDynamic = dynamic;
                m_IsDirty = true;
            }

            uint32_t GetBaseIndex() const
            {
                return m_BaseIndex;
            }

            uint32_t GetElementCount() const
            {
                return m_ElementCount;
            }
            void SetElementCount(uint32_t size)
            {
                m_ElementCount = size;
                m_IsDirty = true;
            }

            virtual uint32_t GetElementSize() const = 0;

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
            virtual uint8_t* Lock() = 0;

            // unlock resource memory
            virtual void Unlock() = 0;

            // do runtime allocation of resource
            virtual bool Allocate() = 0;

            // release runtime allocation
            virtual void Release() = 0;
        };

    }
}
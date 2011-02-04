#pragma once

#include "Platform/Types.h"

#include "Foundation/Automation/Event.h" 
#include "Foundation/Math/Vector2.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/Matrix4.h"

#include "Rendering/Renderer.h"

#include "GraphicsTypes/VertexTypes.h"

#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/SceneGraph/Visitor.h"

namespace Lunar
{
    class BufferedDrawer;
}

namespace Helium
{
    namespace SceneGraph
    {
        namespace IndexElementTypes
        {
            enum IndexElementType
            {
                Unknown,
                Unsigned16,
                Unsigned32,
                Count,
            };
        }

        typedef IndexElementTypes::IndexElementType IndexElementType;

        namespace VertexElementTypes
        {
            enum VertexElementType
            {
                Unknown,
                SimpleVertex,
                ScreenVertex,
                StaticMeshVertex,
                SimpleTexturedVertex,
                Count,
            };
        }

        typedef VertexElementTypes::VertexElementType VertexElementType;

        static uint32_t IndexElementSizes[] =
        {
            0x0,                // Unknown
            sizeof( uint16_t ), // Unsigned16
            sizeof( uint32_t ), // Unsigned32
        };

        HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( IndexElementSizes ) == IndexElementTypes::Count );

        static uint32_t VertexElementSizes[] =
        {
            0x0,                                    // Unknown
            sizeof( Lunar::SimpleVertex ),          // SimpleVertex
            sizeof( Lunar::ScreenVertex ),          // ScreenVertex
            sizeof( Lunar::StaticMeshVertex< 1 > ), // StaticMeshVertex
            sizeof( Lunar::SimpleTexturedVertex ),  // SimpleTexturedVertex
        };

        HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( VertexElementSizes ) == VertexElementTypes::Count );

        static Lunar::ERendererIndexFormat IndexElementFormats[] =
        {
            Lunar::RENDERER_INDEX_FORMAT_INVALID,   // Unknown
            Lunar::RENDERER_INDEX_FORMAT_UINT16,    // Unsigned16
            Lunar::RENDERER_INDEX_FORMAT_UINT32,    // Unsigned32
        };

        HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( IndexElementFormats ) == IndexElementTypes::Count );


        //
        // Struct to be passed into all render calls in the system.  Provides a way
        // to accumulate the counts of all the different things that are rendered per frame.
        // 

        struct DrawArgs
        {
            float32_t m_WalkTime;
            float32_t m_SortTime;
            float32_t m_CompareTime;
            float32_t m_DrawTime;

            uint32_t m_EntryCount;
            uint32_t m_TriangleCount;
            uint32_t m_LineCount;

            DrawArgs()
            {
                Reset();
            }

            void Reset()
            {
                m_WalkTime = 0;
                m_SortTime = 0;
                m_CompareTime = 0;
                m_DrawTime = 0;

                m_EntryCount = 0;
                m_TriangleCount = 0;
                m_LineCount = 0;
            }
        };


        //
        // Standard static draw function callback prototype
        //

        class SceneNode;
        typedef void (*DeviceFunction)( Lunar::Renderer* renderer );
        typedef void (*SceneNodeFunction)( Lunar::Renderer* renderer, const SceneNode* node );
        typedef void (*DrawFunction)( Lunar::Renderer* renderer, DrawArgs* args, const SceneNode* node );


        //
        // Render Object structure
        //

        namespace RenderFlags
        {
            enum RenderFlag
            {
                DistanceSort = 1 << 0,
            };
        }

        class RenderVisitor;

        class RenderEntry
        {
        private:
            friend class RenderVisitor;

            // visitor we are a member of
            RenderVisitor* m_Visitor;

            // the node to draw
            const SceneNode* m_SceneNode;

            // a distance value from the camera to the object (squared)
            float32_t m_Distance;

        public:
            // the flags for this object
            uint32_t m_Flags;

            // the centroid of the object
            Vector3 m_Center;

            // the instance matrix for the object to draw
            Matrix4 m_Location;

            // called before and after instances of the same node is drawing
            SceneNodeFunction m_ObjectSetup;
            SceneNodeFunction m_ObjectReset;

            // called before and after each type of object is drawing
            DeviceFunction m_DrawSetup;
            DeviceFunction m_DrawReset;

            // the draw function to use for each object
            DrawFunction m_Draw;

        public:
            RenderEntry()
                : m_Visitor (NULL)
                , m_SceneNode (NULL)
                , m_Distance (0)
                , m_Flags (0)
                , m_ObjectSetup (NULL)
                , m_ObjectReset (NULL)
                , m_DrawSetup (NULL)
                , m_DrawReset (NULL)
                , m_Draw (NULL)
            {

            }

            static int Compare( const void* ptr1, const void* ptr2 );
        };

        typedef std::vector< RenderEntry > V_RenderEntry;
        typedef std::vector< RenderEntry* > V_RenderEntryDumbPtr;


        //
        // The deferred scene render visitor works by:
        //  - Storing per instance and per type information
        //  - Sorting the render queue by type and instance criteria
        //  - Drawing entries and handling type renderstate transitions
        //

        class Viewport;
        class Camera;

        class RenderVisitor : public Visitor
        {
        private:
            friend class RenderEntry;

            // args to update render stats to
            DrawArgs* m_Args;

            // view we are rendering for
            const SceneGraph::Viewport* m_View;

            // buffered rendering interface
            Lunar::BufferedDrawer* m_DrawInterface;

            // profile start time
            uint64_t m_StartTime;

            // compare time for sorting
            uint64_t m_CompareTime;

        public:
            RenderVisitor();

            const SceneGraph::Viewport* GetViewport()
            {
                return m_View;
            }

            Lunar::BufferedDrawer* GetDrawInterface() const
            {
                return m_DrawInterface;
            }

            DrawArgs* GetArgs() const
            {
                return m_Args;
            }

            void Reset( DrawArgs* args, const SceneGraph::Viewport* view, Lunar::BufferedDrawer* drawInterface );

            void Draw();
        };

        PIPELINE_API bool IsSupportedTexture( const tstring& file );
        PIPELINE_API Lunar::RTexture2d* LoadTexture( const tstring& file, uint32_t* textureSize = NULL, bool* hasAlpha = NULL );
    }
}
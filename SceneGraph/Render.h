#pragma once

#include "Platform/Types.h"

#include "Foundation/Event.h" 
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4.h"

#include "Rendering/Renderer.h"

#include "GraphicsTypes/VertexTypes.h"

#include "SceneGraph/Pick.h"
#include "SceneGraph/Visitor.h"

namespace Helium
{
    class BufferedDrawer;
}

namespace Helium
{
    namespace SceneGraph
    {
        class Camera;
        class Viewport;

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
            sizeof( Helium::SimpleVertex ),          // SimpleVertex
            sizeof( Helium::ScreenVertex ),          // ScreenVertex
            sizeof( Helium::StaticMeshVertex< 1 > ), // StaticMeshVertex
            sizeof( Helium::SimpleTexturedVertex ),  // SimpleTexturedVertex
        };

        HELIUM_COMPILE_ASSERT( HELIUM_ARRAY_COUNT( VertexElementSizes ) == VertexElementTypes::Count );

        static Helium::ERendererIndexFormat IndexElementFormats[] =
        {
            Helium::RENDERER_INDEX_FORMAT_INVALID,   // Unknown
            Helium::RENDERER_INDEX_FORMAT_UINT16,    // Unsigned16
            Helium::RENDERER_INDEX_FORMAT_UINT32,    // Unsigned32
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

        class RenderVisitor : public Visitor
        {
        private:
            friend class RenderEntry;

            // args to update render stats to
            DrawArgs* m_Args;

            // view we are rendering for
            const SceneGraph::Viewport* m_View;

            // buffered rendering interface
            Helium::BufferedDrawer* m_DrawInterface;

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

            Helium::BufferedDrawer* GetDrawInterface() const
            {
                return m_DrawInterface;
            }

            DrawArgs* GetArgs() const
            {
                return m_Args;
            }

            void Reset( DrawArgs* args, const SceneGraph::Viewport* view, Helium::BufferedDrawer* drawInterface );
        };
    }
}
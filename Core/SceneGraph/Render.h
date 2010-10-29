#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Platform/Types.h"

#include "Foundation/Automation/Event.h" 
#include "Foundation/Math/Vector2.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/Matrix4.h"

#include "Core/SceneGraph/Pick.h"
#include "Core/SceneGraph/Visitor.h"

namespace Helium
{
    namespace SceneGraph
    {
        namespace ElementTypes
        {
            enum ElementType
            {
                Unknown,
                Unsigned32,
                Position,
                PositionNormal,
                PositionColored,
                TransformedColored,
                StandardVertex,
                PositionUV,
                Count,
            };
        }

        typedef ElementTypes::ElementType ElementType;

        struct Position
        {
            Vector3 m_Position;

            Position()
                : m_Position (Vector3::Zero)
            {

            }

            Position(const Vector3& p)
                : m_Position (p)
            {

            }

            Position(float32_t x, float32_t y, float32_t z)
                : m_Position (x, y, z)
            {

            }
        };

        struct PositionNormal
        {
            Vector3 m_Position;
            Vector3 m_Normal;

            PositionNormal()
                : m_Position (Vector3::Zero)
                , m_Normal (Vector3::Zero)
            {

            }

            PositionNormal(const Vector3& p)
                : m_Position (p)
                , m_Normal (Vector3::Zero)
            {

            }

            PositionNormal(const Vector3& p, const Vector3& n)
                : m_Position (p)
                , m_Normal (n)
            {

            }

            PositionNormal(float32_t x, float32_t y, float32_t z)
                : m_Position (x, y, z)
                , m_Normal (Vector3::Zero)
            {

            }

            PositionNormal(float32_t xp, float32_t yp, float32_t zp, float32_t xn, float32_t yn, float32_t zn)
                : m_Position (xp, yp, zp)
                , m_Normal (xn, yn, zn)
            {

            }
        };

        struct PositionColored
        {
            Vector3 m_Position;
            uint32_t m_Color;

            PositionColored()
                : m_Position (Vector3::Zero)
                , m_Color (0)
            {

            }

            PositionColored(const Vector3& p)
                : m_Position (p)
                , m_Color (0)
            {

            }

            PositionColored(const Vector3& p, uint32_t c)
                : m_Position (p)
                , m_Color (c)
            {

            }

            PositionColored(float32_t x, float32_t y, float32_t z)
                : m_Position (x, y, z)
                , m_Color (0)
            {

            }

            PositionColored(float32_t x, float32_t y, float32_t z, uint32_t c)
                : m_Position (x, y, z)
                , m_Color (c)
            {

            }
        };

        struct TransformedColored
        {
            Vector4 m_Position;
            uint32_t m_Color;

            TransformedColored()
                : m_Position (Vector3::Zero)
                , m_Color (0)
            {

            }

            TransformedColored(const Vector3& p)
                : m_Position (p)
                , m_Color (0)
            {

            }

            TransformedColored(const Vector3& p, uint32_t c)
                : m_Position (p)
                , m_Color (c)
            {

            }

            TransformedColored(float32_t x, float32_t y, float32_t z)
                : m_Position (x, y, z, 1)
                , m_Color (0)
            {

            }

            TransformedColored(float32_t x, float32_t y, float32_t z, uint32_t c)
                : m_Position (x, y, z, 1)
                , m_Color (c)
            {

            }
        };

        struct StandardVertex
        {
            Vector3 m_Position;
            Vector3 m_Normal;
            uint32_t           m_Diffuse;
            Vector2 m_BaseUV;

            StandardVertex()
                : m_Position( Vector3::Zero )
                , m_Normal( Vector3::Zero )
                , m_Diffuse( D3DCOLOR_ARGB(1, 0, 0, 0) )
                , m_BaseUV( Vector2::Zero )
            {

            }

            StandardVertex( const Vector3& pos, const Vector3& norm, uint32_t diffuse, const Vector2& baseUV )
                : m_Position( pos )
                , m_Normal( norm )
                , m_Diffuse( diffuse )
                , m_BaseUV( baseUV )
            {

            }
        };

        struct PositionUV
        {
            Vector3 m_Position;
            Vector2 m_BaseUV;

            PositionUV( const Vector3& pos = Vector3::Zero, const Vector2& uv = Vector2::Zero )
                : m_Position( pos )
                , m_BaseUV( uv )
            {
            }
        };

        static uint32_t ElementSizes[] =
        {
            0x0,                          // Unknown
            sizeof(uint32_t),                  // Unsigned32
            sizeof(Position),             // Position
            sizeof(PositionNormal),       // PositionNormal
            sizeof(PositionColored),      // PositionColored
            sizeof(TransformedColored),   // TransformedColored
            sizeof(StandardVertex),       // StandardVertex
            sizeof(PositionUV),           // PositionUV
        };

        HELIUM_COMPILE_ASSERT(sizeof(ElementSizes) / sizeof(uint32_t) == ElementTypes::Count);

        static uint32_t ElementFormats[] =
        {
            0x0,                                                        // Unknown
            D3DFMT_INDEX32,                                             // Unsigned32
            D3DFVF_XYZ,                                                 // Position
            D3DFVF_XYZ | D3DFVF_NORMAL,                                 // PositionNormal
            D3DFVF_XYZ | D3DFVF_DIFFUSE,                                // PositionColored
            D3DFVF_XYZRHW | D3DFVF_DIFFUSE,                             // TransformedColored
            D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1,  // StandardVertex
            D3DFVF_XYZ | D3DFVF_TEX1,                                   // PositionUV
        };

        HELIUM_COMPILE_ASSERT(sizeof(ElementFormats) / sizeof(uint32_t) == ElementTypes::Count);


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
        typedef void (*DeviceFunction)( IDirect3DDevice9* device );
        typedef void (*SceneNodeFunction)( IDirect3DDevice9* device, const SceneNode* node );
        typedef void (*DrawFunction)( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* node );


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

            // the render object data pool
            V_RenderEntry m_EntryData;

            // the render object pointers to sort
            V_RenderEntryDumbPtr m_EntryPointers;

            // the device to issue draw commands to
            IDirect3DDevice9* m_Device;

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

            uint32_t GetSize() const
            {
                return (uint32_t)m_EntryData.size();
            }

            void Reset( DrawArgs* args, const SceneGraph::Viewport* view );

            RenderEntry* Allocate( const SceneNode* object );

            void Draw();
        };

        CORE_API bool IsSupportedTexture( const tstring& file );
        CORE_API IDirect3DTexture9* LoadTexture( IDirect3DDevice9* device, const tstring& file, uint32_t* textureSize = NULL, bool* hasAlpha = NULL, D3DPOOL pool = D3DPOOL_MANAGED );
    }
}
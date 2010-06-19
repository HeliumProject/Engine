#pragma once

#include "Foundation/Automation/Event.h" 
#include "Platform/Types.h"
#include "Foundation/Math/Vector2.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Vector4.h"
#include "Foundation/Math/Matrix4.h"

#include "Pick.h"
#include "Visitor.h"

namespace Luna
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
    Math::Vector3 m_Position;

    Position()
      : m_Position (Math::Vector3::Zero)
    {

    }

    Position(const Math::Vector3& p)
      : m_Position (p)
    {

    }

    Position(f32 x, f32 y, f32 z)
      : m_Position (x, y, z)
    {

    }
  };

  struct PositionNormal
  {
    Math::Vector3 m_Position;
    Math::Vector3 m_Normal;

    PositionNormal()
      : m_Position (Math::Vector3::Zero)
      , m_Normal (Math::Vector3::Zero)
    {

    }

    PositionNormal(const Math::Vector3& p)
      : m_Position (p)
      , m_Normal (Math::Vector3::Zero)
    {

    }

    PositionNormal(const Math::Vector3& p, const Math::Vector3& n)
      : m_Position (p)
      , m_Normal (n)
    {

    }

    PositionNormal(f32 x, f32 y, f32 z)
      : m_Position (x, y, z)
      , m_Normal (Math::Vector3::Zero)
    {

    }

    PositionNormal(f32 xp, f32 yp, f32 zp, f32 xn, f32 yn, f32 zn)
      : m_Position (xp, yp, zp)
      , m_Normal (xn, yn, zn)
    {

    }
  };

  struct PositionColored
  {
    Math::Vector3 m_Position;
    u32 m_Color;

    PositionColored()
      : m_Position (Math::Vector3::Zero)
      , m_Color (0)
    {

    }

    PositionColored(const Math::Vector3& p)
      : m_Position (p)
      , m_Color (0)
    {

    }

    PositionColored(const Math::Vector3& p, u32 c)
      : m_Position (p)
      , m_Color (c)
    {

    }

    PositionColored(f32 x, f32 y, f32 z)
      : m_Position (x, y, z)
      , m_Color (0)
    {

    }

    PositionColored(f32 x, f32 y, f32 z, u32 c)
      : m_Position (x, y, z)
      , m_Color (c)
    {

    }
  };

  struct TransformedColored
  {
    Math::Vector4 m_Position;
    u32 m_Color;

    TransformedColored()
      : m_Position (Math::Vector3::Zero)
      , m_Color (0)
    {

    }

    TransformedColored(const Math::Vector3& p)
      : m_Position (p)
      , m_Color (0)
    {

    }

    TransformedColored(const Math::Vector3& p, u32 c)
      : m_Position (p)
      , m_Color (c)
    {

    }

    TransformedColored(f32 x, f32 y, f32 z)
      : m_Position (x, y, z, 1)
      , m_Color (0)
    {

    }

    TransformedColored(f32 x, f32 y, f32 z, u32 c)
      : m_Position (x, y, z, 1)
      , m_Color (c)
    {

    }
  };

  struct StandardVertex
  {
    Math::Vector3 m_Position;
    Math::Vector3 m_Normal;
    u32           m_Diffuse;
    Math::Vector2 m_BaseUV;

    StandardVertex()
      : m_Position( Math::Vector3::Zero )
      , m_Normal( Math::Vector3::Zero )
      , m_Diffuse( D3DCOLOR_ARGB(1, 0, 0, 0) )
      , m_BaseUV( Math::Vector2::Zero )
    {

    }

    StandardVertex( const Math::Vector3& pos, const Math::Vector3& norm, u32 diffuse, const Math::Vector2& baseUV )
      : m_Position( pos )
      , m_Normal( norm )
      , m_Diffuse( diffuse )
      , m_BaseUV( baseUV )
    {

    }
  };

  struct PositionUV
  {
    Math::Vector3 m_Position;
    Math::Vector2 m_BaseUV;

    PositionUV( const Math::Vector3& pos = Math::Vector3::Zero, const Math::Vector2& uv = Math::Vector2::Zero )
      : m_Position( pos )
      , m_BaseUV( uv )
    {
    }
  };

  static u32 ElementSizes[] =
  {
    0x0,                          // Unknown
    sizeof(u32),                  // Unsigned32
    sizeof(Position),             // Position
    sizeof(PositionNormal),       // PositionNormal
    sizeof(PositionColored),      // PositionColored
    sizeof(TransformedColored),   // TransformedColored
    sizeof(StandardVertex),       // StandardVertex
    sizeof(PositionUV),           // PositionUV
  };

  NOC_COMPILE_ASSERT(sizeof(ElementSizes) / sizeof(u32) == ElementTypes::Count);

  static u32 ElementFormats[] =
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

  NOC_COMPILE_ASSERT(sizeof(ElementFormats) / sizeof(u32) == ElementTypes::Count);


  //
  // Struct to be passed into all render calls in the system.  Provides a way
  // to accumulate the counts of all the different things that are rendered per frame.
  // 

  struct DrawArgs
  {
    f32 m_WalkTime;
    f32 m_SortTime;
    f32 m_CompareTime;
    f32 m_DrawTime;

    u32 m_EntryCount;
    u32 m_TriangleCount;
    u32 m_LineCount;

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
    f32 m_Distance;

  public:
    // the flags for this object
    u32 m_Flags;

    // the centroid of the object
    Math::Vector3 m_Center;

    // the instance matrix for the object to draw
    Math::Matrix4 m_Location;

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

  class View;
  class Camera;

  class RenderVisitor : public Visitor
  {
  private:
    friend class RenderEntry;

    // args to update render stats to
    DrawArgs* m_Args;

    // view we are rendering for
    const Luna::View* m_View;

    // the render object data pool
    V_RenderEntry m_EntryData;

    // the render object pointers to sort
    V_RenderEntryDumbPtr m_EntryPointers;

    // the device to issue draw commands to
    IDirect3DDevice9* m_Device;

    // profile start time
    u64 m_StartTime;

    // compare time for sorting
    u64 m_CompareTime;

  public:
    RenderVisitor();

    const Luna::View* GetView()
    {
      return m_View;
    }

    u32 GetSize() const
    {
      return (u32)m_EntryData.size();
    }
    
    void Reset( DrawArgs* args, const Luna::View* view );

    RenderEntry* Allocate( const SceneNode* object );

    void Draw();
  };

  bool IsSupportedTexture( const std::string& file );
  IDirect3DTexture9* LoadTexture( IDirect3DDevice9* device, const std::string& file, u32* textureSize = NULL, bool* hasAlpha = NULL, D3DPOOL pool = D3DPOOL_MANAGED );
}

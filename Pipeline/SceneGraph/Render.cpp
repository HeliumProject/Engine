/*#include "Precompile.h"*/
#include "Render.h"

#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/Camera.h"
#include "Pipeline/SceneGraph/SceneNode.h"

#include "Foundation/Log.h"
#include "Foundation/String/Wildcard.h"
#include "Foundation/String/Tokenize.h"

using namespace Helium;
using namespace Helium::SceneGraph;

RenderVisitor::RenderVisitor()
: m_Args (NULL)
, m_View (NULL)
, m_Device (NULL)
, m_StartTime (0x0)
{

}

void RenderVisitor::Reset( DrawArgs* args, const SceneGraph::Viewport* view )
{
  m_Args = args;
  m_View = view;
  m_Device = view->GetDevice();

  // if you hit this then you are leaking entries in the state stack, BAD :P
  HELIUM_ASSERT( m_States.size() == 1 );
  m_States.clear();
  m_States.resize( 1 );

  m_EntryData.clear();
  m_EntryPointers.clear();

  m_StartTime = Helium::TimerGetClock();
}

int RenderEntry::Compare( const void* ptr1, const void* ptr2 )
{
  const RenderEntry* left = *(const RenderEntry**)ptr1;
  const RenderEntry* right = *(const RenderEntry**)ptr2;

  struct CompareTimer
  {
    uint64_t m_Start;
    RenderVisitor* m_Visitor;

    CompareTimer(RenderVisitor* visitor)
      : m_Visitor (visitor)
    {
      m_Start = Helium::TimerGetClock();
    }

    ~CompareTimer()
    {
      m_Visitor->m_CompareTime += Helium::TimerGetClock() - m_Start;
    };
  };

  CompareTimer timer (left->m_Visitor);
  HELIUM_ASSERT(left->m_Visitor == right->m_Visitor);

  // if our alpha flags are different, sort by alpha / no alpha
  if ((left->m_Flags & RenderFlags::DistanceSort) != (right->m_Flags & RenderFlags::DistanceSort))
  {
    // if the left one has alpha, its greater so it will be drawn after
    return left->m_Flags & RenderFlags::DistanceSort ? 1 : -1;
  }

  // next sort by distance to the camera (if we are in the alpha group)
  if (left->m_Flags & RenderFlags::DistanceSort)
  {
    return (right->m_Distance - left->m_Distance) > 0 ? 1 : -1;
  }

  // next sort by selection bit, this causes the selected items to be drawn after
  if (left->m_SceneNode && right->m_SceneNode)
  {
    bool leftSelected = left->m_SceneNode->IsSelected();
    bool rightSelected = right->m_SceneNode->IsSelected();
    if ( leftSelected != rightSelected )
    {
      return leftSelected ? 1 : -1;
    }
  }

  // next sort by the object (this sorts by vertex/index resource streams)
  if (left->m_SceneNode > right->m_SceneNode)
  {
    return -1;
  }
  else if (left->m_SceneNode < right->m_SceneNode)
  {
    return 1;
  }

  // next sort by render state setup function (within the same object's resources)
  if (left->m_DrawSetup > right->m_DrawSetup)
  {
    return -1;
  }
  else if (left->m_DrawSetup < right->m_DrawSetup)
  {
    return 1;
  }

  // next sort by actual draw function (this probably doesn't make much difference)
  if (left->m_Draw > right->m_Draw)
  {
    return -1;
  }
  else if (left->m_Draw < right->m_Draw)
  {
    return 1;
  }

  return 0;
}

RenderEntry* RenderVisitor::Allocate(const SceneNode* node)
{
  static RenderEntry renderObject;

  size_t capacity = m_EntryData.capacity();

  m_EntryData.push_back( renderObject );
  m_EntryData.back().m_Visitor = this;
  m_EntryData.back().m_SceneNode = node;

  if (m_EntryData.capacity() > capacity)
  {
    Log::Debug( TXT( "RenderEntries reallocated from %d to %d\n" ), capacity * sizeof(RenderEntry), m_EntryData.capacity() * sizeof(RenderEntry));
  }

  return &m_EntryData.back();
}

void RenderVisitor::Draw()
{
  m_Args->m_WalkTime = Helium::CyclesToMillis( Helium::TimerGetClock() - m_StartTime );

  // reset
  m_Device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&Matrix4::Identity );

  if (!m_EntryData.empty())
  {
    Vector3 camera;
    m_View->GetCamera()->GetPosition(camera);

    {
      SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Setup") );

      // make a block of pointers to sort
      m_EntryPointers.resize(m_EntryData.size());
      V_RenderEntry::iterator itr = m_EntryData.begin();
      V_RenderEntry::iterator end = m_EntryData.end();
      for ( size_t index = 0; itr != end; ++itr, ++index )
      {
        RenderEntry* entry = &(*itr);

        m_EntryPointers[ index ] = entry;

        if ( entry->m_Flags | RenderFlags::DistanceSort)
        {
          entry->m_Location.TransformVertex(entry->m_Center);
          entry->m_Distance = (entry->m_Center - camera).LengthSquared();
        }
      }
    }

    {
      SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Sort") );

      uint64_t start = Helium::TimerGetClock();
      m_CompareTime = 0x0;

      qsort( &m_EntryPointers.front(), m_EntryPointers.size(), sizeof(RenderEntry*), &RenderEntry::Compare );

      m_Args->m_SortTime = Helium::CyclesToMillis( Helium::TimerGetClock() - start );
      m_Args->m_CompareTime = Helium::CyclesToMillis( m_CompareTime );
    }

    {
      SCENE_GRAPH_RENDER_SCOPE_TIMER( ("Draw") );

      uint64_t start = Helium::TimerGetClock();

      DrawFunction draw = NULL;
      DeviceFunction drawReset = NULL;

      SceneNode* node = NULL;
      SceneNodeFunction nodeReset = NULL;

      V_RenderEntryDumbPtr::const_iterator itr = m_EntryPointers.begin();
      V_RenderEntryDumbPtr::const_iterator end = m_EntryPointers.end();
      for ( ; itr != end; ++itr )
      {
        const RenderEntry* entry (*itr);

        // on draw function change
        if (entry->m_Draw != draw)
        {
          // reset the render state back to default
          if (drawReset)
          {
            drawReset( m_Device );
          }

          // if our new entry has a setup function
          if (entry->m_DrawSetup)
          {
            // setup render state for new draw function
            entry->m_DrawSetup( m_Device );
          }

          // cache reset function for next draw function change (could be NULL)
          drawReset = entry->m_DrawReset;
        }

        // on object object change
        if (entry->m_SceneNode != node)
        {
          // reset the render state back to default
          if (nodeReset)
          {
            nodeReset( m_Device, node );
          }

          // if our new entry has a setup function
          if (entry->m_ObjectSetup)
          {
            // setup render state for new object function
            entry->m_ObjectSetup( m_Device, entry->m_SceneNode );
          }

          // cache reset function for next object function change (could be NULL)
          nodeReset = entry->m_ObjectReset;
        }

        // the the world transform for this entry
        m_Device->SetTransform( D3DTS_WORLD, (D3DMATRIX*)&entry->m_Location );

        // draw this entry
        entry->m_Draw(m_Device, m_Args, entry->m_SceneNode);
      }

      // if our last entry has a reset function
      if (drawReset)
      {
        // reset render state back to default
        drawReset( m_Device );
      }

      // if our last entry has a reset function
      if (nodeReset)
      {
        // reset render state back to default
        nodeReset( m_Device, node );
      }

      m_Args->m_DrawTime = Helium::CyclesToMillis( Helium::TimerGetClock() - start );
    }
  }
}

bool SceneGraph::IsSupportedTexture( const tstring& file )
{
  static const tchar_t* extensions[] = {
      TXT( "*.bmp" ),
      TXT( "*.dds" ),
      TXT( "*.dib" ),
      TXT( "*.hdr" ),
      TXT( "*.jpg" ),
      TXT( "*.pfm" ),
      TXT( "*.png" ),
      TXT( "*.ppm" ),
      TXT( "*.tga" ),
      NULL
  };

  for ( const tchar_t** ext = extensions; *ext != NULL; ext++ )
  {
    if ( WildcardMatch( *ext, file.c_str() ) )
    {
      return true;
    }
  }

  return false;
}

IDirect3DTexture9* SceneGraph::LoadTexture( IDirect3DDevice9* device, const tstring& file, uint32_t* textureSize, bool* hasAlpha, D3DPOOL pool )
{
  IDirect3DTexture9* texture = NULL;
  bool alpha = false;

  if (!file.empty())
  {
      if ( Helium::Path( file ).Exists() )
    {
      D3DFORMAT textureFormat = D3DFMT_DXT1; 

      D3DXIMAGE_INFO sourceInfo;
      LPDIRECT3DTEXTURE9 tempTexture;
    
      if ( D3DXCreateTextureFromFileEx( device,
                                        file.c_str(),
                                        D3DX_DEFAULT,
                                        D3DX_DEFAULT,
                                        D3DX_DEFAULT,
                                        NULL,
                                        D3DFMT_A8R8G8B8,
                                        pool,
                                        D3DX_DEFAULT,        // filter? 
                                        D3DX_FILTER_NONE,    // mip filter
                                        NULL,
                                        &sourceInfo,
                                        NULL,
                                        &tempTexture ) == D3D_OK )
      {
        // lock the first mip level, and iterate over all pixels looking for
        // alpha != 0 || alpha != 255
        D3DLOCKED_RECT lockedRect; 
        HRESULT locked = tempTexture->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY); 
        
        D3DSURFACE_DESC desc; 
        tempTexture->GetLevelDesc(0, &desc); 

        for(uint32_t r = 0; r < desc.Height && textureFormat == D3DFMT_DXT1; r++)
        {
          uint32_t* pixels = (uint32_t*) (((uint8_t*)lockedRect.pBits) + lockedRect.Pitch * r); 
          for(uint32_t c = 0; c < desc.Width; c++)
          {
            uint32_t masked = pixels[c] & 0xFF000000; 
            if(masked != 0xFF000000)
            {
              alpha = true; 
            }
            if(masked != 0xFF000000 && masked != 0x00000000)
            {
              textureFormat = D3DFMT_DXT5; 
              break; 
            }
          }
        }

        tempTexture->UnlockRect(0);

        tempTexture->Release(); 
      }

      int compressionRatio;
      switch ( textureFormat )
      {
      case D3DFMT_DXT1:
        {
          if (alpha)
          {
            compressionRatio = 8;
          }
          else
          {
            compressionRatio = 6;
          }
          break;
        }

      case D3DFMT_DXT5:
        {
          compressionRatio = 4;
          break;
        }

      default:
        {
          compressionRatio = 1;
          HELIUM_BREAK();
          break;
        }
      }

      if ( D3DXCreateTextureFromFileEx( device,
                                        file.c_str(),
                                        D3DX_DEFAULT,
                                        D3DX_DEFAULT,
                                        D3DX_DEFAULT,
                                        NULL,
                                        textureFormat, 
                                        pool,
                                        D3DX_DEFAULT,
                                        D3DX_DEFAULT,
                                        NULL,
                                        &sourceInfo,
                                        NULL,
                                        &texture ) == D3D_OK )
      {
        texture->SetAutoGenFilterType( D3DTEXF_ANISOTROPIC );

        if ( textureSize )
        {
          *textureSize = 0; 
          for(uint32_t i = 0; i < texture->GetLevelCount(); i++)
          {
            D3DSURFACE_DESC desc; 
            texture->GetLevelDesc(i, &desc); 
            *textureSize += desc.Width * desc.Height * 4 / compressionRatio;
          }
        }
      }
      else
      {
        Log::Warning( TXT( "Unable to create texture from file '%s'\n" ), file.c_str() );
      }
    }
    else
    {
      Log::Warning( TXT( "File '%s' does not exist\n" ), file.c_str() );
    }
  }

  if ( hasAlpha )
  {
    *hasAlpha = alpha;
  }

  return texture;
}

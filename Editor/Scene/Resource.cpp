#include "Precompile.h"
#include "Resource.h"
#include "Statistics.h"

#include "IndexResource.h"
#include "VertexResource.h"

#include "Foundation/Profile.h"
#include "Foundation/Log.h"
#include "Foundation/Container/Insert.h" 

#include <algorithm>

using Helium::Insert; 
using namespace Editor;



ResourceTracker::ResourceTracker(IDirect3DDevice9* device)
: m_Device (device)
{
  if (!IndexResource::s_MemoryPool.Valid())
  {
    IndexResource::s_MemoryPool = Profile::Memory::CreatePool( TXT( "Direct3D Index Data" ) );
  }

  if (!VertexResource::s_MemoryPool.Valid())
  {
    VertexResource::s_MemoryPool = Profile::Memory::CreatePool( TXT( "Direct3D Vertex Data" ) );
  }
}

ResourceTracker::~ResourceTracker()
{

}

void ResourceTracker::ResetState()
{
  m_Indices = NULL;
  m_Vertices = NULL;
  m_VertexFormat = ElementFormats[ ElementTypes::Unknown ];
}

void ResourceTracker::Register(Resource* resource)
{
  Insert<S_Resource>::Result inserted = m_Resources.insert( resource );
  HELIUM_ASSERT(inserted.second);
}

void ResourceTracker::Release(Resource* resource)
{
  m_Resources.erase( resource );
}

void ResourceTracker::DeviceLost()
{
  S_Resource::const_iterator itr = m_Resources.begin();
  S_Resource::const_iterator end = m_Resources.end();
  for ( ; itr != end ; ++itr )
  {
    if ( !(*itr)->IsManaged() )
    {
      (*itr)->Delete();
    }
  }
}

void ResourceTracker::DeviceReset()
{
  S_Resource::const_iterator itr = m_Resources.begin();
  S_Resource::const_iterator end = m_Resources.end();
  for ( ; itr != end ; ++itr )
  {
    if ( !(*itr)->IsManaged() )
    {
      (*itr)->Create();
    }
  }
}

void Resource::Create()
{
  if ( !m_IsCreated )
  {
    m_IsCreated = true;

    if (Allocate())
    {
      Populate();
    }
  }
}

void Resource::Delete()
{
  if ( m_IsCreated )
  {
    m_IsCreated = false;

    Release();
  }
}

void Resource::Update()
{
  if (m_IsCreated)
  {
    if (m_IsDirty || IsManaged())
    {
      if (!m_IsDirty && IsManaged())
      {
        Log::Debug( TXT( "Re-creating non-default resource '%s'...\n" ), typeid(*this).name() );
      }

      Delete();
      Create();
    }
    else
    {
      Populate();
    }
  }
  else
  {
    Create();
  }
}

void Resource::Populate()
{
  if (m_Populator.Valid())
  {
    PopulateArgs args ( m_Type, m_BaseIndex = 0, Lock() );

    if (args.m_Buffer)
    {
      u32 offset = args.m_Offset;

      m_Populator.Invoke( &args );

      HELIUM_ASSERT( args.m_Offset == ( offset + ElementSizes[ m_ElementType ] * m_ElementCount ) );

      Unlock();
    }
    else
    {
      HELIUM_BREAK(); // this is BAD
    }
  }
}

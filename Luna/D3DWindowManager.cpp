#include "Precompile.h"
#include "D3DWindowManager.h"

using namespace Luna;

D3DWindowManager::D3DWindowManager()
: m_IsLost( false )
{
}

D3DWindowManager::~D3DWindowManager()
{
}

HRESULT D3DWindowManager::HandleClientDefaultPool( u32 reason )
{
  if ( reason == Render::DEFPOOL_RELEASE )
  {
    m_Lost.Raise( DeviceStates::Lost );
  }
  else if ( reason == Render::DEFPOOL_CREATE )
  {
    m_Found.Raise( DeviceStates::Found );
  }
  return S_OK;
}

bool D3DWindowManager::TestDeviceReady()
{
  IDirect3DDevice9* device = GetD3DDevice();
  if ( !device )
  {
    return false;
  }

  if ( !m_IsLost )
  {
    return true;
  }

  HRESULT result = device->TestCooperativeLevel();
  if ( result == D3DERR_DEVICENOTRESET )
  {
    Reset();
    result = device->TestCooperativeLevel();
  }

  m_IsLost = result != D3D_OK;

  return !m_IsLost;
}

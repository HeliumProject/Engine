#pragma once

#include "Platform/Compiler.h"
#include "Foundation/Automation/Event.h"
#include "igDXRender/d3dmanager.h"

namespace Luna
{
  namespace DeviceStates
  {
    enum DeviceState
    {
      Found,
      Lost
    };
  }
  typedef DeviceStates::DeviceState DeviceState;

  struct DeviceStateArgs
  {
    DeviceState m_DeviceState;

    DeviceStateArgs( DeviceState state )
      : m_DeviceState( state )
    {
    }
  };
  typedef Nocturnal::Signature< void, const DeviceStateArgs& > DeviceStateSignature;

  class D3DWindowManager : public igDXRender::D3DManager
  {
  public:
    D3DWindowManager();
    virtual ~D3DWindowManager();

    virtual HRESULT HandleClientDefaultPool( u32 reason ) NOC_OVERRIDE;

    bool TestDeviceReady();

    inline bool IsDeviceLost() const
    {
      return m_IsLost;
    }

    inline void SetDeviceLost( bool lost = true )
    {
      m_IsLost = lost;
    }

  private:
    bool m_IsLost;

  private:
    DeviceStateSignature::Event m_Found;
  public:
    void AddDeviceFoundListener( const DeviceStateSignature::Delegate& listener )
    {
      m_Found.Add( listener );
    }
    void RemoveDeviceFoundListener( const DeviceStateSignature::Delegate& listener )
    {
      m_Found.Remove( listener );
    }
  private:
    DeviceStateSignature::Event m_Lost;
  public:
    void AddDeviceLostListener( const DeviceStateSignature::Delegate& listener )
    {
      m_Lost.Add( listener );
    }
    void RemoveDeviceLostListener( const DeviceStateSignature::Delegate& listener )
    {
      m_Lost.Remove( listener );
    }
  };
}

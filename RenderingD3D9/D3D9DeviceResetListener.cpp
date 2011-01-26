#include "RenderingD3D9Pch.h"
#include "RenderingD3D9/D3D9DeviceResetListener.h"

using namespace Lunar;

/// Constructor.
D3D9DeviceResetListener::D3D9DeviceResetListener()
    : m_pPreviousListener( NULL )
    , m_pNextListener( NULL )
{
}

/// Copy constructor.
///
/// @param[in] rSource  Source object from which to copy.
D3D9DeviceResetListener::D3D9DeviceResetListener( const D3D9DeviceResetListener& /*rSource*/ )
{
    // Listeners must have their own entries in the renderer's listener list, so we never copy the link values.
}

/// Destructor.
///
/// This will automatically unregister this listener from the D3D9Renderer instance.
D3D9DeviceResetListener::~D3D9DeviceResetListener()
{
    D3D9Renderer* pRenderer = static_cast< D3D9Renderer* >( Renderer::GetStaticInstance() );
    HELIUM_ASSERT( pRenderer );

    pRenderer->UnregisterDeviceResetListener( this );
}

/// @fn void D3D9DeviceResetListener::OnPreReset()
/// Handle events issued when the Direct3D device is about to be reset.  This typically involves releasing resources
/// that have been allocated in the D3DPOOL_DEFAULT pool.
///
/// @see OnPostReset()

/// @fn void D3D9DeviceResetListener::OnPostReset( D3D9Renderer* pRenderer )
/// Handle events issued after immediately after the Direct3D device has been reset.  This typically involves
/// reallocating and reinitializing resources that have been allocated in the D3DPOOL_DEFAULT pool.
///
/// @param[in] pRenderer  Pointer to the Direct3D 9 renderer instance.
///
/// @see OnPreReset()

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
D3D9DeviceResetListener& D3D9DeviceResetListener::operator=( const D3D9DeviceResetListener& /*rOther*/ )
{
    // Listeners must have their own entries in the renderer's listener list, so we never copy the link values.
    return *this;
}

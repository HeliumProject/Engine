#include "FrameworkPch.h"
#include "Framework/Camera.h"

#include "Math/SimdVectorConversion.h"

HELIUM_IMPLEMENT_OBJECT( Helium::Camera, Framework, 0 );

using namespace Helium;

const float32_t Camera::DEFAULT_FOV = 70.0f;

/// Constructor.
Camera::Camera()
: m_sceneViewId( Invalid< uint32_t >() )
, m_fov( DEFAULT_FOV )
{
}

/// Destructor.
Camera::~Camera()
{
}

//PMDTODO: Implement this
///// @copydoc GameObject::Serialize()
//void Camera::Serialize( Serializer& s )
//{
//    HELIUM_SERIALIZE_BASE( s );
//
//    s << HELIUM_TAGGED( m_fov );
//}

/// @copydoc Entity::Attach()
void Camera::Attach()
{
    Base::Attach();

    // Update the scene view associated with this camera.
    if( IsValid( m_sceneViewId ) )
    {
        World* pWorld = GetWorld();
        HELIUM_ASSERT( pWorld );
        GraphicsScene* pScene = pWorld->GetGraphicsScene();
        if( pScene )
        {
            GraphicsSceneView* pSceneView = pScene->GetSceneView( m_sceneViewId );
            if( pSceneView )
            {
                Simd::Matrix44 rotationMatrix( Simd::Matrix44::INIT_ROTATION, GetRotation() );
                pSceneView->SetView(
                    GetPosition(),
                    Vector4ToVector3( rotationMatrix.GetRow( 2 ) ),
                    Vector4ToVector3( rotationMatrix.GetRow( 1 ) ) );
                pSceneView->SetHorizontalFov( m_fov );
            }
        }
    }
}

/// @copydoc Entity::SetPosition()
void Camera::SetPosition( const Simd::Vector3& rPosition )
{
    Base::SetPosition( rPosition );

    DeferredReattach();
}

/// @copydoc Entity::SetRotation()
void Camera::SetRotation( const Simd::Quat& rRotation )
{
    Base::SetRotation( rRotation );

    DeferredReattach();
}

/// Set the ID scene view associated with this camera.
///
/// Changing the scene view will trigger a reattach and update of the scene view based on the camera parameters.
///
/// @param[in] sceneViewId  ID of the scene view to associate with this camera.
///
/// @see GetSceneViewId()
void Camera::SetSceneViewId( uint32_t sceneViewId )
{
    VerifySafety();

    if( m_sceneViewId != sceneViewId )
    {
        m_sceneViewId = sceneViewId;
        DeferredReattach();
    }
}

/// Set the horizontal field-of-view angle.
///
/// @param[in] fov  Field-of-view angle, in degrees.
///
/// @see GetFov()
void Camera::SetFov( float32_t fov )
{
    VerifySafety();

    HELIUM_ASSERT( fov >= HELIUM_EPSILON );
    m_fov = Clamp( fov, HELIUM_EPSILON, 180.0f );

    DeferredReattach();
}

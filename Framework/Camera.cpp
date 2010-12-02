//----------------------------------------------------------------------------------------------------------------------
// Camera.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "FrameworkPch.h"
#include "Framework/Camera.h"

#include "Platform/Math/Simd/VectorConversion.h"

using namespace Lunar;

L_IMPLEMENT_OBJECT( Camera, Framework, 0 );

const float32_t Camera::DEFAULT_FOV = 70.0f;

/// Constructor.
Camera::Camera()
: m_fov( DEFAULT_FOV )
{
}

/// Destructor.
Camera::~Camera()
{
}

/// @copydoc GameObject::Serialize()
void Camera::Serialize( Serializer& s )
{
    L_SERIALIZE_SUPER( s );

    s << L_TAGGED( m_fov );
}

/// @copydoc Entity::Attach()
void Camera::Attach()
{
    Super::Attach();

    // Get the main scene view.
    World* pWorld = GetWorld();
    HELIUM_ASSERT( pWorld );

    size_t mainSceneViewId = pWorld->GetMainSceneViewId();
    if( IsValid( mainSceneViewId ) )
    {
        GraphicsScene* pScene = pWorld->GetGraphicsScene();
        HELIUM_ASSERT( pScene );
        GraphicsSceneView* pSceneView = pScene->GetSceneView( mainSceneViewId );
        HELIUM_ASSERT( pSceneView );

        Simd::Matrix44 rotationMatrix( Simd::Matrix44::INIT_ROTATION, GetRotation() );
        pSceneView->SetView(
            GetPosition(),
            Vector4ToVector3( rotationMatrix.GetRow( 2 ) ),
            Vector4ToVector3( rotationMatrix.GetRow( 1 ) ) );
        pSceneView->SetHorizontalFov( m_fov );
    }
}

/// @copydoc Entity::SetPosition()
void Camera::SetPosition( const Simd::Vector3& rPosition )
{
    Super::SetPosition( rPosition );

    DeferredReattach();
}

/// @copydoc Entity::SetRotation()
void Camera::SetRotation( const Simd::Quat& rRotation )
{
    Super::SetRotation( rRotation );

    DeferredReattach();
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

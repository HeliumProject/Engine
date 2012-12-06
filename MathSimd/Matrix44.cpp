#include "MathSimdPch.h"
#include "MathSimd/Matrix44.h"
#include "Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Matrix44 );

const Helium::Simd::Matrix44 Helium::Simd::Matrix44::IDENTITY(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f );

void Helium::Simd::Matrix44::PopulateComposite( Reflect::Composite& comp )
{
#pragma TODO("Support static arrays in reflect")
    comp.AddField( &Matrix44::m_m00,       TXT( "m_m00" ) );
    comp.AddField( &Matrix44::m_m01,       TXT( "m_m01" ) );
    comp.AddField( &Matrix44::m_m02,       TXT( "m_m02" ) );
    comp.AddField( &Matrix44::m_m03,       TXT( "m_m03" ) );
    comp.AddField( &Matrix44::m_m10,       TXT( "m_m10" ) );
    comp.AddField( &Matrix44::m_m11,       TXT( "m_m11" ) );
    comp.AddField( &Matrix44::m_m12,       TXT( "m_m12" ) );
    comp.AddField( &Matrix44::m_m13,       TXT( "m_m13" ) );
    comp.AddField( &Matrix44::m_m20,       TXT( "m_m20" ) );
    comp.AddField( &Matrix44::m_m21,       TXT( "m_m21" ) );
    comp.AddField( &Matrix44::m_m22,       TXT( "m_m22" ) );
    comp.AddField( &Matrix44::m_m23,       TXT( "m_m23" ) );
    comp.AddField( &Matrix44::m_m30,       TXT( "m_m30" ) );
    comp.AddField( &Matrix44::m_m31,       TXT( "m_m31" ) );
    comp.AddField( &Matrix44::m_m32,       TXT( "m_m32" ) );
    comp.AddField( &Matrix44::m_m33,       TXT( "m_m33" ) );
}

/// Set this matrix to a perspective projection matrix.
///
/// @param[in] horizontalFovRadians  Horizontal field-of-view angle, in radians.
/// @param[in] aspectRatio           Aspect ratio (width to height).
/// @param[in] nearClip              Near clip plane distance.
/// @param[in] farClip               Far clip plane distance.
///
/// @see SetOrthographicProjection()
void Helium::Simd::Matrix44::SetPerspectiveProjection(
    float32_t horizontalFovRadians,
    float32_t aspectRatio,
    float32_t nearClip,
    float32_t farClip )
{
    float32_t xScale = 1.0f / Tan( horizontalFovRadians * 0.5f );
    float32_t yScale = xScale * aspectRatio;

    float32_t zScale = farClip / ( farClip - nearClip );
    float32_t tScale = -nearClip * zScale;

    *this = Matrix44(
        xScale, 0.0f,   0.0f,   0.0f,
        0.0f,   yScale, 0.0f,   0.0f,
        0.0f,   0.0f,   zScale, 1.0f,
        0.0f,   0.0f,   tScale, 0.0f );
}

/// Set this matrix to a perspective projection matrix with an infinite far clip plane.
///
/// @param[in] horizontalFovRadians  Horizontal field-of-view angle, in radians.
/// @param[in] aspectRatio           Aspect ratio (width to height).
/// @param[in] nearClip              Near clip plane distance.
///
/// @see SetOrthographicProjection()
void Helium::Simd::Matrix44::SetPerspectiveProjection( float32_t horizontalFovRadians, float32_t aspectRatio, float32_t nearClip )
{
    float32_t xScale = 1.0f / Tan( horizontalFovRadians * 0.5f );
    float32_t yScale = xScale * aspectRatio;

    float32_t zScale = 1.0f;
    float32_t tScale = -nearClip;

    *this = Matrix44(
        xScale, 0.0f,   0.0f,   0.0f,
        0.0f,   yScale, 0.0f,   0.0f,
        0.0f,   0.0f,   zScale, 1.0f,
        0.0f,   0.0f,   tScale, 0.0f );
}

/// Set this matrix to an orthogonal projection matrix.
///
/// @param[in] width     View volume width.
/// @param[in] height    View volume height.
/// @param[in] nearClip  Near clip plane distance.
/// @param[in] farClip   Far clip plane distance.
///
/// @see SetPerspectiveProjection()
void Helium::Simd::Matrix44::SetOrthographicProjection( float32_t width, float32_t height, float32_t nearClip, float32_t farClip )
{
    float32_t xScale = 2.0f / width;
    float32_t yScale = 2.0f / height;

    float32_t zScale = 1.0f / ( farClip - nearClip );
    float32_t tScale = -nearClip * zScale;

    *this = Matrix44(
        xScale, 0.0f,   0.0f,   0.0f,
        0.0f,   yScale, 0.0f,   0.0f,
        0.0f,   0.0f,   zScale, 0.0f,
        0.0f,   0.0f,   tScale, 1.0f );
}
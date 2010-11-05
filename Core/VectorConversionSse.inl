//----------------------------------------------------------------------------------------------------------------------
// VectorConversionSse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Convert a Vector3 directly to a Vector4, leaving the w-component undefined.
    ///
    /// This provides the fastest conversion from a Vector3 to a Vector4, although the w-component of the resulting
    /// Vector4 will be undefined.  PointToVector4() and RayToVector4() can be used if special handling of the
    /// w-component is necessary, at somewhat of a loss to performance.
    ///
    /// @param[in] rVector  Vector3 to convert.
    ///
    /// @return  Converted Vector4.
    ///
    /// @see PointToVector4(), RayToVector4(), Vector4ToVector3()
    Vector4 Vector3ToVector4( const Vector3& rVector )
    {
        return Vector4( rVector.GetSimdVector() );
    }

    /// Convert a Vector3 to a Vector4, setting the w-component to 1.
    ///
    /// Setting the w-component to 1 will ensure transformations of the resulting Vector4 using a Matrix44 will
    /// transform the vector as a point in 3D space.  If the value of the w-component vector does not matter or will be
    /// altered later, Vector3ToVector4 can be used to perform a somewhat faster conversion while leaving the
    /// w-component undefined.
    ///
    /// @param[in] rVector  Vector3 to convert.
    ///
    /// @return  Converted Vector4.
    ///
    /// @see RayToVector4(), Vector3ToVector4(), Vector4ToVector3()
    Vector4 PointToVector4( const Vector3& rVector )
    {
        L_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] L_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        L_SIMD_ALIGN_PRE const float32_t one[ 4 ] L_SIMD_ALIGN_POST = { 0.0f, 0.0f, 0.0f, 1.0f };

        SimdVector componentMaskVector = Simd::LoadAligned( componentMask );
        SimdVector oneVector = Simd::LoadAligned( one );

        SimdVector vector3Masked = Simd::And( rVector.GetSimdVector(), componentMaskVector );

        return Vector4( Simd::Or( vector3Masked, oneVector ) );
    }

    /// Convert a Vector3 to a Vector4, setting the w-component to 0.
    ///
    /// Setting the w-component to 0 will ensure transformations of the resulting Vector4 using a Matrix44 will
    /// transform the vector as a ray or direction in 3D space.  If the value of the w-component vector does not matter
    /// or will be altered later, Vector3ToVector4 can be used to perform a somewhat faster conversion while leaving the
    /// w-component undefined.
    ///
    /// @param[in] rVector  Vector3 to convert.
    ///
    /// @return  Converted Vector4.
    ///
    /// @see PointToVector4(), Vector3ToVector4(), Vector4ToVector3()
    Vector4 RayToVector4( const Vector3& rVector )
    {
        L_SIMD_ALIGN_PRE const uint32_t componentMask[ 4 ] L_SIMD_ALIGN_POST =
        {
            0xffffffff,
            0xffffffff,
            0xffffffff,
            0
        };

        SimdVector componentMaskVector = Simd::LoadAligned( componentMask );

        return Vector4( Simd::And( rVector.GetSimdVector(), componentMaskVector ) );
    }

    /// Convert a Vector4 to a Vector3.
    ///
    /// @param[in] rVector  Vector4 to convert.
    ///
    /// @return  Converted Vector3.
    ///
    /// @see Vector3ToVector4(), PointToVector4(), RayToVector4()
    Vector3 Vector4ToVector3( const Vector4& rVector )
    {
        return Vector3( rVector.GetSimdVector() );
    }
}

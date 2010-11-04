//----------------------------------------------------------------------------------------------------------------------
// Plane.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates a plane with all components uninitialized.  Values should be assigned before use.
    Plane::Plane()
    {
    }

    /// Constructor.
    ///
    /// Initializes this plane based on a vector normal to the plane and the distance of the plane from the origin.
    ///
    /// @param[in] rNormal   Plane normal.
    /// @param[in] distance  Distance of the plane from the origin along the plane normal, scaled by the normal
    ///                      magnitude.
    Plane::Plane( const Vector3& rNormal, float32_t distance )
    {
        Set( rNormal, distance );
    }

    /// Constructor.
    ///
    /// Initializes this plane based on three points on the plane.  The plane normal is computed using the cross product
    /// of the vector from the first point to the second and the vector from the first point to the third.
    ///
    /// Note that the plane normal is normalized automatically using Vector3::Normalize() with the default epsilon
    /// value.
    ///
    /// @param[in] rPoint0  First point on the plane.
    /// @param[in] rPoint1  Second point on the plane.
    /// @param[in] rPoint2  Third point on the plane.
    Plane::Plane( const Vector3& rPoint0, const Vector3& rPoint1, const Vector3& rPoint2 )
    {
        Set( rPoint0, rPoint1, rPoint2 );
    }

#if HELIUM_SIMD_SIZE == 16
    /// Get the contents of this plane as a SIMD vector.
    ///
    /// @return  Reference to the SIMD vector in which this plane is stored.
    ///
    /// @see SetSimdVector()
    Helium::SimdVector& Plane::GetSimdVector()
    {
        return m_plane;
    }

    /// Get the contents of this plane as a SIMD vector.
    ///
    /// @return  Constant reference to the SIMD vector in which this plane is stored.
    ///
    /// @see SetSimdVector()
    const Helium::SimdVector& Plane::GetSimdVector() const
    {
        return m_plane;
    }

    /// Set the contents of this plane to the given SIMD vector.
    ///
    /// @param[in] rVector  SIMD vector.
    ///
    /// @see GetSimdVector()
    void Plane::SetSimdVector( const Helium::SimdVector& rVector )
    {
        m_plane = rVector;
    }
#endif  // HELIUM_SIMD_SIZE == 16

    /// Get the normal vector for this plane.
    ///
    /// @return  Plane normal vector.
    Vector3 Plane::GetNormal() const
    {
#if HELIUM_SIMD_SIZE == 16
        return Vector3( m_plane );
#else
        Vector3 result;
        MemoryCopy( &result, m_plane, sizeof( result ) );

        return result;
#endif
    }

    /// Get a copy of this plane with the plane normal normalized and the D component adjusted accordingly, with safety
    /// threshold checking.
    ///
    /// If the magnitude of the plane normal is below the given epsilon, the normal will be set to a unit vector
    /// pointing along the x-axis, and the D component will be set to zero.
    ///
    /// @param[in] epsilon  Threshold at which to test for zero-length plane normals.
    ///
    /// @return  Normalized copy of this plane.
    ///
    /// @see Normalize()
    Plane Plane::GetNormalized( float32_t epsilon ) const
    {
        Plane result = *this;
        result.Normalize( epsilon );

        return result;
    }

    /// Test whether each component in this plane is equal to the corresponding component in another plane within a
    /// default threshold.
    ///
    /// @param[in] rPlane  Plane.
    ///
    /// @return  True if this plane and the given plane are equal or nearly equal, false if not.
    bool Plane::operator==( const Plane& rPlane ) const
    {
        return Equals( rPlane );
    }

    /// Test whether any component in this plane is not equal to the corresponding component in another plane within a
    /// default threshold.
    ///
    /// @param[in] rPlane  Plane.
    ///
    /// @return  True if this plane and the given plane are not equal, false if they are or are nearly equal.
    bool Plane::operator!=( const Plane& rPlane ) const
    {
        return !Equals( rPlane );
    }
}

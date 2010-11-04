//----------------------------------------------------------------------------------------------------------------------
// Sphere.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Constructor.
    ///
    /// This creates a sphere with all components uninitialized.  Values should be assigned before use.
    Sphere::Sphere()
    {
    }

    /// Constructor.
    ///
    /// This creates a sphere with the given center and radius.
    ///
    /// @param[in] rCenter  Sphere center.
    /// @param[in] radius   Sphere radius.
    Sphere::Sphere( const Vector3& rCenter, float32_t radius )
    {
        Set( rCenter, radius );
    }

    /// Constructor.
    ///
    /// This creates a sphere with the given center and radius.
    ///
    /// @param[in] centerX  X-coordinate of the sphere center.
    /// @param[in] centerY  Y-coordinate of the sphere center.
    /// @param[in] centerZ  Z-coordinate of the sphere center.
    /// @param[in] radius   Sphere radius.
    Sphere::Sphere( float32_t centerX, float32_t centerY, float32_t centerZ, float32_t radius )
    {
        Set( centerX, centerY, centerZ, radius );
    }

    /// Constructor.
    ///
    /// This creates a sphere based on the given vector, with the sphere center taken from the x, y, and z coordinates
    /// and the radius taken from the w coordinate.
    ///
    /// @param[in] rVector  Vector from which to initialize this sphere.
    Sphere::Sphere( const Vector4& rVector )
    {
        Set( rVector );
    }

    /// Constructor.
    ///
    /// This creates a sphere encompassing the given axis-aligned bounding box.
    ///
    /// @param[in] rBox  Axis-aligned bounding box.
    Sphere::Sphere( const AaBox& rBox )
    {
        Set( rBox );
    }

#if HELIUM_SIMD_SIZE == 16
    /// Constructor.
    ///
    /// This directly initializes the internal SIMD vector with the specified vector value.
    ///
    /// @param[in] rVector  SIMD vector from which to initialize.
    Sphere::Sphere( const Helium::SimdVector& rVector )
    {
        m_centerRadius = rVector;
    }

    /// Get the contents of this sphere as a SIMD vector.
    ///
    /// @return  Reference to the SIMD vector in which this sphere is stored.
    ///
    /// @see SetSimdVector()
    Helium::SimdVector& Sphere::GetSimdVector()
    {
        return m_centerRadius;
    }

    /// Get the contents of this sphere as a SIMD vector.
    ///
    /// @return  Constant reference to the SIMD vector in which this sphere is stored.
    ///
    /// @see SetSimdVector()
    const Helium::SimdVector& Sphere::GetSimdVector() const
    {
        return m_centerRadius;
    }

    /// Set the contents of this sphere to the given SIMD vector.
    ///
    /// @param[in] rVector  SIMD vector.
    ///
    /// @see GetSimdVector()
    void Sphere::SetSimdVector( const Helium::SimdVector& rVector )
    {
        m_centerRadius = rVector;
    }
#endif  // HELIUM_SIMD_SIZE == 16
}

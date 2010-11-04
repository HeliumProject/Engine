//----------------------------------------------------------------------------------------------------------------------
// Vector3SoaSse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if HELIUM_SIMD_SSE

namespace Lunar
{
    /// Splat each component of the given vector across each SIMD vector for each component in this vector set.
    ///
    /// @param[in] rVector  Vector from which to set this vector.
    void Vector3Soa::Splat( const Vector3& rVector )
    {
        Helium::SimdVector vectorVec = rVector.GetSimdVector();
        m_x = _mm_shuffle_ps( vectorVec, vectorVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
        m_y = _mm_shuffle_ps( vectorVec, vectorVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
        m_z = _mm_shuffle_ps( vectorVec, vectorVec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
    }
}

#endif  // HELIUM_SIMD_SSE

#if HELIUM_SIMD_SSE

/// Splat each component of the given plane across each SIMD vector for each component in this plane set.
///
/// @param[in] rPlane  Plane from which to set this plane.
void Helium::Simd::PlaneSoa::Splat( const Plane& rPlane )
{
    Register planeVec = rPlane.GetSimdVector();
    m_a = _mm_shuffle_ps( planeVec, planeVec, _MM_SHUFFLE( 0, 0, 0, 0 ) );
    m_b = _mm_shuffle_ps( planeVec, planeVec, _MM_SHUFFLE( 1, 1, 1, 1 ) );
    m_c = _mm_shuffle_ps( planeVec, planeVec, _MM_SHUFFLE( 2, 2, 2, 2 ) );
    m_d = _mm_shuffle_ps( planeVec, planeVec, _MM_SHUFFLE( 3, 3, 3, 3 ) );
}

#endif  // HELIUM_SIMD_SSE

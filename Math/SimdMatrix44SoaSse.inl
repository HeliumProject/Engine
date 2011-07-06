#if HELIUM_SIMD_SSE

/// Splat each component of the given matrix across each SIMD vector for each component in this matrix set.
///
/// @param[in] rMatrix  Matrix from which to set this matrix.
void Helium::Simd::Matrix44Soa::Splat( const Matrix44& rMatrix )
{
    Register rowVec;

#define SPLAT_ROW( N ) \
    rowVec = rMatrix.GetSimdVector( N ); \
    m_matrix[ N ][ 0 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 0, 0, 0, 0 ) ); \
    m_matrix[ N ][ 1 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 1, 1, 1, 1 ) ); \
    m_matrix[ N ][ 2 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 2, 2, 2, 2 ) ); \
    m_matrix[ N ][ 3 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 3, 3, 3, 3 ) );

    SPLAT_ROW( 0 );
    SPLAT_ROW( 1 );
    SPLAT_ROW( 2 );
    SPLAT_ROW( 3 );

#undef SPLAT_ROW
}

#endif  // HELIUM_SIMD_SSE

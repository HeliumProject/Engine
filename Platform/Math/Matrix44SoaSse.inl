//----------------------------------------------------------------------------------------------------------------------
// Matrix44SoaSse.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if HELIUM_SIMD_SSE

namespace Helium
{
    /// Splat each component of the given matrix across each SIMD vector for each component in this matrix set.
    ///
    /// @param[in] rMatrix  Matrix from which to set this matrix.
    void Matrix44Soa::Splat( const Matrix44& rMatrix )
    {
        SimdVector rowVec;

#define SPLAT_ROW( Z, N, DATA ) \
        rowVec = rMatrix.GetSimdVector( N ); \
        m_matrix[ N ][ 0 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 0, 0, 0, 0 ) ); \
        m_matrix[ N ][ 1 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 1, 1, 1, 1 ) ); \
        m_matrix[ N ][ 2 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 2, 2, 2, 2 ) ); \
        m_matrix[ N ][ 3 ] = _mm_shuffle_ps( rowVec, rowVec, _MM_SHUFFLE( 3, 3, 3, 3 ) );

        BOOST_PP_REPEAT( 4, SPLAT_ROW, );

#undef SPLAT_ROW
    }
}

#endif  // HELIUM_SIMD_SSE

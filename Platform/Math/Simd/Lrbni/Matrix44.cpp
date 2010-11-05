//#include "CorePch.h"
#include "Platform/Math/Simd.h"

#if HELIUM_SIMD_LRBNI

#include "Platform/Math/Simd/Matrix44.h"

namespace Helium
{
    namespace Simd
    {
        static HELIUM_FORCEINLINE void DeterminantHelper(
            const Register& rMatrix,
            Register& rDeterminant,
            Register& rDetSubmat33 )
        {
            Register nextRow = _mm512_shuf128x32( rMatrix, _MM_PERM_ADCB, _MM_PERM_DCBA );

            Register det22Adj = Simd::MultiplyF32( rMatrix, _mm512_shuf128x32( rMatrix, _MM_PERM_ADCB, _MM_PERM_ADCB ) );
            det22Adj = _mm512_msubr213_ps( _mm512_shuf128x32( rMatrix, _MM_PERM_DCBA, _MM_PERM_ADCB ), nextRow, det22Adj );

            Register det22Opp = Simd::MultiplyF32( rMatrix, _mm512_shuf128x32( rMatrix, _MM_PERM_ADCB, _MM_PERM_BADC ) );
            det22Opp = _mm512_msubr213_ps( _mm512_shuf128x32( rMatrix, _MM_PERM_DCBA, _MM_PERM_BADC ), nextRow, det22Opp );

            rDetSubmat33 = Simd::MultiplyF32(
                _mm512_shuf128x32( rMatrix, _MM_PERM_ADCB, _MM_PERM_ADCB ),
                _mm512_shuf128x32( det22Adj, _MM_PERM_BADC, _MM_PERM_BADC ) );
            rDetSubmat33 = _mm512_msubr213_ps(
                _mm512_shuf128x32( rMatrix, _MM_PERM_ADCB, _MM_PERM_BADC ),
                _mm512_shuf128x32( det22Opp, _MM_PERM_BADC, _MM_PERM_ADCB ),
                rDetSubmat33 );
            rDetSubmat33 = Simd::MultiplyAddF32(
                _mm512_shuf128x32( rMatrix, _MM_PERM_ADCB, _MM_PERM_CBAD ),
                _mm512_shuf128x32( det22Adj, _MM_PERM_BADC, _MM_PERM_ADCB ),
                rDetSubmat33 );

            Register determinantBase = Simd::MultiplyF32( rMatrix, rDetSubmat33 );

            rDeterminant = Simd::SubtractF32(
                determinantBase,
                _mm512_shuf128x32( determinantBase, _MM_PERM_DCBA, _MM_PERM_ADCB ) );
            rDeterminant = Simd::AddF32( rDeterminant, _mm512_shuf128x32( determinantBase, _MM_PERM_DCBA, _MM_PERM_BADC ) );
            rDeterminant = Simd::SubtractF32(
                rDeterminant,
                _mm512_shuf128x32( determinantBase, _MM_PERM_DCBA, _MM_PERM_CBAD ) );
        }

/// Set this matrix to the product of two matrices.
///
/// @param[in] rMatrix0  First matrix.
/// @param[in] rMatrix1  Second matrix.
void Helium::Simd::Matrix44::MultiplySet( const Matrix44& rMatrix0, const Matrix44& rMatrix1 )
{
    Register result = Simd::MultiplyF32(
        _mm512_shuf128x32( rMatrix0.m_matrix, _MM_PERM_DCBA, _MM_PERM_AAAA ),
        _mm512_shuf128x32( rMatrix1.m_matrix, _MM_PERM_AAAA, _MM_PERM_DCBA ) );
    result = Simd::MultiplyAddF32(
        _mm512_shuf128x32( rMatrix0.m_matrix, _MM_PERM_DCBA, _MM_PERM_BBBB ),
        _mm512_shuf128x32( rMatrix1.m_matrix, _MM_PERM_BBBB, _MM_PERM_DCBA ),
        result );
    result = Simd::MultiplyAddF32(
        _mm512_shuf128x32( rMatrix0.m_matrix, _MM_PERM_DCBA, _MM_PERM_CCCC ),
        _mm512_shuf128x32( rMatrix1.m_matrix, _MM_PERM_CCCC, _MM_PERM_DCBA ),
        result );
    result = Simd::MultiplyAddF32(
        _mm512_shuf128x32( rMatrix0.m_matrix, _MM_PERM_DCBA, _MM_PERM_DDDD ),
        _mm512_shuf128x32( rMatrix1.m_matrix, _MM_PERM_DDDD, _MM_PERM_DCBA ),
        result );

    m_matrix = result;
}

/// Compute the determinant of this matrix.
///
/// @return  Matrix determinant.
float32_t Helium::Simd::Matrix44::GetDeterminant() const
{
    Register determinant, detSubmat33;
    DeterminantHelper( m_matrix, determinant, detSubmat33 );

    return reinterpret_cast< const float32_t* >( &determinant )[ 0 ];
}

/// Get the inverse of this matrix.
///
/// @param[out] rMatrix  Matrix inverse.
///
/// @see Invert()
void Helium::Simd::Matrix44::GetInverse( Matrix44& rMatrix ) const
{
    Register invDeterminant, detSubmat33;
    DeterminantHelper( m_matrix, invDeterminant, detSubmat33 );

    invDeterminant = Simd::InverseF32( invDeterminant );

    rMatrix.m_matrix = Simd::MultiplyF32( detSubmat33, invDeterminant );

    rMatrix.Transpose();
}

/// Get the transpose of this matrix.
///
/// @param[out] rMatrix  Matrix transpose.
///
/// @see Transpose()
void Helium::Simd::Matrix44::GetTranspose( Matrix44& rMatrix ) const
{
    Mask transposeMask;

    transposeMask = _mm512_int2mask( 0x33cc );
    Register temp = _mm512_mask_shuf128x32( m_matrix, transposeMask, m_matrix, _MM_PERM_BADC, _MM_PERM_BADC );

    transposeMask = _mm512_int2mask( 0x5a5a );
    rMatrix.m_matrix = _mm512_mask_shuf128x32( temp, transposeMask, temp, _MM_PERM_CDAB, _MM_PERM_CDAB );
}

#endif  // HELIUM_SIMD_LRBNI

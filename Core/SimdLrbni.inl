//----------------------------------------------------------------------------------------------------------------------
// SimdLrbni.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#if L_SIMD_LRBNI

namespace Lunar
{
    // Type punning between prototype definitions of __m512 and __m512i.
    union Lrbni512Pun
    {
        __m512 f;
        __m512i i;
    };

    /// Load a SIMD vector from aligned memory.
    ///
    /// @param[in] pSource  Memory, aligned to L_SIMD_ALIGNMENT, from which to load.
    ///
    /// @return  SIMD vector.
    SimdVector Simd::LoadAligned( const void* pSource )
    {
        // LRBni prototype load functions are not const-correct, so we need to const cast here.
        return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_16X16, _MM_HINT_NONE );
    }

    /// Load a SIMD vector from unaligned memory.
    ///
    /// @param[in] pSource  Memory from which to load.
    ///
    /// @return  SIMD vector.
    SimdVector Simd::LoadUnaligned( const void* pSource )
    {
        // LRBni prototype load functions are not const-correct, so we need to const cast here.
        return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_16X16, _MM_HINT_NONE );
    }

    /// Store the contents of a SIMD vector in aligned memory.
    ///
    /// @param[out] pDest  Memory, aligned to L_SIMD_ALIGNMENT, in which to store the data.
    /// @param[in]  vec    SIMD vector to store.
    void Simd::StoreAligned( void* pDest, SimdVector vec )
    {
        _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_16, _MM_HINT_NONE );
    }

    /// Store the contents of a SIMD vector in unaligned memory.
    ///
    /// @param[out] pDest  Memory in which to store the data.
    /// @param[in]  vec    SIMD vector to store.
    void Simd::StoreUnaligned( void* pDest, SimdVector vec )
    {
        _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_16, _MM_HINT_NONE );
    }

    /// Load a 32-bit value into each component of a SIMD vector.
    ///
    /// @param[in] pSource  Address of the 32-bit value to load (must be aligned to a 4-byte boundary).
    ///
    /// @return  SIMD vector.
    ///
    /// @see Store32(), LoadSplat128()
    SimdVector Simd::LoadSplat32( const void* pSource )
    {
        // LRBni prototype load functions are not const-correct, so we need to const cast here.
        return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_1X16, _MM_HINT_NONE );
    }

    /// Load 16 bytes of data into a SIMD vector, repeating the data as necessary to fill.
    ///
    /// For platforms with only 16-byte SIMD vectors, this has the same effect as LoadAligned().
    ///
    /// @param[in] pSource  Address of the data to load (must be aligned to a 16-byte boundary).
    ///
    /// @return  SIMD vector.
    ///
    /// @see Store128(), LoadSplat32()
    SimdVector Simd::LoadSplat128( const void* pSource )
    {
        // LRBni prototype load functions are not const-correct, so we need to const cast here.
        return _mm512_loadd( const_cast< void* >( pSource ), _MM_FULLUPC_NONE, _MM_BROADCAST_4X16, _MM_HINT_NONE );
    }

    /// Store the first 32-bit value of a SIMD vector into memory.
    ///
    /// @param[in] pDest  Address in which to store the value (must be aligned to a 4-byte boundary).
    /// @param[in] vec    Vector containing the value to store.
    ///
    /// @see LoadSplat32(), Store128()
    void Simd::Store32( void* pDest, SimdVector vec )
    {
        _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_1, _MM_HINT_NONE );
    }

    /// Store 16 bytes of data from a SIMD vector into memory.
    ///
    /// For platforms with only 16-byte SIMD vectors, this has the same effect as StoreAligned().
    ///
    /// @param[in] pDest  Address in which to store the data (must be aligned to a 16-byte boundary).
    /// @param[in] vec    Vector containing the data to store.
    ///
    /// @see LoadSplat128(), Store32()
    void Simd::Store128( void* pDest, SimdVector vec )
    {
        _mm512_stored( pDest, vec, _MM_DOWNC_NONE, _MM_SUBSET32_4, _MM_HINT_NONE );
    }

    /// Load a vector containing all zeros.
    ///
    /// @return  Vector containing all zeros.
    SimdVector Simd::LoadZeros()
    {
        return _mm512_setzero_ps();
    }

    /// Perform a component-wise addition of two SIMD vectors of single-precision floating-point values.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector to add.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::AddF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_add_ps( vec0, vec1 );
    }

    /// Perform a component-wise subtraction of one SIMD vector of single-precision floating-point values from another.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector to subtract.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::SubtractF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_sub_ps( vec0, vec1 );
    }

    /// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector by which to multiply.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::MultiplyF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_mul_ps( vec0, vec1 );
    }

    /// Perform a component-wise division of one SIMD vector of single-precision floating-point values by another.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector by which to divide.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::DivideF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_div_ps( vec0, vec1 );
    }

    /// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values, and add
    /// the resulting value with those in a third vector.
    ///
    /// @param[in] vecMul0  SIMD vector.
    /// @param[in] vecMul1  SIMD vector by which to mulitiply.
    /// @param[in] vecAdd   SIMD vector to add to the result.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::MultiplyAddF32( SimdVector vecMul0, SimdVector vecMul1, SimdVector vecAdd )
    {
        return _mm512_madd213_ps( vecMul0, vecMul1, vecAdd );
    }

    /// Compute the square root of each component in a SIMD vector of single-precision floating-point values.
    ///
    /// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
    /// same as using the C-library sqrtf() function on each component.
    ///
    /// @param[in] vec  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::SqrtF32( SimdVector vec )
    {
        return _mm512_sqrt_ps( vec );
    }

    /// Compute the multiplicative inverse of each component in a SIMD vector of single-precision floating-point values.
    ///
    /// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
    /// same as actually computing the reciprocal of each component using scalar division.
    ///
    /// @param[in] vec  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::InverseF32( SimdVector vec )
    {
        return _mm512_recip_ps( vec );
    }

    /// Compute the multiplicative inverse of the square root of each component in a SIMD vector of single-precision
    /// floating-point values.
    ///
    /// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
    /// same as actually computing the reciprocal of the square root of each component using the C-library sqrtf()
    /// function and scalar division.
    ///
    /// @param[in] vec  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::InverseSqrtF32( SimdVector vec )
    {
        return _mm512_rsqrt_ps( vec );
    }

    /// Create a SIMD vector of single-precision floating-point values containing the minimum between each component in
    /// the two given SIMD vectors.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::MinF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_min_ps( vec0, vec1 );
    }

    /// Create a SIMD vector of single-precision floating-point values containing the maximum between each component in
    /// the two given SIMD vectors.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::MaxF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_max_ps( vec0, vec1 );
    }

    /// Compare each component in two SIMD vectors of single-precision floating-point values for equality, setting each
    /// component in the result mask based on the result of the comparison.
    ///
    /// If the corresponding components in the two given vectors are equal, the corresponding component in the result
    /// mask will be set, otherwise it will be cleared.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  Mask with the result of the operation.
    SimdMask Simd::EqualsF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_cmpeq_ps( vec0, vec1 );
    }

    /// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
    /// in the first vector is less than the corresponding component in the second, setting each component in the result
    /// mask based on the result of the comparison.
    ///
    /// If a component in the first vector is less than the corresponding component in the second vector, the
    /// corresponding component in the result mask will be set, otherwise it will be cleared.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  Mask with the result of the operation.
    SimdMask Simd::LessF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_cmplt_ps( vec0, vec1 );
    }

    /// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
    /// in the first vector is greater than the corresponding component in the second, setting each component in the
    /// result mask based on the result of the comparison.
    ///
    /// If a component in the first vector is greater than the corresponding component in the second vector, the
    /// corresponding component in the result mask will be set, otherwise it will be cleared.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  Mask with the result of the operation.
    SimdMask Simd::GreaterF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_cmple_ps( vec1, vec0 );
    }

    /// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
    /// in the first vector is less than or equal to the corresponding component in the second, setting each component
    /// in the result mask based on the result of the comparison.
    ///
    /// If a component in the first vector is less than or equal to the corresponding component in the second vector,
    /// the corresponding component in the result mask will be set, otherwise it will be cleared.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  Mask with the result of the operation.
    SimdMask Simd::LessEqualsF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_cmple_ps( vec0, vec1 );
    }

    /// Compare each component in two SIMD vectors of single-precision floating-point values for whether the component
    /// in the first vector is greater than or equal to the corresponding component in the second, setting each
    /// component in the result mask based on the result of the comparison.
    ///
    /// If a component in the first vector is greater than or equal to the corresponding component in the second vector,
    /// the corresponding component in the result mask will be set, otherwise it will be cleared.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  Mask with the result of the operation.
    SimdMask Simd::GreaterEqualsF32( SimdVector vec0, SimdVector vec1 )
    {
        return _mm512_cmplt_ps( vec1, vec0 );
    }

    /// Compute the bitwise-AND of two SIMD vectors.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::And( SimdVector vec0, SimdVector vec1 )
    {
        Lrbni512Pun vec0Pun, vec1Pun, resultPun;
        vec0Pun.f = vec0;
        vec1Pun.f = vec1;

        resultPun.i = _mm512_and_pi( vec0Pun.i, vec1Pun.i );

        return resultPun.f;
    }

    /// Compute the bitwise-AND of the one's complement (bitwise-NOT) of a vector with another vector.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation (that is, the bitwise-AND of the second vector and the
    ///          complement of the first vector).
    SimdVector Simd::AndNot( SimdVector vec0, SimdVector vec1 )
    {
        Lrbni512Pun vec0Pun, vec1Pun, resultPun;
        vec0Pun.f = vec0;
        vec1Pun.f = vec1;

        resultPun.i = _mm512_andn_pi( vec0Pun.i, vec1Pun.i );

        return resultPun.f;
    }

    /// Compute the bitwise-OR of two SIMD vectors.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::Or( SimdVector vec0, SimdVector vec1 )
    {
        Lrbni512Pun vec0Pun, vec1Pun, resultPun;
        vec0Pun.f = vec0;
        vec1Pun.f = vec1;

        resultPun.i = _mm512_or_pi( vec0Pun.i, vec1Pun.i );

        return resultPun.f;
    }

    /// Compute the bitwise-XOR of two SIMD vectors.
    ///
    /// @param[in] vec0  SIMD vector.
    /// @param[in] vec1  SIMD vector.
    ///
    /// @return  SIMD vector with the result of the operation.
    SimdVector Simd::Xor( SimdVector vec0, SimdVector vec1 )
    {
        Lrbni512Pun vec0Pun, vec1Pun, resultPun;
        vec0Pun.f = vec0;
        vec1Pun.f = vec1;

        resultPun.i = _mm512_xor_pi( vec0Pun.i, vec1Pun.i );

        return resultPun.f;
    }
}

#endif  // L_SIMD_LRBNI

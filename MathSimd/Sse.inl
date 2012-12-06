#if HELIUM_SIMD_SSE

/// Load a SIMD vector from aligned memory.
///
/// @param[in] pSource  Memory, aligned to HELIUM_SIMD_ALIGNMENT, from which to load.
///
/// @return  SIMD vector.
Helium::Simd::Register Helium::Simd::LoadAligned( const void* pSource )
{
    return _mm_load_ps( static_cast< const float32_t* >( pSource ) );
}

/// Load a SIMD vector from unaligned memory.
///
/// @param[in] pSource  Memory from which to load.
///
/// @return  SIMD vector.
Helium::Simd::Register Helium::Simd::LoadUnaligned( const void* pSource )
{
    return _mm_loadu_ps( static_cast< const float32_t* >( pSource ) );
}

/// Store the contents of a SIMD vector in aligned memory.
///
/// @param[out] pDest  Memory, aligned to HELIUM_SIMD_ALIGNMENT, in which to store the data.
/// @param[in]  vec    SIMD vector to store.
void Helium::Simd::StoreAligned( void* pDest, Helium::Simd::Register vec )
{
    _mm_store_ps( static_cast< float32_t* >( pDest ), vec );
}

/// Store the contents of a SIMD vector in unaligned memory.
///
/// @param[out] pDest  Memory in which to store the data.
/// @param[in]  vec    SIMD vector to store.
void Helium::Simd::StoreUnaligned( void* pDest, Helium::Simd::Register vec )
{
    _mm_storeu_ps( static_cast< float32_t* >( pDest ), vec );
}

/// Load a 32-bit value into each component of a SIMD vector.
///
/// @param[in] pSource  Address of the 32-bit value to load (must be aligned to a 4-byte boundary).
///
/// @return  SIMD vector.
///
/// @see Store32(), LoadSplat128()
Helium::Simd::Register Helium::Simd::LoadSplat32( const void* pSource )
{
    return _mm_load_ps1( static_cast< const float32_t* >( pSource ) );
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
Helium::Simd::Register Helium::Simd::LoadSplat128( const void* pSource )
{
    return _mm_load_ps( static_cast< const float32_t* >( pSource ) );
}

/// Store the first 32-bit value of a SIMD vector into memory.
///
/// @param[in] pDest  Address in which to store the value (must be aligned to a 4-byte boundary).
/// @param[in] vec    Vector containing the value to store.
///
/// @see LoadSplat32(), Store128()
void Helium::Simd::Store32( void* pDest, Helium::Simd::Register vec )
{
    _mm_store_ss( static_cast< float32_t* >( pDest ), vec );
}

/// Store 16 bytes of data from a SIMD vector into memory.
///
/// For platforms with only 16-byte SIMD vectors, this has the same effect as StoreAligned().
///
/// @param[in] pDest  Address in which to store the data (must be aligned to a 16-byte boundary).
/// @param[in] vec    Vector containing the data to store.
///
/// @see LoadSplat128(), Store32()
void Helium::Simd::Store128( void* pDest, Helium::Simd::Register vec )
{
    _mm_store_ps( static_cast< float32_t* >( pDest ), vec );
}

/// Fill a SIMD vector with a single-precision floating-point value splat across all vector components.
///
/// @param[in] value  Value to splat.
///
/// @return  SIMD vector containing the splat value.
Helium::Simd::Register Helium::Simd::SetSplatF32( float32_t value )
{
    return _mm_set_ps1( value );
}

/// Fill a SIMD vector with a 32-bit unsigned integer value splat across all vector components.
///
/// @param[in] value  Value to splat.
///
/// @return  SIMD vector containing the splat value.
Helium::Simd::Register Helium::Simd::SetSplatU32( uint32_t value )
{
    // Load the value using SSE1 instructions for wider support.
    return LoadSplat32( &value );
}

/// Load a vector containing all zeros.
///
/// @return  Vector containing all zeros.
Helium::Simd::Register Helium::Simd::LoadZeros()
{
    return _mm_setzero_ps();
}

/// Select components from one of two vectors based on the given mask.
///
/// If a given bit in the select mask is unset, the corresponding element of the first vector will be passed
/// through, otherwise the corresponding element of the second vector will be passed through.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
/// @param[in] mask  Selection mask.
Helium::Simd::Register Helium::Simd::Select( Helium::Simd::Register vec0, Helium::Simd::Register vec1, Helium::Simd::Mask mask )
{
    return _mm_or_ps( _mm_andnot_ps( mask, vec0 ), _mm_and_ps( mask, vec1 ) );
}

/// Perform a component-wise addition of two SIMD vectors of single-precision floating-point values.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector to add.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::AddF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_add_ps( vec0, vec1 );
}

/// Perform a component-wise subtraction of one SIMD vector of single-precision floating-point values from another.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector to subtract.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::SubtractF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_sub_ps( vec0, vec1 );
}

/// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector by which to multiply.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MultiplyF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_mul_ps( vec0, vec1 );
}

/// Perform a component-wise division of one SIMD vector of single-precision floating-point values by another.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector by which to divide.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::DivideF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_div_ps( vec0, vec1 );
}

/// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values, and add
/// the resulting values with those in a third vector.
///
/// The result is computed with the following formula:
/// vecMul0 * vecMul1 + vecAdd
///
/// @param[in] vecMul0  SIMD vector.
/// @param[in] vecMul1  SIMD vector by which to multiply.
/// @param[in] vecAdd   SIMD vector to add to the result.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MultiplyAddF32(
    Helium::Simd::Register vecMul0,
    Helium::Simd::Register vecMul1,
    Helium::Simd::Register vecAdd )
{
    return _mm_add_ps( _mm_mul_ps( vecMul0, vecMul1 ), vecAdd );
}

/// Perform a component-wise multiplication of two SIMD vectors of single-precision floating-point values, and
/// subtract the resulting values from those in a third vector.
///
/// The result is computed with the following formula:
/// vecSub - vecMul0 * vecMul1
///
/// @param[in] vecMul0  SIMD vector.
/// @param[in] vecMul1  SIMD vector by which to multiply.
/// @param[in] vecAdd   SIMD vector from which to subtract the result.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MultiplySubtractReverseF32(
    Helium::Simd::Register vecMul0,
    Helium::Simd::Register vecMul1,
    Helium::Simd::Register vecSub )
{
    return _mm_sub_ps( vecSub, _mm_mul_ps( vecMul0, vecMul1 ) );
}

/// Compute the square root of each component in a SIMD vector of single-precision floating-point values.
///
/// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
/// same as using the C-library sqrtf() function on each component.
///
/// @param[in] vec  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::SqrtF32( Helium::Simd::Register vec )
{
    return _mm_sqrt_ps( vec );
}

/// Compute the multiplicative inverse of each component in a SIMD vector of single-precision floating-point values.
///
/// Note that this may be only an approximation on certain platforms, so its precision is not guaranteed to be the
/// same as actually computing the reciprocal of each component using scalar division.
///
/// @param[in] vec  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::InverseF32( Helium::Simd::Register vec )
{
    return _mm_rcp_ps( vec );
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
Helium::Simd::Register Helium::Simd::InverseSqrtF32( Helium::Simd::Register vec )
{
    return _mm_rsqrt_ps( vec );
}

/// Create a SIMD vector of single-precision floating-point values containing the minimum between each component in
/// the two given SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MinF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_min_ps( vec0, vec1 );
}

/// Create a SIMD vector of single-precision floating-point values containing the maximum between each component in
/// the two given SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::MaxF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_max_ps( vec0, vec1 );
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
Helium::Simd::Mask Helium::Simd::EqualsF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_cmpeq_ps( vec0, vec1 );
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
Helium::Simd::Mask Helium::Simd::LessF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_cmplt_ps( vec0, vec1 );
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
Helium::Simd::Mask Helium::Simd::GreaterF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_cmpgt_ps( vec0, vec1 );
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
Helium::Simd::Mask Helium::Simd::LessEqualsF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_cmple_ps( vec0, vec1 );
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
Helium::Simd::Mask Helium::Simd::GreaterEqualsF32( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_cmpge_ps( vec0, vec1 );
}

/// Compute the bitwise-AND of two SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::And( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_and_ps( vec0, vec1 );
}

/// Compute the bitwise-AND of the one's complement (bitwise-NOT) of a vector with another vector.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation (that is, the bitwise-AND of the second vector and the
///          complement of the first vector).
Helium::Simd::Register Helium::Simd::AndNot( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_andnot_ps( vec0, vec1 );
}

/// Compute the bitwise-OR of two SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::Or( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_or_ps( vec0, vec1 );
}

/// Compute the bitwise-XOR of two SIMD vectors.
///
/// @param[in] vec0  SIMD vector.
/// @param[in] vec1  SIMD vector.
///
/// @return  SIMD vector with the result of the operation.
Helium::Simd::Register Helium::Simd::Xor( Helium::Simd::Register vec0, Helium::Simd::Register vec1 )
{
    return _mm_xor_ps( vec0, vec1 );
}

/// Compute the bitwise-AND of two SIMD masks.
///
/// @param[in] mask0  SIMD mask.
/// @param[in] mask1  SIMD mask.
///
/// @return  SIMD mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::MaskAnd( Helium::Simd::Mask mask0, Helium::Simd::Mask mask1 )
{
    return _mm_and_ps( mask0, mask1 );
}

/// Compute the bitwise-AND of the one's complement (bitwise-NOT) of a mask with another mask.
///
/// @param[in] mask0  SIMD mask.
/// @param[in] mask1  SIMD mask.
///
/// @return  SIMD mask with the result of the operation (that is, the bitwise-AND of the second mask and the
///          complement of the first mask).
Helium::Simd::Mask Helium::Simd::MaskAndNot( Helium::Simd::Mask mask0, Helium::Simd::Mask mask1 )
{
    return _mm_andnot_ps( mask0, mask1 );
}

/// Compute the bitwise-OR of two SIMD masks.
///
/// @param[in] mask0  SIMD mask.
/// @param[in] mask1  SIMD mask.
///
/// @return  SIMD mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::MaskOr( Helium::Simd::Mask mask0, Helium::Simd::Mask mask1 )
{
    return _mm_or_ps( mask0, mask1 );
}

/// Compute the bitwise-XOR of two SIMD masks.
///
/// @param[in] mask0  SIMD mask.
/// @param[in] mask1  SIMD mask.
///
/// @return  SIMD mask with the result of the operation.
Helium::Simd::Mask Helium::Simd::MaskXor( Helium::Simd::Mask mask0, Helium::Simd::Mask mask1 )
{
    return _mm_xor_ps( mask0, mask1 );
}

#endif  // HELIUM_SIMD_SSE

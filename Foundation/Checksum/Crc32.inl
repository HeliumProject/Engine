//----------------------------------------------------------------------------------------------------------------------
// Crc32.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

/// Calculate a CRC-32 value for a block of data.
///
/// @param[in] pData      Data for which to compute the CRC.
/// @param[in] byteCount  Size of the data block, in bytes.
///
/// @return  CRC-32 hash value for the block of data.
///
/// @see BeginCrc32(), UpdateCrc32(), FinishCrc32()
uint32_t Helium::Crc32( const void* pData, size_t byteCount )
{
    uint32_t crc = BeginCrc32();
    crc = UpdateCrc32( crc, pData, byteCount );
    crc = FinishCrc32( crc );

    return crc;
}

/// Calculate a CRC-32 value for a string.
///
/// @param[in] pString    String for which to compute the CRC.
///
/// @return  CRC-32 hash value for the block of data.
///
/// @see BeginCrc32(), UpdateCrc32(), FinishCrc32()
uint32_t Helium::Crc32( const tchar_t* pString )
{
    uint32_t crc = BeginCrc32();
    crc = UpdateCrc32( crc, pString, StringLength( pString ) * sizeof( tchar_t ) );
    crc = FinishCrc32( crc );

    return crc;
}

/// Begin incremental calculation of a CRC-32 value.
///
/// This will provide an initial value that can be passed to UpdateCrc32() and FinishCrc32() for incrementally
/// computing a CRC-32 value based on multiple consecutive chunks of data.  After calling BeginCrc32(),
/// UpdateCrc32() can then be called on each chunk of data, passing in the value returned from the previous call to
/// UpdateCrc32() (or, for the first call to UpdateCrc32(), passing in the value returned by BeginCrc32()).  Once
/// all data has been processed, the running hash can then be passed to FinishCrc32() to compute the final CRC-32
/// value for the data.
///
/// @return  Running hash value to pass to UpdateCrc32() and FinishCrc32().
///
/// @see UpdateCrc32(), FinishCrc32(), Crc32()
uint32_t Helium::BeginCrc32()
{
    return 0xffffffff;
}

/// Finish incremental calculation of a CRC-32 value.
///
/// See the documentation for BeginCrc32() for more information on incremental CRC-32 calculations.
///
/// @param[in] runningCrc  Running hash returned by the most recent call to UpdateCrc32().
///
/// @return  Final CRC-32 value.
///
/// @see BeginCrc32(), UpdateCrc32(), Crc32()
uint32_t Helium::FinishCrc32( uint32_t runningCrc )
{
    return ~runningCrc;
}

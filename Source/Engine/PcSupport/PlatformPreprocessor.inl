namespace Helium
{
    /// Get whether byte swapping needs to be performed when converting between the current platform and the target
    /// platform of this preprocessor.
    ///
    /// @return  True if the current platform byte order and the byte order of this preprocessor's platform do not
    ///          match, false if they do match.
    bool PlatformPreprocessor::SwapBytes() const
    {
#if HELIUM_ENDIAN_LITTLE
        return ( GetByteOrder() != BYTE_ORDER_LITTLE );
#else
        return ( GetByteOrder() != BYTE_ORDER_BIG );
#endif
    }

    /// Constructor.
    PlatformPreprocessor::ShaderToken::ShaderToken()
    {
    }

    /// Constructor.
    ///
    /// @param[in] rName        Preprocessor token name string.
    /// @param[in] rDefinition  Definition string to which the token should resolve.
    PlatformPreprocessor::ShaderToken::ShaderToken( const CharString& rName, const CharString& rDefinition )
        : name( rName )
        , definition( rDefinition )
    {
    }
}

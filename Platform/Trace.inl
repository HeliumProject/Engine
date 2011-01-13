#if HELIUM_ENABLE_TRACE

/// Get the current logging level.
///
/// @return  Current logging level.
///
/// @see SetLevel()
Helium::ETraceLevel Helium::Trace::GetLevel() const
{
    return m_level;
}

#endif  // HELIUM_ENABLE_TRACE

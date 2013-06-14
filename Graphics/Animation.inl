namespace Helium
{
#if HELIUM_USE_GRANNY_ANIMATION
    /// Get the Granny-specific animation data.
    ///
    /// @return  Granny animation data.
    const Granny::AnimationData& Animation::GetGrannyData() const
    {
        return m_grannyData;
    }
#endif  // HELIUM_USE_GRANNY_ANIMATION
}

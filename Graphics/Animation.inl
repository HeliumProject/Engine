//----------------------------------------------------------------------------------------------------------------------
// Animation.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
#if L_USE_GRANNY_ANIMATION
    /// Get the Granny-specific animation data.
    ///
    /// @return  Granny animation data.
    const Granny::AnimationData& Animation::GetGrannyData() const
    {
        return m_grannyData;
    }
#endif  // L_USE_GRANNY_ANIMATION
}

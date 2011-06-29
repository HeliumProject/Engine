//----------------------------------------------------------------------------------------------------------------------
// SkeletalMeshEntity.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the animation resource assigned to this entity.
    ///
    /// @return  Assigned animation resource.
    ///
    /// @see SetAnimation()
    Animation* SkeletalMeshEntity::GetAnimation() const
    {
        VerifySafety();

        return m_spAnimation;
    }

#if L_USE_GRANNY_ANIMATION
    /// Get the Granny-specific data for this entity.
    ///
    /// @return  Granny-specific data.
    const Granny::SkeletalMeshEntityData& SkeletalMeshEntity::GetGrannyData() const
    {
        return m_grannyData;
    }
#endif  // L_USE_GRANNY_ANIMATION
}

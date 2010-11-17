//----------------------------------------------------------------------------------------------------------------------
// Camera.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Lunar
{
    /// Get the horizontal field-of-view angle.
    ///
    /// @return  Field-of-view angle, in degrees.
    ///
    /// @see SetFov()
    float32_t Camera::GetFov() const
    {
        VerifySafety();
        return m_fov;
    }
}

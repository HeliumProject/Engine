//----------------------------------------------------------------------------------------------------------------------
// Camera.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the ID of the scene view updated by this camera.
    ///
    /// @return  ID of the scene view associated with this camera.
    uint32_t Camera::GetSceneViewId() const
    {
        VerifySafety();
        return m_sceneViewId;
    }

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

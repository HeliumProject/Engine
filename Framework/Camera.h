//----------------------------------------------------------------------------------------------------------------------
// Camera.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_CAMERA_H
#define HELIUM_FRAMEWORK_CAMERA_H

#include "Framework/Entity.h"
namespace Helium
{
    /// Camera entity.
    class HELIUM_FRAMEWORK_API Camera : public Entity
    {
        HELIUM_DECLARE_OBJECT( Camera, Entity );

        /// Scene view ID.
        uint32_t m_sceneViewId;
        /// Horizontal field-of-view angle, in degrees.
        float32_t m_fov;
    };

    typedef Helium::StrongPtr< Camera > CameraPtr;
    typedef Helium::StrongPtr< const Camera > ConstCameraPtr;
}

#include "Framework/Camera.inl"

#endif  // HELIUM_FRAMEWORK_CAMERA_H

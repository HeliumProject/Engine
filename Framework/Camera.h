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

    public:
        /// Default field-of-view angle.
        static const float32_t DEFAULT_FOV;

        /// @name Construction/Destruction
        //@{
        Camera();
        virtual ~Camera();
        //@}

        /// @name Serialization
        //@{
        virtual void Serialize( Serializer& s );
        //@}

        /// @name Entity Registration
        //@{
        virtual void Attach();
        //@}

        /// @name Transform Data
        //@{
        virtual void SetPosition( const Simd::Vector3& rPosition );
        virtual void SetRotation( const Simd::Quat& rRotation );
        //@}

        /// @name Camera Parameters
        //@{
        inline uint32_t GetSceneViewId() const;
        void SetSceneViewId( uint32_t sceneViewId );

        inline float32_t GetFov() const;
        void SetFov( float32_t fov );
        //@}

    private:
        /// Scene view ID.
        uint32_t m_sceneViewId;
        /// Horizontal field-of-view angle, in degrees.
        float32_t m_fov;
    };
}

#include "Framework/Camera.inl"

#endif  // HELIUM_FRAMEWORK_CAMERA_H

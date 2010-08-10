#pragma once

#include "Core/API.h"
#include "Core/Content/Constants.h"

#include "Foundation/Reflect/Serializers.h"
#include "Foundation/Math/Constants.h"

#include <hash_map>

namespace Helium
{
    namespace Content
    {
        static const f32 TRANSLATION_DETECTION_TOLERANCE = 0.0001f;
        static const f32 SCALE_DETECTION_TOLERANCE       = 0.01f;
        static const f32 ROTATION_DETECTION_TOLERANCE    = 0.001f;

        struct AnimationCompressionControl
        {
            bool m_UseHypercubeCompression;
            u32 m_TranslateScale;
            u32 m_ScaleScale;
            u32 m_RotationScale;

            AnimationCompressionControl()
                : m_UseHypercubeCompression( true )
                , m_TranslateScale( Helium::AnimationSettings::Compression::TRANSLATION_SCALER_LOG )
                , m_ScaleScale( Helium::AnimationSettings::Compression::SCALE_SCALER_LOG )
                , m_RotationScale( Helium::AnimationSettings::Compression::ROTATION_SCALER_LOG )
            {
            }
        };

        class JointAnimation;
        typedef Helium::SmartPtr< JointAnimation > JointAnimationPtr;
        typedef std::vector< JointAnimationPtr > V_JointAnimation;
        typedef std::map< Helium::TUID, JointAnimationPtr > M_JointAnimation;

        class CORE_API JointAnimation : public Reflect::Element
        {
        public:

            Helium::TUID    m_JointID;
            f32                m_BlendFactor;

            // number of samples specified by the animator
            u32                m_WindowSamples;
            // number of total samples -- the exporter can add on data for extra processing in the builder
            u32                m_TotalSamples;

            // The values in the animation
            Math::V_Vector3    m_Translate;
            Math::V_Quaternion m_Rotate;
            Math::V_Vector3    m_Scale;

            JointAnimation()
                : m_BlendFactor( -1.0f )
                , m_WindowSamples( 0 )
                , m_TotalSamples( 0 )
            {
            }

            REFLECT_DECLARE_CLASS(JointAnimation, Reflect::Element);

            static void EnumerateClass( Reflect::Compositor<JointAnimation>& comp );

            // The "Window" samples are the samples specified explicitly by the animator
            // The exporter can add on extra samples for extra processing
            u32  WindowSamples() const;
            u32  TotalSamples() const;

            bool HasMotion( const f32 translationTolerance = TRANSLATION_DETECTION_TOLERANCE, const f32 scaleTolerance = SCALE_DETECTION_TOLERANCE, const f32 rotationTolerance = ROTATION_DETECTION_TOLERANCE ) const;
            bool HasTranslation( const f32 translationTolerance = TRANSLATION_DETECTION_TOLERANCE ) const;
            bool HasScaling( const f32 scaleTolerance = SCALE_DETECTION_TOLERANCE ) const;
            bool HasRotation( const f32 rotationTolerance = ROTATION_DETECTION_TOLERANCE ) const;

            void SetAllTSRSamples( const Math::Vector3& translate = Math::Vector3::Zero, const Math::Vector3& scale = Math::Vector3::Unit, const Math::Quaternion& rotate = Math::Quaternion::Identity );
            void ApplyTranslateAdjustment( const Math::Vector3& adjustment, u32 frame );
            void ApplyParentTransform( const Content::JointAnimationPtr& parentAnimation, bool applyScale = true );
            void RemoveLinearMotion( const Math::Axis& axis, f32& linearMotionPerFrame );
            void SmoothRotations();
            void ApplyInverseParentTransform( const Content::JointAnimationPtr& parentAnimation );
            f32 GetLinearDistancePerFrame();

        };
    }
}
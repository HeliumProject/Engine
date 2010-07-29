#include "JointAnimation.h"

using namespace Reflect;

using namespace Content;

REFLECT_DEFINE_CLASS(JointAnimation);

void JointAnimation::EnumerateClass( Reflect::Compositor<JointAnimation>& comp )
{
    Reflect::Field* fieldJointID = comp.AddField( &JointAnimation::m_JointID, "m_JointID" );
    Reflect::Field* fieldBlendFactor = comp.AddField( &JointAnimation::m_BlendFactor, "m_BlendFactor" );
    Reflect::Field* fieldWindowSamples = comp.AddField( &JointAnimation::m_WindowSamples, "m_WindowSamples" );
    Reflect::Field* fieldTotalSamples = comp.AddField( &JointAnimation::m_TotalSamples, "m_TotalSamples" );

    Reflect::Field* fieldTranslate = comp.AddField( &JointAnimation::m_Translate, "m_Translate" );
    Reflect::Field* fieldRotate = comp.AddField( &JointAnimation::m_Rotate, "m_Rotate" );
    Reflect::Field* fieldScale = comp.AddField( &JointAnimation::m_Scale, "m_Scale" );
}


u32 JointAnimation::WindowSamples() const
{
    // support legacy data
    if ( m_WindowSamples == 0 )
    {
        return (u32)m_Translate.size();
    }

    return m_WindowSamples;
}

u32 JointAnimation::TotalSamples() const
{
    // support legacy data
    if ( m_TotalSamples == 0 )
    {
        return (u32)m_Translate.size();
    }

    return m_TotalSamples;
}

bool JointAnimation::HasMotion( const f32 translationTolerance, const f32 scaleTolerance, const f32 rotationTolerance ) const
{
    // hopefully short-circuiting works with this evaluation
    return HasTranslation( translationTolerance ) || HasScaling( scaleTolerance ) || HasRotation( rotationTolerance );
}

bool JointAnimation::HasTranslation( const f32 translationTolerance ) const
{
    const Math::Vector3& initialTranslation = m_Translate.front();

    for each ( const Math::Vector3& translation in m_Translate )
    {
        if ( !initialTranslation.Equal( translation, translationTolerance ) )
            return true;
    }

    return false;
}

bool JointAnimation::HasScaling( const f32 scaleTolerance ) const
{
    const Math::Vector3& initialScale       = m_Scale.front();

    for each ( const Math::Vector3& scale in m_Scale )
    {
        if ( !initialScale.Equal( scale, scaleTolerance ) )
            return true;
    }

    return false;
}

bool JointAnimation::HasRotation( const f32 rotationTolerance ) const
{
    const Math::Quaternion& initialRotation = m_Rotate.front();

    for each ( const Math::Quaternion& rotation in m_Rotate )
    {
        if ( !initialRotation.Equal( rotation, rotationTolerance ) )
            return true;
    }

    return false;
}

void JointAnimation::SetAllTSRSamples( const Math::Vector3& translate, const Math::Vector3& scale, const Math::Quaternion& rotate )
{
    u32 numSamples = TotalSamples();
    for ( u32 frameIndex = 0; frameIndex < numSamples; ++frameIndex )
    {
        m_Translate[ frameIndex ] = translate;
        m_Scale[ frameIndex ] = scale;
        m_Rotate[ frameIndex ] = rotate;
    }
}

void JointAnimation::ApplyTranslateAdjustment( const Math::Vector3& adjustment, u32 frame )
{
    u32 totalSamples = TotalSamples();
    HELIUM_ASSERT( frame < totalSamples );
    m_Translate[ frame ] += adjustment;
}

void JointAnimation::ApplyParentTransform( const Content::JointAnimationPtr& parentAnimation, bool applyScale )
{
    u32 totalSamples = TotalSamples();
    for ( u32 frameIndex = 0; frameIndex < totalSamples; ++frameIndex )
    {
        Math::Quaternion rotateResult = parentAnimation->m_Rotate[ frameIndex ] * m_Rotate[ frameIndex ];
        Math::Matrix4 parentRotationMatrix( parentAnimation->m_Rotate[ frameIndex ] );
        Math::Vector4 scaleVector( parentAnimation->m_Scale[ frameIndex ] );
        scaleVector.w = 1.0f;

        parentRotationMatrix.x *= scaleVector.x;
        parentRotationMatrix.y *= scaleVector.y;
        parentRotationMatrix.z *= scaleVector.z;
        parentRotationMatrix.t *= scaleVector.w;

        Math::Vector3 targetTranslate( m_Translate[ frameIndex ] );

        Math::Vector4 resultTranslateVector( parentRotationMatrix * Math::Vector4( targetTranslate ) );

        m_Translate[ frameIndex ].x = resultTranslateVector.x + parentAnimation->m_Translate[ frameIndex ].x;
        m_Translate[ frameIndex ].y = resultTranslateVector.y + parentAnimation->m_Translate[ frameIndex ].y;
        m_Translate[ frameIndex ].z = resultTranslateVector.z + parentAnimation->m_Translate[ frameIndex ].z;

        m_Rotate[ frameIndex ] = rotateResult;

        if ( applyScale )
        {
            m_Scale[ frameIndex ].x *= parentAnimation->m_Scale[ frameIndex ].x;
            m_Scale[ frameIndex ].y *= parentAnimation->m_Scale[ frameIndex ].y;
            m_Scale[ frameIndex ].z *= parentAnimation->m_Scale[ frameIndex ].z;
        }
    }
}

void JointAnimation::ApplyInverseParentTransform( const Content::JointAnimationPtr& parentAnimation )
{
    u32 totalSamples = TotalSamples();
    for ( u32 frameIndex = 0; frameIndex < totalSamples; ++frameIndex )
    {
        // Create the parent's transformation matrix
        Math::Matrix4 parentRotation( parentAnimation->m_Rotate[frameIndex] );
        Math::Matrix4 parentTranslation( parentAnimation->m_Translate[frameIndex] );
        Math::Scale s( parentAnimation->m_Scale[frameIndex] );
        Math::Matrix4 parentScale( s );
        Math::Matrix4 parentTransform( ( parentScale * parentRotation ) * parentTranslation );

        // Create this joint's transformation matrix
        Math::Matrix4 rotation( m_Rotate[frameIndex] );
        Math::Matrix4 translation( m_Translate[frameIndex] );
        s = m_Scale[frameIndex];
        Math::Matrix4 scale( s );
        Math::Matrix4 transform( ( scale * rotation ) * translation );

        // multiply to get the new transformation matrix
        Math::Matrix4 newTransform = transform * parentTransform.Inverted();

        Math::Scale newScale;
        Math::Vector3 newTranslate;
        Math::Matrix3 newRotate;
        newTransform.Decompose( newScale, newRotate, newTranslate );

        m_Scale[ frameIndex ].x = newScale.x;
        m_Scale[ frameIndex ].y = newScale.y;
        m_Scale[ frameIndex ].z = newScale.z;
        m_Translate[ frameIndex ] = newTranslate;
        m_Rotate[ frameIndex ] = newRotate;

        // because the engine is going to LERP/SLERP the rotations, we need to make sure that
        // we avoid situations that would potentially cause them to 'flip' or rotate in a
        // counter-intuitive direction
        if ( frameIndex > 0 && m_Rotate[ frameIndex ].Dot( m_Rotate[ frameIndex - 1 ] ) < 0.0f )
            m_Rotate[ frameIndex ] = -m_Rotate[ frameIndex ];
    }
}

f32 JointAnimation::GetLinearDistancePerFrame()
{
    Math::Vector3 vec = m_Translate.back() - m_Translate.front(); 

    f32 distance = vec.Length();
    f32 distancePerFrame = distance / (WindowSamples() - 1);

    return distancePerFrame;
}
void JointAnimation::SmoothRotations()
{
    // because the engine is going to LERP/SLERP the rotations, we need to make sure that
    // we avoid situations that would potentially cause them to 'flip' or rotate in a
    // counter-intuitive direction, we don't do this check in the runtime, instead we just
    // export pre-baked data here, avoiding having to do it in the builder every time
    // we build

    u32 totalSamples = TotalSamples();
    for ( u32 i = 1; i < totalSamples; ++i )
    {
        if ( ( m_Rotate.size() > 0 ) && ( m_Rotate[ i  ].Dot( m_Rotate[ i - 1 ] ) < 0.0f ) )
            m_Rotate[ i ] = -m_Rotate[ i ];
    }
}

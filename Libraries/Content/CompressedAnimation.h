#pragma once

#include "API.h"
#include "Reflect/Serializers.h"
#include <hash_map>

#include "Animation.h"
#include "Content/Constants.h"

// i am sorry, this is a fast first pass at translating this from code
// that was worked on by many people under pressure, this is pretty
// rough, but I am hoping it's at least somewhat cleaner and more
// contained than it was before. :(

namespace Content
{
  
  struct AnimationCompressionControl;

  class CONTENT_API CompressedAnimation : public Reflect::Element
  {
  public:
    
    CompressedAnimation();
    CompressedAnimation( const AnimationCompressionControl& compressionControl, bool looping = false );

    bool m_Looping;

    bool m_UseHypercubeCompression;
    u32  m_TranslateScale;
    u32  m_ScaleScale;
    u32  m_RotateScale;

    f32  m_BlendFactor;

    // The values in the animation
    V_i16 m_TranslateX;
    V_i16 m_TranslateY;
    V_i16 m_TranslateZ;

    V_i16 m_RotateX;
    V_i16 m_RotateY;
    V_i16 m_RotateZ;
    V_i16 m_RotateW;

    V_i16 m_ScaleX;
    V_i16 m_ScaleY;
    V_i16 m_ScaleZ;

    V_i16 m_BindTranslate;  // 3 elements: x, y, z
    V_i16 m_MinTranslate;
    V_i16 m_MaxTranslate;
    V_i16 m_AvgTranslate;
    V_u8  m_DiffTypeTranslate;

    V_i16 m_MinScale;
    V_i16 m_MaxScale;
    V_i16 m_AvgScale;
    V_u8  m_DiffTypeScale;

    V_i16 m_MinRotate;
    V_i16 m_MaxRotate;
    V_i16 m_AvgRotate;
    V_u8  m_DiffTypeRotate;

    Nocturnal::UID::TUID m_JointID;

    REFLECT_DECLARE_CLASS( CompressedAnimation, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<CompressedAnimation>& comp );

    bool m_ExceededRangeOnBindConversion;
    bool m_ExceededRangeOnTranslationConversion;
    bool m_ExceededRangeOnScaleConversion;
    bool m_ExceededRangeOnRotationConversion;

    void Compress( const std::string& jt_name, const AnimationPtr& uncompressedAnimation, const Math::Vector3& bindTranslate );

    u32 NumSamples() const;
    bool HasMotion() const { return m_HasMotion; }

    inline bool ExceededRangeOnTranslationConversion() { return m_ExceededRangeOnTranslationConversion; }
    inline bool ExceededRangeOnScaleConversion() { return m_ExceededRangeOnScaleConversion; }
    inline bool ExceededRangeOnRotationConversion() { return m_ExceededRangeOnRotationConversion; }

  private:

    bool m_HasMotion;

    static void ComputeAverageAndDiffType( i16& average, u8& diffType, const i16& min, const i16& max );
  };

  typedef Nocturnal::SmartPtr< CompressedAnimation > CompressedAnimationPtr;
  typedef std::vector< CompressedAnimationPtr > V_CompressedAnimation;
  typedef std::map< Nocturnal::UID::TUID, CompressedAnimationPtr > M_CompressedAnimation;
}

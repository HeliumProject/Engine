#pragma once

#include "Pipeline/API.h"
#include "Foundation/Reflect/Serializers.h"
#include <hash_map>

#include "JointAnimation.h"
#include "Pipeline/Content/Constants.h"

// i am sorry, this is a fast first pass at translating this from code
// that was worked on by many people under pressure, this is pretty
// rough, but I am hoping it's at least somewhat cleaner and more
// contained than it was before. :(

namespace Content
{
  
  struct AnimationCompressionControl;

  class PIPELINE_API CompressedJointAnimation : public Reflect::Element
  {
  public:
    
    CompressedJointAnimation();
    CompressedJointAnimation( const AnimationCompressionControl& compressionControl, bool looping = false );

    bool m_Looping;

    bool m_UseHypercubeCompression;
    u32  m_TranslateScale;
    u32  m_ScaleScale;
    u32  m_RotateScale;

    // The values in the animation
    std::vector< i16 > m_TranslateX;
    std::vector< i16 > m_TranslateY;
    std::vector< i16 > m_TranslateZ;

    std::vector< i16 > m_RotateX;
    std::vector< i16 > m_RotateY;
    std::vector< i16 > m_RotateZ;
    std::vector< i16 > m_RotateW;

    std::vector< i16 > m_ScaleX;
    std::vector< i16 > m_ScaleY;
    std::vector< i16 > m_ScaleZ;

    std::vector< i16 > m_BindTranslate;  // 3 elements: x, y, z
    std::vector< i16 > m_MinTranslate;
    std::vector< i16 > m_MaxTranslate;
    std::vector< i16 > m_AvgTranslate;
    std::vector< u8 >  m_DiffTypeTranslate;

    std::vector< i16 > m_MinScale;
    std::vector< i16 > m_MaxScale;
    std::vector< i16 > m_AvgScale;
    std::vector< u8 >  m_DiffTypeScale;

    std::vector< i16 > m_MinRotate;
    std::vector< i16 > m_MaxRotate;
    std::vector< i16 > m_AvgRotate;
    std::vector< u8 >  m_DiffTypeRotate;

    Helium::TUID m_JointID;

    REFLECT_DECLARE_CLASS( CompressedJointAnimation, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<CompressedJointAnimation>& comp );

    bool m_ExceededRangeOnBindConversion;
    bool m_ExceededRangeOnTranslationConversion;
    bool m_ExceededRangeOnScaleConversion;
    bool m_ExceededRangeOnRotationConversion;

    void Compress( const tstring& jt_name, const JointAnimationPtr& uncompressedAnimation, const Math::Vector3& bindTranslate );

    u32 NumSamples() const;
    bool HasMotion() const { return m_HasMotion; }

    inline bool ExceededRangeOnTranslationConversion() { return m_ExceededRangeOnTranslationConversion; }
    inline bool ExceededRangeOnScaleConversion() { return m_ExceededRangeOnScaleConversion; }
    inline bool ExceededRangeOnRotationConversion() { return m_ExceededRangeOnRotationConversion; }

  private:

    bool m_HasMotion;

    static void ComputeAverageAndDiffType( i16& average, u8& diffType, const i16& min, const i16& max );
  };

  typedef Helium::SmartPtr< CompressedJointAnimation > CompressedJointAnimationPtr;
  typedef std::vector< CompressedJointAnimationPtr > V_CompressedJointAnimation;
  typedef std::map< Helium::TUID, CompressedJointAnimationPtr > M_CompressedJointAnimation;
}

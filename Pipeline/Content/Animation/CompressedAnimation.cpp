#include "CompressedAnimation.h"

#include "Foundation/Math/Conversion.h"

#include "Foundation/Log.h"

using namespace Math;

namespace Content
{

  REFLECT_DEFINE_CLASS( CompressedAnimation );

void CompressedAnimation::EnumerateClass( Reflect::Compositor<CompressedAnimation>& comp )
{
  Reflect::Field* fieldLooping = comp.AddField( &CompressedAnimation::m_Looping, "m_Looping" );

  Reflect::Field* fieldUseHypercubeCompression = comp.AddField( &CompressedAnimation::m_UseHypercubeCompression, "m_UseHypercubeCompression" );
  Reflect::Field* fieldTranslateScale = comp.AddField( &CompressedAnimation::m_TranslateScale, "m_TranslateScale" );
  Reflect::Field* fieldScaleScale = comp.AddField( &CompressedAnimation::m_ScaleScale, "m_ScaleScale" );
  Reflect::Field* fieldRotateScale = comp.AddField( &CompressedAnimation::m_RotateScale, "m_RotateScale" );
  
  Reflect::Field* fieldTranslateX = comp.AddField( &CompressedAnimation::m_TranslateX, "m_TranslateX" );
  Reflect::Field* fieldTranslateY = comp.AddField( &CompressedAnimation::m_TranslateY, "m_TranslateY" );
  Reflect::Field* fieldTranslateZ = comp.AddField( &CompressedAnimation::m_TranslateZ, "m_TranslateZ" );

  Reflect::Field* fieldRotateX = comp.AddField( &CompressedAnimation::m_RotateX, "m_RotateX" );
  Reflect::Field* fieldRotateY = comp.AddField( &CompressedAnimation::m_RotateY, "m_RotateY" );
  Reflect::Field* fieldRotateZ = comp.AddField( &CompressedAnimation::m_RotateZ, "m_RotateZ" );
  Reflect::Field* fieldRotateW = comp.AddField( &CompressedAnimation::m_RotateW, "m_RotateW" );

  Reflect::Field* fieldScaleX = comp.AddField( &CompressedAnimation::m_ScaleX, "m_ScaleX" );
  Reflect::Field* fieldScaleY = comp.AddField( &CompressedAnimation::m_ScaleY, "m_ScaleY" );
  Reflect::Field* fieldScaleZ = comp.AddField( &CompressedAnimation::m_ScaleZ, "m_ScaleZ" );

  Reflect::Field* fieldMinTranslate = comp.AddField( &CompressedAnimation::m_MinTranslate, "m_MinTranslate" );
  Reflect::Field* fieldMaxTranslate = comp.AddField( &CompressedAnimation::m_MaxTranslate, "m_MaxTranslate" );
  Reflect::Field* fieldAvgTranslate = comp.AddField( &CompressedAnimation::m_AvgTranslate, "m_AvgTranslate" );
  Reflect::Field* fieldDiffTypeTranslate = comp.AddField( &CompressedAnimation::m_DiffTypeTranslate, "m_DiffTypeTranslate" );
  
  Reflect::Field* fieldMinScale = comp.AddField( &CompressedAnimation::m_MinScale, "m_MinScale" );
  Reflect::Field* fieldMaxScale = comp.AddField( &CompressedAnimation::m_MaxScale, "m_MaxScale" );
  Reflect::Field* fieldAvgScale = comp.AddField( &CompressedAnimation::m_AvgScale, "m_AvgScale" );
  Reflect::Field* fieldDiffTypeScale = comp.AddField( &CompressedAnimation::m_DiffTypeScale, "m_DiffTypeScale" );

  Reflect::Field* fieldMinRotate = comp.AddField( &CompressedAnimation::m_MinRotate, "m_MinRotate" );
  Reflect::Field* fieldMaxRotate = comp.AddField( &CompressedAnimation::m_MaxRotate, "m_MaxRotate" );
  Reflect::Field* fieldAvgRotate = comp.AddField( &CompressedAnimation::m_AvgRotate, "m_AvgRotate" );
  Reflect::Field* fieldDiffTypeRotate = comp.AddField( &CompressedAnimation::m_DiffTypeRotate, "m_DiffTypeRotate" );
}


  CompressedAnimation::CompressedAnimation()
    : m_Looping( false )
    , m_ExceededRangeOnBindConversion( false )
    , m_ExceededRangeOnTranslationConversion( false )
    , m_ExceededRangeOnScaleConversion( false )
    , m_ExceededRangeOnRotationConversion( false )
    , m_BlendFactor( -1.0f )
  {
    AnimationCompressionControl defaultCompressionControl;
    m_UseHypercubeCompression = defaultCompressionControl.m_UseHypercubeCompression;
    m_TranslateScale = defaultCompressionControl.m_TranslateScale;
    m_ScaleScale = defaultCompressionControl.m_ScaleScale;
    m_RotateScale = defaultCompressionControl.m_RotationScale;
  }

  CompressedAnimation::CompressedAnimation(
      const AnimationCompressionControl& compressionControl,
      bool looping
      )
    : m_Looping( looping )
    , m_UseHypercubeCompression( compressionControl.m_UseHypercubeCompression )
    , m_TranslateScale( compressionControl.m_TranslateScale )
    , m_ScaleScale( compressionControl.m_ScaleScale )
    , m_RotateScale( compressionControl.m_RotationScale )
    , m_ExceededRangeOnBindConversion( false )
    , m_ExceededRangeOnTranslationConversion( false )
    , m_ExceededRangeOnScaleConversion( false )
    , m_ExceededRangeOnRotationConversion( false )
    , m_BlendFactor( -1.0f )
  {
    m_BindTranslate.resize( 3 );
    m_MinTranslate.resize( 3, Math::I16_UPPER_BOUND );
    m_MaxTranslate.resize( 3, Math::I16_LOWER_BOUND );
    m_AvgTranslate.resize( 3 );
    m_DiffTypeTranslate.resize( 3 );

    m_MinScale.resize( 3, Math::I16_UPPER_BOUND );
    m_MaxScale.resize( 3, Math::I16_LOWER_BOUND );
    m_AvgScale.resize( 3 );
    m_DiffTypeScale.resize( 3 );

    m_MinRotate.resize( 4, Math::I16_UPPER_BOUND );
    m_MaxRotate.resize( 4, Math::I16_LOWER_BOUND );
    m_AvgRotate.resize( 4 );
    m_DiffTypeRotate.resize( 4 );
  }

  u32 CompressedAnimation::NumSamples() const
  {
    NOC_ASSERT( m_TranslateX.size() == m_TranslateY.size() == m_TranslateX.size() );
    NOC_ASSERT( m_RotateX.size() == m_RotateY.size() == m_RotateZ.size() == m_RotateW.size() );
    NOC_ASSERT( m_ScaleX.size() == m_ScaleY.size() == m_ScaleZ.size() );
    NOC_ASSERT( m_TranslateX.size() == m_RotateX.size() == m_ScaleX.size() );

    return (u32) m_TranslateX.size();
  }

  void CompressedAnimation::ComputeAverageAndDiffType( i16& average, u8& diffType, const i16& min, const i16& max )
  {
    average  = ( min + max ) / 2;
    u32 diff = ( max - min );

    diffType = ( diff == 0 ) ? 0 : ( diff > 255 ) ? 1 : 2;
  }

  void CompressedAnimation::Compress(const std::string& jt_name,  const AnimationPtr& uncompressedAnimation, const Math::Vector3& bindTranslate )
  {
    // copy over their blend factor for convenience
    m_BlendFactor = uncompressedAnimation->m_BlendFactor;

    f32 translateScale = (f32)( 1 << m_TranslateScale );
    f32 scaleScale = (f32)( 1 << m_ScaleScale );
    f32 rotateScale = (f32)( 1 << m_RotateScale );

    // bind frame, having these as dimension-indexed arrays makes some
    // code nicer in the builders...
    bool status = true;
    status &= FloatToI16( m_BindTranslate[ 0 ], bindTranslate.x, translateScale );
    status &= FloatToI16( m_BindTranslate[ 1 ], bindTranslate.y, translateScale );
    status &= FloatToI16( m_BindTranslate[ 2 ], bindTranslate.z, translateScale );

#pragma TODO( "this also happens in moby builder right now, that should eventually go away in preference to this" )
#pragma TODO( "this auto-reduction may not actually work the way we would hope, but it at least gets the animation working in some condition instead of failing completely" )
    bool autoCompressionReduction = false;
    u32 compressionReductionAmount = 1;
    while( !status && compressionReductionAmount <= m_TranslateScale )
    {
      autoCompressionReduction = true;

      translateScale = (f32)( 1 << ( m_TranslateScale - compressionReductionAmount ) );

      status = true;
      status &= Math::FloatToI16( m_BindTranslate[ 0 ], bindTranslate.x, translateScale );
      status &= Math::FloatToI16( m_BindTranslate[ 1 ], bindTranslate.y, translateScale );
      status &= Math::FloatToI16( m_BindTranslate[ 2 ], bindTranslate.z, translateScale );

      ++compressionReductionAmount;
    }

    if ( autoCompressionReduction )
    {
      Log::Warning( "Joint '%s' transform exceeded range, compression automatically reduced to: %d\n", jt_name.c_str(), m_TranslateScale - compressionReductionAmount );
    }

    m_ExceededRangeOnBindConversion = !status;

    ////////////////////////////////////////////////////////////////////////////////////

    m_HasMotion = uncompressedAnimation->HasMotion();

    u32 numSamples = m_HasMotion ? uncompressedAnimation->WindowSamples() : 1; // only encode one frame if there's no motion
    u32 numOutputSamples = m_Looping ? numSamples + 1 : numSamples;

    m_TranslateX.resize( numOutputSamples );
    m_TranslateY.resize( numOutputSamples );
    m_TranslateZ.resize( numOutputSamples );

    m_ScaleX.resize( numOutputSamples );
    m_ScaleY.resize( numOutputSamples );
    m_ScaleZ.resize( numOutputSamples );

    m_RotateX.resize( numOutputSamples );
    m_RotateY.resize( numOutputSamples );
    m_RotateZ.resize( numOutputSamples );
    m_RotateW.resize( numOutputSamples );

    
    for ( u32 sampleIndex = 0; sampleIndex < numOutputSamples; ++sampleIndex )
    {
      // if the blendfactor is zero, just make this frame all 0s
      if ( m_BlendFactor == 0.0f )
      {
        m_TranslateX[ sampleIndex ] = m_TranslateY[ sampleIndex ] = m_TranslateZ[ sampleIndex ] = 0;
        m_ScaleX[ sampleIndex ]     = m_ScaleY[ sampleIndex ]     = m_ScaleZ[ sampleIndex ]     = 0;
        m_RotateX[ sampleIndex ]    = m_RotateY[ sampleIndex ]    = m_RotateZ[ sampleIndex ]    = m_RotateW[ sampleIndex ] = 0;
      }
      else
      {
        // scale
        status = true;
        status &= FloatToI16( m_ScaleX[ sampleIndex ], uncompressedAnimation->m_Scale[ sampleIndex % numSamples ].x, scaleScale );
        status &= FloatToI16( m_ScaleY[ sampleIndex ], uncompressedAnimation->m_Scale[ sampleIndex % numSamples ].y, scaleScale );
        status &= FloatToI16( m_ScaleZ[ sampleIndex ], uncompressedAnimation->m_Scale[ sampleIndex % numSamples ].z, scaleScale );
        m_ExceededRangeOnScaleConversion |= !status;

        // rotation
        status = true;

        // NOTE: this is also done in the animation object itself, but here we need to be able to handle
        //       looping anims, where the rotation beyond the end hasn't been 'smoothed'
        Math::Quaternion rotation = uncompressedAnimation->m_Rotate[ sampleIndex % numSamples ];
        if ( m_Looping && sampleIndex >= numSamples )
        {
          if ( rotation.Dot( uncompressedAnimation->m_Rotate[ ( sampleIndex - 1 ) % numSamples ] ) < 0.0f )
            rotation = -rotation;
        }

        status &= FloatToI16( m_RotateX[ sampleIndex ], rotation.values.x, rotateScale );
        status &= FloatToI16( m_RotateY[ sampleIndex ], rotation.values.y, rotateScale );
        status &= FloatToI16( m_RotateZ[ sampleIndex ], rotation.values.z, rotateScale );
        status &= FloatToI16( m_RotateW[ sampleIndex ], rotation.values.w, rotateScale );
        m_ExceededRangeOnRotationConversion |= !status;

        // translation
        status = true;
        status &= FloatToI16( m_TranslateX[ sampleIndex ], uncompressedAnimation->m_Translate[ sampleIndex % numSamples ].x, translateScale );
        status &= FloatToI16( m_TranslateY[ sampleIndex ], uncompressedAnimation->m_Translate[ sampleIndex % numSamples ].y, translateScale );
        status &= FloatToI16( m_TranslateZ[ sampleIndex ], uncompressedAnimation->m_Translate[ sampleIndex % numSamples ].z, translateScale );
        m_ExceededRangeOnTranslationConversion |= !status;

        // 
        // mike's trick to improve quat compression (constrain to lie on the surface of a hypercube) - obvious !
        // 
        if ( m_UseHypercubeCompression )
        {
          // find max abs component
          i16 *rot[4] = { &m_RotateX[ sampleIndex ], &m_RotateY[ sampleIndex ], &m_RotateZ[ sampleIndex ], &m_RotateW[ sampleIndex ] };

          // find max abs component
          i32 max_abs = 0;
          u32 best_dimension  = 0;
          for( u32 dimension = 0; dimension < 4; ++dimension )
          {
            i32 abs_comp = abs( *rot[ dimension ] );
            if ( abs_comp > max_abs )
            {
              max_abs = abs_comp;
              best_dimension = dimension;
            }
          }

          // compute rescale value
          f32 rescale_value = ( 23170.0f / (f32)max_abs );  // 23170 = 32768/sqrt(2)

          // rescale quat
          for( u32 dimension = 0; dimension < 4; ++dimension )
          {
            if ( dimension != best_dimension )
            {
              *rot[ dimension ] = (i32)((f32) *rot[ dimension ] * rescale_value );
            }
            else if ( *rot[ dimension ] < 0 )
            {
              *rot[ dimension ] = -23170;
            }
            else
            {
              *rot[ dimension ] = 23170;
            }
          }
        }
      }//@@@ loop sampleIndex


      //
      // calculate mins and maxs
      //
      i16* rotData[4]   = { &m_RotateX[ sampleIndex ],    &m_RotateY[ sampleIndex ],    &m_RotateZ[ sampleIndex ], &m_RotateW[ sampleIndex ] };
      i16* transData[3] = { &m_TranslateX[ sampleIndex ], &m_TranslateY[ sampleIndex ], &m_TranslateZ[ sampleIndex ] };
      i16* scaleData[3] = { &m_ScaleX[ sampleIndex ],     &m_ScaleY[ sampleIndex ],     &m_ScaleZ[ sampleIndex ] };

      for ( u32 dimension = 0; dimension < 4; ++dimension )
      {
        m_MaxRotate[ dimension ] = MAX( *rotData[ dimension ], m_MaxRotate[ dimension ] );
        m_MinRotate[ dimension ] = MIN( *rotData[ dimension ], m_MinRotate[ dimension ] );

        if ( dimension >= 3 )
          continue;

        m_MaxTranslate[ dimension ] = MAX( *transData[ dimension ], m_MaxTranslate[ dimension ] );
        m_MinTranslate[ dimension ] = MIN( *transData[ dimension ], m_MinTranslate[ dimension ] );

        m_MaxScale[ dimension ] = MAX( *scaleData[ dimension ], m_MaxScale[ dimension ] );
        m_MinScale[ dimension ] = MIN( *scaleData[ dimension ], m_MinScale[ dimension ] );
      }
    }

    //
    // calc averages and diff types
    //
    for ( u32 dimension = 0; dimension < 4; ++dimension )
    {
      ComputeAverageAndDiffType( m_AvgRotate[ dimension ], m_DiffTypeRotate[ dimension ], m_MinRotate[ dimension ], m_MaxRotate[ dimension ] );

      if ( dimension >= 3 )
        continue;

      ComputeAverageAndDiffType( m_AvgTranslate[ dimension ], m_DiffTypeTranslate[ dimension ], m_MinTranslate[ dimension ], m_MaxTranslate[ dimension ] );
      ComputeAverageAndDiffType( m_AvgScale[ dimension ], m_DiffTypeScale[ dimension ], m_MinScale[ dimension ], m_MaxScale[ dimension ] );
    }
  }
}
#include "PostEffectsDepthAttribute.h"
#include "Reflect/Serializer.h"

using namespace Reflect; 

namespace Content
{
  REFLECT_DEFINE_CLASS( PostEffectsDepthAttribute );

void PostEffectsDepthAttribute::EnumerateClass( Reflect::Compositor<PostEffectsDepthAttribute>& comp )
{
  comp.GetComposite().m_UIName = "Post Effects Depth";

  Reflect::Field* fieldUseLocalLightDirection = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalLightDirection, "m_UseLocalLightDirection" );

  Reflect::EnumerationField* enumLightColorSelection = comp.AddEnumerationField( &PostEffectsDepthAttribute::m_LightColorSelection, "m_LightColorSelection" );
  Reflect::Field* fieldLightColor = comp.AddField( &PostEffectsDepthAttribute::m_LightColor, "m_LightColor" );

  Reflect::Field* fieldUseLocalLightIntensity = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalLightIntensity, "m_UseLocalLightIntensity" );
  Reflect::Field* fieldLightIntensity = comp.AddField( &PostEffectsDepthAttribute::m_LightIntensity, "m_LightIntensity" );
  fieldLightIntensity->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=0.3} value{}].]" );

  Reflect::Field* fieldUseLocalMieCoefficient = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalMieCoefficient, "m_UseLocalMieCoefficient" );
  Reflect::Field* fieldMieCoefficient = comp.AddField( &PostEffectsDepthAttribute::m_MieCoefficient, "m_MieCoefficient" );
  fieldMieCoefficient->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1.0} value{}].]" );

  Reflect::Field* fieldUseLocalMieDirectional = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalMieDirectional, "m_UseLocalMieDirectional" );
  Reflect::Field* fieldMieDirectional = comp.AddField( &PostEffectsDepthAttribute::m_MieDirectional, "m_MieDirectional" );
  fieldMieDirectional->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1.0} value{}].]" );

  Reflect::Field* fieldUseLocalRayleighCoefficient = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalRayleighCoefficient, "m_UseLocalRayleighCoefficient" );
  Reflect::Field* fieldRayleighCoefficient = comp.AddField( &PostEffectsDepthAttribute::m_RayleighCoefficient, "m_RayleighCoefficient" );
  fieldRayleighCoefficient->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1.0} value{}].]" );

  Reflect::Field* fieldUseLocalDOFDistance = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalDOFDistance, "m_UseLocalDOFDistance" );
  Reflect::Field* fieldDOFDistance = comp.AddField( &PostEffectsDepthAttribute::m_DOFDistance, "m_DOFDistance" );
  fieldDOFDistance->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=1500.0} value{}].]" );

  Reflect::Field* fieldUseLocalDOFScale = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalDOFScale, "m_UseLocalDOFScale" );
  Reflect::Field* fieldDOFScale = comp.AddField( &PostEffectsDepthAttribute::m_DOFScale, "m_DOFScale" );
  fieldDOFScale->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=2.5} value{}].]" );

  Reflect::Field* fieldUseLocalDOFMax = comp.AddField( &PostEffectsDepthAttribute::m_UseLocalDOFMax, "m_UseLocalDOFMax" );
  Reflect::Field* fieldDOFMax = comp.AddField( &PostEffectsDepthAttribute::m_DOFMax, "m_DOFMax" );
  fieldDOFMax->SetProperty( "UIScript", "UI[.[slider{min=0.0; max=5.0} value{}].]" );

  Reflect::Field* fieldDepthEffectColor = comp.AddField( &PostEffectsDepthAttribute::m_DepthEffectColor, "m_DepthEffectColor", ContentFlags::ParametricKey );
  fieldDepthEffectColor->m_Create = &Reflect::CreateObject< ParametricColorKey >;

  Reflect::Field* fieldDepthEffectWeight = comp.AddField( &PostEffectsDepthAttribute::m_DepthEffectWeight, "m_DepthEffectWeight", ContentFlags::ParametricKey );
  fieldDepthEffectWeight->m_Create = &Reflect::CreateObject< ParametricIntensityKey >;
}


  /////////////////////////////////////////////////////////////////////////////
  // Constructor
  // 
  PostEffectsDepthAttribute::PostEffectsDepthAttribute()
    : m_LightColorSelection(PostEffectsDepthColorTypes::Local)
    , m_UseLocalLightDirection(false)
    , m_UseLocalLightIntensity(false)
    , m_UseLocalMieCoefficient(false)
    , m_UseLocalMieDirectional(false)
    , m_UseLocalRayleighCoefficient(false)
    , m_UseLocalColorCorrection(false)
    , m_UseLocalDOFDistance(false)
    , m_UseLocalDOFScale(false)   
    , m_UseLocalDOFMax(false)      
    , m_LightColor( 0xFF ) // Default color is white
    , m_LightIntensity( 0.05f )
    , m_MieCoefficient( 0.07f )
    , m_MieDirectional( 0.90f )
    , m_RayleighCoefficient( 0.88f )
    , m_DOFDistance( 300.0f )
    , m_DOFScale( 0.5f )
    , m_DOFMax( 3.0f )
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Destructor
  // 
  PostEffectsDepthAttribute::~PostEffectsDepthAttribute() 
  {
  }

  /////////////////////////////////////////////////////////////////////////////
  // Intended use for this attribute.
  // 
  Attribute::AttributeUsage PostEffectsDepthAttribute::GetAttributeUsage() const 
  { 
    return Attribute::AttributeUsages::Class; 
  }

  /////////////////////////////////////////////////////////////////////////////
  // Category to file this attribute in.
  // 
  Attribute::AttributeCategoryType PostEffectsDepthAttribute::GetCategoryType() const
  {
    return Attribute::AttributeCategoryTypes::PostEffects;
  }

  bool PostEffectsDepthAttribute::ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName)
  {
    if ( fieldName == "m_UseLightColor" )
    {
      bool useLightColor; 
      Serializer::GetValue( Reflect::AssertCast<Serializer>(element), useLightColor );

      if(useLightColor)
      {
        m_LightColorSelection = PostEffectsDepthColorTypes::Custom; 
      }
      else
      {
        m_LightColorSelection = PostEffectsDepthColorTypes::Local; 
      }
      return true; 
    }

    return __super::ProcessComponent(element, fieldName);
  }

  /////////////////////////////////////////////////////////////////////////////
  // The depth based color settings are only valid if:
  // 1) This attribute is enabled
  // 2) There are colors and weights specified
  // 3) All the weight values are not black (i.e. must have intensity > 0)
  // 
  bool PostEffectsDepthAttribute::IsDepthColorEnabled() const
  {
    if ( m_Enabled )
    {
      if ( m_DepthEffectColor.size() > 0 && m_DepthEffectWeight.size() > 0 )
      {
        V_ParametricIntensityKeyPtr::const_iterator weightItr = m_DepthEffectWeight.begin();
        V_ParametricIntensityKeyPtr::const_iterator weightEnd = m_DepthEffectWeight.end();
        for ( ; weightItr != weightEnd; ++weightItr )
        {
          const ParametricIntensityKeyPtr& weight = *weightItr;
          if ( weight->m_Intensity > 0 )
          {
            return true;
          }
        }
      }
    }

    return false;
  }
}
#pragma once

#include "API.h"
#include "Asset/AssetFlags.h"
#include "ParametricColorKey.h"
#include "ParametricIntensityKey.h"
#include "Attribute/Attribute.h"

namespace Content
{
  namespace PostEffectsDepthColorTypes
  {
    enum PostEffectsDepthColorType
    {
      Local, 
      Global,
      Custom, 
    };

    static void PostEffectsDepthColorTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(Local, "Local");
      info->AddElement(Global, "Global");
      info->AddElement(Custom, "Custom");
    }
  }
  
  typedef PostEffectsDepthColorTypes::PostEffectsDepthColorType PostEffectsDepthColorType; 

  /////////////////////////////////////////////////////////////////////////////
  // Light scattering settings for a level.
  // 
  class CONTENT_API PostEffectsDepthAttribute : public Attribute::AttributeBase
  {
  public:
    PostEffectsDepthColorType m_LightColorSelection; 
    bool m_UseLocalLightDirection;
    bool m_UseLocalLightIntensity;
    bool m_UseLocalMieCoefficient;
    bool m_UseLocalMieDirectional;
    bool m_UseLocalRayleighCoefficient;
    bool m_UseLocalColorCorrection;
    bool m_UseLocalDOFDistance;
    bool m_UseLocalDOFScale;   
    bool m_UseLocalDOFMax;     

    Math::Color3 m_LightColor;
    f32 m_LightIntensity;
    f32 m_MieCoefficient;
    f32 m_MieDirectional;
    f32 m_RayleighCoefficient;

    f32 m_DOFDistance; // camera distance to where the blur starts
    f32 m_DOFScale; // (divide by 10 for engine) "cone of confusion" scale, it controls how quickly it gets blurry 
    f32 m_DOFMax; // max size of the cone, which controls the max blurriness 

    V_ParametricColorKeyPtr m_DepthEffectColor;
    V_ParametricIntensityKeyPtr m_DepthEffectWeight;

  public:
    PostEffectsDepthAttribute();
    virtual ~PostEffectsDepthAttribute();

    virtual Attribute::AttributeUsage GetAttributeUsage() const NOC_OVERRIDE;
    virtual Attribute::AttributeCategoryType GetCategoryType() const NOC_OVERRIDE;

    virtual bool ProcessComponent(Reflect::ElementPtr element, const std::string& fieldName) NOC_OVERRIDE;

    bool IsDepthColorEnabled() const;

    REFLECT_DECLARE_CLASS( PostEffectsDepthAttribute, Attribute::AttributeBase );
    static void EnumerateClass( Reflect::Compositor<PostEffectsDepthAttribute>& comp );
  };

  typedef Nocturnal::SmartPtr< PostEffectsDepthAttribute > PostEffectsDepthAttributePtr;
}
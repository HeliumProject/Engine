#pragma once

#include "Light.h"
#include "Content/ShadowDirection.h"

namespace Luna
{
  class ShadowDirection : public Luna::Light
  {
    //
    // Runtime Type Info
    //

  public:
    LUNA_DECLARE_TYPE( Luna::ShadowDirection, Luna::Light );
    static void InitializeType();
    static void CleanupType();

    //
    // Member functions
    //

    ShadowDirection(Luna::Scene* scene);
    ShadowDirection(Luna::Scene* scene, Content::ShadowDirection* light);

    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual std::string GetApplicationTypeName() const NOC_OVERRIDE;
  };
}

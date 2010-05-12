#pragma once

#include "WizardPage.h"
#include "CreateAssetWizard.h"

#include "Common/Memory/SmartPtr.h"
#include "Common/Types.h"

// Forwards
namespace Attribute
{
  class AttributeBase;
  typedef Nocturnal::SmartPtr< AttributeBase > AttributePtr;
}

namespace AssetManager
{
  // Forwards
  class ShaderTexturesPanel;
  class TextureMapFieldPanel;

  class ShaderTexturesPage : public WizardPage< ShaderTexturesPanel >
  {
  private:
    ShaderTexturesPage( CreateAssetWizard* wizard );

  public:
    static UIToolKit::WizardPage* Create( CreateAssetWizard* wizard );
    virtual ~ShaderTexturesPage();

    // Returns properly typed wizard object.
    CreateAssetWizard* GetCreateAssetWizard() const
    {
      return wxStaticCast( __super::GetWizard(), CreateAssetWizard );
    }

    virtual bool TransferDataToWindow() NOC_OVERRIDE;
    virtual bool TransferDataFromWindow() NOC_OVERRIDE;

    virtual void Finished() NOC_OVERRIDE;   

  private:
    typedef std::map< Attribute::AttributeBase*, TextureMapFieldPanel* > M_AttribToTextureMapPanel;
    
    M_AttribToTextureMapPanel  m_AttribToTextureMapPanel;
  };

}
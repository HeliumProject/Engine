#pragma once

#include "Asset/ShaderGroupAttribute.h"
#include "Common/Container/ReversibleMap.h"

namespace Luna
{
  class AssetClass;
  class SwapShadersPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog for managing shader groups.  Shader groups basically allow for a 
  // palette swap on a character.  Users can specify that instances of a 
  // character can choose between different sets of shaders to use.
  //
  class ShaderGroupDlg : public wxDialog 
  {
  private:
    typedef Nocturnal::ReversibleMap< i32, tuid > M_ChoiceLookup;

  private:
    SwapShadersPanel* m_Panel;
    Luna::AssetClass* m_Asset;
    Asset::ShaderGroupAttributePtr m_ShaderGroupAttribute;
    M_ChoiceLookup m_ChoiceLookup;

  public:
    ShaderGroupDlg( wxWindow* parent, Luna::AssetClass* asset );
    virtual ~ShaderGroupDlg();
    virtual int ShowModal();
    Asset::ShaderGroupAttributePtr GetShaderGroupAttribute() const;
    void UpdateChoices();
    void SetThumbnail( std::string imagePath );

  private:
    void PopulateChoices();

    void OnChooseShaderGroup( wxCommandEvent& args );
  };
}
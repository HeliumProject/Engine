#pragma once

#include "Common/Types.h"
#include "Common/Compiler.h"

// Forwards
namespace Asset
{
  class AnimationConfigAsset;
}

typedef std::map< i32, std::string > M_i32str;

namespace Luna
{
  // Forwards
  class AnimGroupPanel;

  /////////////////////////////////////////////////////////////////////////////
  // Dialog showing a choice of category and modifier settings for a specified
  // AnimationConfigAsset.
  // 
  class AnimGroupDlg : public wxDialog
  {
  private:
		wxStdDialogButtonSizer* m_ButtonSizer;
    AnimGroupPanel* m_Panel;
    wxButton* m_ButtonOK;
    Asset::AnimationConfigAsset* m_AnimConfig;
    M_i32str m_ModifierLookup;
    std::string m_Category;

  public:
    AnimGroupDlg( wxWindow* parent, Asset::AnimationConfigAsset* animConfig, const std::string& title );
    virtual ~AnimGroupDlg();
    virtual int ShowModal() NOC_OVERRIDE;

    const std::string& GetCategory() const;
    void SetCategory( const std::string& category );

    void GetModifiers( M_string& modifiers ) const;
    void SetModifiers( const M_string& modifiers );

  private:
    void PopulateCategoryChoice();
    void PopulateModifierGrid();
    void OnCategoryChoice( wxCommandEvent& args );
  };
}

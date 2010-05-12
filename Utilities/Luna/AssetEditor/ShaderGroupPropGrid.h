#pragma once

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "Asset/ShaderGroupAttribute.h"
#include "Asset/ShaderUsagesAttribute.h"
#include <wx/propgrid/manager.h>


namespace Luna
{
  class AssetClass;
  class ShaderGroupDlg;

  /////////////////////////////////////////////////////////////////////////////
  // Property grid control that allows you to manipulate shader groups.
  // 
  class ShaderGroupPropGrid : public wxPropertyGridManager
  {
  private:
    ShaderGroupDlg* m_Dialog;
    Asset::ShaderUsagesAttributePtr m_ShaderUsagesAttribute;
    Asset::ShaderGroupAttributePtr m_ShaderGroupAttribute;

  public:
    ShaderGroupPropGrid( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxPGMAN_DEFAULT_STYLE, const wxChar* name = wxPropertyGridManagerNameStr );
    virtual ~ShaderGroupPropGrid();
    void Initialize( ShaderGroupDlg* dialog, Asset::ShaderUsagesAttributePtr shaderUsages, Asset::ShaderGroupAttributePtr shaderGroup );
    bool Validate( std::string& error );
    Asset::ShaderGroupAttributePtr GetShaderGroupAttribute() const;

  protected:
    void MakePage( Asset::ShaderGroupPtr shaderGroup );
    void UpdateThumbnail( const std::string& shaderPath );

    void OnAddShaderGroup( wxCommandEvent& args );
    void OnDeleteShaderGroup( wxCommandEvent& args );
    friend class ShaderGroupDlg;

    void OnPropertyGridChange( wxPropertyGridEvent& args );
    void OnPropertySelected( wxPropertyGridEvent& args );
  };
}

#pragma once

#include "Frame.h"
#include "BuildServer.h"

namespace Luna
{
  class BuildInfo;
  class RunGamePanel;
  class LevelInfo;
  typedef Nocturnal::SmartPtr< LevelInfo > LevelInfoPtr;

  //////////////////////////////////////////////////////////////////////////////
  //
  // 
  class RunGameFrame : public Frame
  {
  private:
    RunGamePanel* m_Panel;
    BuildServer m_BuildServer;
    const BuildInfo* m_SelectedBuild;
    wxArrayString m_Builds;
    i32 m_IconOk;
    i32 m_IconError;

  public:
    RunGameFrame( wxWindow* parent );
    virtual ~RunGameFrame();

  private:
    virtual void SaveWindowState() NOC_OVERRIDE;
    virtual const std::string& GetPreferencePrefix() const NOC_OVERRIDE;

    void SelectBuild();
    void RefreshBuilds();
    void RefreshLevels();
    void AddLevelItem( const LevelInfoPtr& level, u32 index );
    bool ValidateRun( std::string& error );

    void OnBuildSelected( wxCommandEvent& args );
    void OnLevelSelected( wxListEvent& args );
    void OnSelectAll( wxCommandEvent& args );
    void OnSelectNone( wxCommandEvent& args );
    void OnRerun( wxCommandEvent& args );
    void OnRun( wxCommandEvent& args );
  };
}

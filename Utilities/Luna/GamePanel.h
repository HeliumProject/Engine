#pragma once

#include "LunaGenerated.h"

namespace Luna
{
  class GameRowPanel;

  class GamePanel : public GamePanelGenerated
  {
  public:
    GamePanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
    virtual ~GamePanel();

    void AddRow( GameRowPanel* row );
    void DeleteRow( GameRowPanel* row );
    void DeleteAllRows();
    void SelectRow( GameRowPanel* row );

  private:

  private:
  };
}

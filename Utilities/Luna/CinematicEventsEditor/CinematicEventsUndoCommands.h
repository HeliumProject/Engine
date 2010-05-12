#pragma once

#include "Undo/Command.h"
#include "Asset/CinematicAsset.h"

namespace Luna
{
  class CinematicEventsManager;

  class CinematicEventsChangedCommand : public Undo::Command
  {
  public:
    CinematicEventsChangedCommand( CinematicEventsManager& manager );

    virtual void Undo();
    virtual void Redo();

    void UpdateValues();
    void Swap();

  public:
    Asset::CinematicAssetPtr          m_Clip;
    Asset::OS_CinematicEvent          m_Events;
    Asset::OS_CinematicEvent          m_Selection;
    int                                   m_Frame;
    CinematicEventsManager& m_Manager;
  };
}
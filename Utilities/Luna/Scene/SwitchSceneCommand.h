#pragma once

#include "API.h"
#include "Undo/PropertyCommand.h"

namespace Luna
{
  // Forwards
  class Scene;
  class SceneManager;
  struct SceneChangeArgs;

  /////////////////////////////////////////////////////////////////////////////
  // Undo command for switching the current scene.
  // 
  class LSwitchSceneCommand : public Undo::PropertyCommand< Luna::Scene* >
  {
  private:
    Luna::SceneManager* m_SceneManager;
    Luna::Scene* m_OldScene;
    Luna::Scene* m_NewScene;
    bool m_IsValid;

  public:
    LSwitchSceneCommand( Luna::SceneManager* manager, Luna::Scene* newScene );
    virtual ~LSwitchSceneCommand();
    virtual bool IsSignificant() const NOC_OVERRIDE;
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;

  private:
    void SceneRemoving( const SceneChangeArgs& args );
  };
}

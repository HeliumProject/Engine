#pragma once

#include "Luna/API.h"
#include "Editor/Document.h"

namespace Luna
{
  // Forwards
  class Scene;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for files edited by the scene editor.  Handles RCS prompts (in the
  // base class) and stores a pointer to the scene that this file is associated
  // with.
  // 
  class SceneDocument : public Document
  {
  private:
    Luna::Scene* m_Scene;

    // RTTI
    LUNA_DECLARE_TYPE( SceneDocument, Document );
    static void InitializeType();
    static void CleanupType();

  public:
    SceneDocument( const tstring& file, const tstring& name = TXT( "" ) );
    virtual ~SceneDocument();
    void SetScene( Luna::Scene* scene );
    Luna::Scene* GetScene() const;
  };

  typedef Nocturnal::SmartPtr< SceneDocument > SceneDocumentPtr;
}
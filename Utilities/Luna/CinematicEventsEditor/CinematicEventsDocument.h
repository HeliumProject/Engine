#pragma once

#include "API.h"
#include "Editor/Document.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationSetAsset.h"
#include "Asset/CinematicAsset.h"

namespace Luna
{
  class CinematicEventsManager;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for files edited by the cinematic events editor.  Handles RCS prompts (in the
  // base class) and keeps references to the edited animation clips
  // 
  class CinematicEventsDocument : public Document
  {
  private:
    CinematicEventsManager* m_Manager;

    // RTTI
    LUNA_DECLARE_TYPE( CinematicEventsDocument, Document );
    static void InitializeType();
    static void CleanupType();

  public:
    CinematicEventsDocument( CinematicEventsManager* manager, const Asset::AssetClassPtr& cinematicAsset );
    virtual ~CinematicEventsDocument();

    void Initialize();
    bool Save();
    bool Edit();
    bool EditCinematic();

    std::string GetCinematicEventsFile();
    Asset::CinematicEventListPtr GetCinematicEventList();

  public:
    Asset::AssetClassPtr        m_CinematicAsset;

    typedef std::map< std::string, Asset::CinematicEventListPtr > M_StringToEventList;
    M_StringToEventList         m_EventLists;
    
    typedef std::map< std::string, DocumentPtr > M_StringToDocument;
    M_StringToDocument        m_Files;
  };

  typedef Nocturnal::SmartPtr< CinematicEventsDocument > CinematicEventsDocumentPtr;
}
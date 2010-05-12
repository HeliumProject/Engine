#pragma once

#include "API.h"
#include "Editor/Document.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationSetAsset.h"
#include "Asset/AnimationClip.h"

namespace Luna
{
  class AnimationEventsManager;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for files edited by the effect events editor.  Handles RCS prompts (in the
  // base class) and keeps references to the edited animation clips
  // 
  class AnimationEventsDocument : public Document
  {
  private:
    AnimationEventsManager* m_Manager;

    // RTTI
    LUNA_DECLARE_TYPE( AnimationEventsDocument, Document );
    static void InitializeType();
    static void CleanupType();

  public:
    AnimationEventsDocument( AnimationEventsManager* manager, const Asset::AssetClassPtr& mobyAsset );
    virtual ~AnimationEventsDocument();

    void Initialize();
    bool Save();
    bool Edit( const Asset::AnimationClipDataPtr& clip );
    bool EditAnimSet();

    std::string GetClipEventsFile( const Asset::AnimationClipDataPtr& clip );
    Asset::AnimationEventListPtr GetClipEventList( const Asset::AnimationClipDataPtr& clip );

  public:
    Asset::AssetClassPtr        m_MobyClass;
    Asset::AnimationSetAssetPtr m_AnimationSetAsset;
    Asset::V_AnimationClipData  m_ClipData;
    Asset::S_AnimationClipData  m_DirtyClips;

    typedef std::map< std::string, Asset::AnimationEventListPtr > M_StringToEventList;
    M_StringToEventList         m_EventLists;
    
    typedef std::map< std::string, DocumentPtr > M_StringToDocument;
    M_StringToDocument        m_Files;
  };

  typedef Nocturnal::SmartPtr< AnimationEventsDocument > AnimationEventsDocumentPtr;
}
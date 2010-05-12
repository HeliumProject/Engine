#include "AssetEditorGenerated.h"
#include "AssetManager.h"
#include "AssetClass.h" 
#include "RemoteAsset.h"

#include "Asset/EntityAsset.h"
#include "Asset/AnimationSetAsset.h"
#include "Content/Scene.h"

#include "Live/RemoteEditor.h"

namespace Luna
{
  class AssetEditor; 
  class AssetManager; 

  class MultiAnimPanel : public MultiAnimPanelGenerated
  {
  public: 
    MultiAnimPanel( AssetEditor* assetEditor ); 
    ~MultiAnimPanel(); 

  private: 
    struct EntityData
    {
      Asset::EntityAssetPtr       m_EntityClass; 
      Asset::AnimationSetAssetPtr m_AnimSet; 
      wxChoice*                   m_JointChoice; 
      Content::ScenePtr           m_Scene; 
      Content::JointTransformPtr  m_Joint; 
    };

    enum RemoteLoadFlags
    {
      LoadRemote       = 1 << 0, 
      DoNotLoadRemote  = 1 << 1,
    };

    Luna::AssetManager*       m_Manager; 
    Asset::V_EntityAsset m_EntityClasses;
    S_AssetClassSmartPtr m_Assets; 
    Luna::AttachmentPtr  m_Attachment; 

    EntityData           m_MainData; 
    EntityData           m_AttachData; 

    V_tuid               m_SharedClips; 
    tuid                 m_AnimClip; 

    // boolean values for controlling state

    bool                 m_EnableJointAttach;
    bool                 m_ForceLoop; 


    void PopulateJointIndices(EntityData* entityData); 
    void PopulateEntityDropdown(wxChoice* choice); 
    void UpdateEntityDropdown(wxChoice* choice, Asset::EntityAsset* entityClass);
    void PopulateAnimClips(); 

    void OnAssetLoaded(const AssetLoadArgs& args); 
    void OnAssetUnloaded(const AssetLoadArgs& args); 

    void LoadEntityData(MultiAnimPanel::EntityData* entityData, Asset::EntityAsset* entityClass, u32 flags); 
    void UnloadEntityData(EntityData* entityData); 

    void TweakAttachment(); 
    void LoadRemoteAnimation(); 
    void LoadRemoteEntities(); 

    void OnSingleAssetView(Luna::AssetViewArgs& args); 
  
		virtual void OnMainEntityChoice( wxCommandEvent& event ); 
		virtual void OnMainJointChoice( wxCommandEvent& event ); 
		virtual void OnAttachEntityChoice( wxCommandEvent& event ); 
		virtual void OnAttachJointChoice( wxCommandEvent& event ); 
		virtual void OnAnimClipChoice( wxCommandEvent& event ); 
    virtual void OnAnimClipTextChoice( wxCommandEvent& event ); 
 		virtual void OnButtonSnapBegin( wxCommandEvent& event ); 
		virtual void OnButtonPlayPause( wxCommandEvent& event ); 
		virtual void OnButtonSnapEnd( wxCommandEvent& event ); 
    virtual void OnScroll( wxScrollEvent& event ); 
 		virtual void OnButtonEnableJointAttach( wxCommandEvent& event );
		virtual void OnButtonForceLoop( wxCommandEvent& event );
		virtual void OnButtonFrameEntities( wxCommandEvent& event );



  }; 

}
#include "Precompile.h"
#include "MultiAnimPanel.h"
#include "AssetEditor.h"
#include "RemoteAsset.h"

#include "Editor/Preferences.h"
#include "Editor/SessionManager.h"
#include "Task/Build.h"

#include "Asset/AnimationAttribute.h"
#include "Asset/ArtFileAttribute.h"
#include "Asset/EngineType.h"
#include "Content/Scene.h"
#include "FileSystem/FileSystem.h"
#include "Finder/ContentSpecs.h"
#include "UIToolKit/ImageManager.h"
#include "UIToolKit/AutoCompleteComboBox.h"

#include <wx/arrstr.h>
#include <wx/utils.h>
#include <algorithm>
#include <strstream>

namespace Luna
{
  MultiAnimPanel::MultiAnimPanel( AssetEditor* assetEditor )
    : MultiAnimPanelGenerated( assetEditor )
    , m_Manager( assetEditor->GetAssetManager() )
    , m_AnimClip( TUID::Null )
    , m_EnableJointAttach( true )
    , m_ForceLoop( false )
  {
    m_Manager->AddAssetLoadedListener( AssetLoadSignature::Delegate(this, &MultiAnimPanel::OnAssetLoaded ) ); 
    m_Manager->AddAssetUnloadingListener( AssetLoadSignature::Delegate(this, &MultiAnimPanel::OnAssetUnloaded ) ); 

    PopulateEntityDropdown(m_MainEntityChoice); 
    PopulateEntityDropdown(m_AttachEntityChoice); 

    m_MainData.m_JointChoice = m_MainJointChoice; 
    m_AttachData.m_JointChoice = m_AttachJointChoice; 

    // pause_16.png
    m_ButtonPlayPause->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "play_16.png" ) );
    m_ButtonPlayPause->Enable(); 
    
    m_ButtonSnapBegin->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "snap_begin_16.png" ) );
    m_ButtonSnapBegin->Enable(); 
    
    m_ButtonSnapEnd->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "snap_end_16.png" ) );
    m_ButtonSnapEnd->Enable(); 

    m_ButtonEnableJointAttach->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "joint_32.png" ) );
    m_ButtonEnableJointAttach->Enable(); 

    m_ButtonForceLoop->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "refresh_32.png" ) );
    m_ButtonForceLoop->Enable( false ); 
    m_ButtonForceLoop->Hide(); 

    m_ButtonFrameEntities->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "view_32.png" ) );
    m_ButtonFrameEntities->Enable( false ); 
    m_ButtonFrameEntities->Hide(); 
    
    
    RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 
    remote->AddAssetViewListener( Luna::AssetViewSignature::Delegate(this, &MultiAnimPanel::OnSingleAssetView)); 
    

  }

  MultiAnimPanel::~MultiAnimPanel()
  {
    RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 
    if(remote)
    {
      remote->RemoveAssetViewListener( Luna::AssetViewSignature::Delegate(this, &MultiAnimPanel::OnSingleAssetView)); 
    }

    m_Manager->RemoveAssetLoadedListener( AssetLoadSignature::Delegate(this, &MultiAnimPanel::OnAssetLoaded ) ); 
    m_Manager->RemoveAssetUnloadingListener( AssetLoadSignature::Delegate(this, &MultiAnimPanel::OnAssetUnloaded ) ); 
  }

  void MultiAnimPanel::PopulateEntityDropdown(wxChoice* choice)
  {
    int currentSelection = choice->GetSelection(); 
    void* clientData = NULL;
    
    // Don't fetch client data unless there is any... item 0 is always <none> so skip it
    if ( currentSelection > 0 )
    {
      clientData = choice->GetClientData(currentSelection); 
    }

    int nextSelection = 0; 
    wxArrayString strings;

    strings.push_back( "<none>" ); 
    for(u32 i = 0; i < m_EntityClasses.size(); ++i)
    {
      Asset::EntityAsset* entityClass = m_EntityClasses[i]; 
      if(clientData && entityClass == clientData)
      {
        nextSelection = i+1; 
      }

      strings.push_back( TuidToLabel( entityClass->GetFileID(), FilePathOptions::FileNamePlusExt) ); 
    }

    choice->Clear(); 
    choice->Append(strings); 
    choice->SetClientData(0, NULL); 
    for(u32 i = 0; i < m_EntityClasses.size(); ++i)
    {
      // off by one, since <none> does not have client data set
      choice->SetClientData( i+1, m_EntityClasses[i] ); 
    }
    choice->SetSelection(nextSelection); 
  }

  void MultiAnimPanel::UpdateEntityDropdown(wxChoice* choice, Asset::EntityAsset* entityClass)
  {
    for(u32 i = 1; i < choice->GetCount(); ++i)
    {
      void* clientData = choice->GetClientData(i); 
      if(entityClass == (Asset::EntityAsset*) clientData)
      {
        choice->SetSelection(i); 
        return; 
      }
    }

    choice->SetSelection(0); 
  }

  void MultiAnimPanel::PopulateJointIndices(EntityData* entityData)
  {
    if(entityData->m_Scene == NULL)
    {
      return; 
    }

    Content::V_JointTransform joints; 
    entityData->m_Scene->GetAll<Content::JointTransform>(joints); 

    int currentSelection = entityData->m_JointChoice->GetSelection(); 
    void* clientData = (currentSelection >= 0) ? entityData->m_JointChoice->GetClientData(currentSelection) : NULL;

    int nextSelection = 0; 

    entityData->m_Joint = NULL; 

    entityData->m_JointChoice->Clear(); 
    entityData->m_JointChoice->Append("<root>"); 

    for(u32 i = 0; i < joints.size(); ++i)
    {
      if( joints[i]->GetDynamicJointID() != -1 )
      {
        void* nextData = joints[i].Ptr(); 
        entityData->m_JointChoice->Append( joints[i]->GetName(), nextData ); 

        if(clientData == nextData)
        {
          nextSelection = i; 
        }
      }
    }

    entityData->m_JointChoice->SetSelection(nextSelection); 
    if(nextSelection == 0)
    {
      // invalidate the saved selections
      entityData->m_Joint = NULL; 
    }
  }

  void MultiAnimPanel::PopulateAnimClips()
  {
    // get the current selection and the current tuid 
    // before we clear the m_SharedClips because clientData points into that
    //
    int currentSelection = m_AnimClipCombo->GetSelection(); 
    tuid currentTuid = TUID::Null;

    if(currentSelection >= 0)
    {
      void* clientData = m_AnimClipCombo->GetClientData(currentSelection); 
      if(clientData)
      {
        currentTuid = *(tuid*) clientData; 
      }
    }

    // figure out what our set of selectable clips are
    //
    Asset::V_AnimationClipData mainClipData; 
    Asset::V_AnimationClipData attachClipData; 
    Asset::M_ClipToIndex clipMapping; // required but ignored in this function

    // we keep just the tuids for compare...
    V_tuid mainClips; 
    V_tuid attachClips; 

    if(m_MainData.m_AnimSet)
    {
      m_MainData.m_AnimSet->GatherUniqueClips(mainClipData, clipMapping); 
      for(u32 i = 0; i < mainClipData.size(); ++i)
      {
        mainClips.push_back( mainClipData[i]->m_ArtFile ); 
      }
    }

    V_tuid nextClips; 

    if(m_EnableJointAttach)
    {
       nextClips = mainClips; 
    }
    else
    {
      // play the same anim clip on both guys, so they must have the same clip in their set
      // form the intersection of their sets
      //
      if( m_AttachData.m_AnimSet)
      {
        m_AttachData.m_AnimSet->GatherUniqueClips(attachClipData, clipMapping); 
        for(u32 i = 0; i < attachClipData.size(); ++i)
        {
          attachClips.push_back( attachClipData[i]->m_ArtFile ); 
        }

        std::set_intersection( mainClips.begin(), mainClips.end(), 
                               attachClips.begin(), attachClips.end(), 
                               std::back_inserter( nextClips ) ); 
      }
      else
      {
        // this only makes sense when there is no attach moby
        // and we just want to play the clips on the main guy
        // 
        nextClips = mainClips; 
      }
    }

    if(nextClips != m_SharedClips)
    {
      m_SharedClips = nextClips; 

      // populate the ui. combo box sorts it for us
      //
      int nextSelection = -1; 

      m_AnimClipCombo->Clear(); 
      for(u32 i = 0; i < m_SharedClips.size(); ++i)
      {
        Asset::AnimationClipData data; 
        data.m_ArtFile = m_SharedClips[i]; 

        if(currentTuid == m_SharedClips[i])
        {
          nextSelection = i; 
        }

        m_AnimClipCombo->Append( data.GetTitle(), &m_SharedClips[i] ); 
      }
      m_AnimClipCombo->SetSelection(nextSelection); 
      if(nextSelection == -1)
      {
        m_AnimClip = TUID::Null; 
      }

    }

  }

  void MultiAnimPanel::OnAssetLoaded(const AssetLoadArgs& args)
  {
    m_Assets.insert( args.m_AssetClass );

    Reflect::ElementPtr element = args.m_AssetClass->GetPackage(); 
    Asset::EntityAssetPtr entityClass = Reflect::ObjectCast<Asset::EntityAsset>(element); 

    if(entityClass && entityClass->GetEngineType() == Asset::EngineTypes::Moby)
    {
      m_EntityClasses.push_back( entityClass ); 
    }

    PopulateEntityDropdown(m_MainEntityChoice); 
    PopulateEntityDropdown(m_AttachEntityChoice); 

  }

  void MultiAnimPanel::OnAssetUnloaded(const AssetLoadArgs& args)
  {
    m_Assets.erase( args.m_AssetClass );

    Reflect::ElementPtr element = args.m_AssetClass->GetPackage(); 
    Asset::EntityAssetPtr entityClass = Reflect::ObjectCast<Asset::EntityAsset>(element); 

    if(!entityClass)
    {
      return; 
    }

    // check to see if we have it in our list of entity classes. 
    // check the list instead of re-checking engine type in case we decide to use AnimationAttribute
    // as the insertion criteria instead of engine type. 
    // 
    Asset::V_EntityAsset::iterator found = std::find(m_EntityClasses.begin(), m_EntityClasses.end(), entityClass); 
    if(found == m_EntityClasses.end())
    {
      return; 
    }

    m_EntityClasses.erase( found ); 

    // if we had this as one of the selected classes, then clear it out
    //
    if(m_MainData.m_EntityClass == entityClass)
    {
      UnloadEntityData(&m_MainData); 
    }

    if(m_AttachData.m_EntityClass == entityClass)
    {
      UnloadEntityData(&m_AttachData); 
    }
  
    PopulateEntityDropdown(m_MainEntityChoice); 
    PopulateEntityDropdown(m_AttachEntityChoice); 

  }

  void MultiAnimPanel::LoadEntityData(MultiAnimPanel::EntityData* entityData, Asset::EntityAsset* entityClass, u32 flags)
  {
    if(entityData->m_EntityClass.Ptr() == entityClass)
    {
      return; 
    }

    entityData->m_EntityClass = entityClass; 
    entityData->m_AnimSet = NULL; 

    if(entityClass)
    {
      // need to load up the animset
      Asset::AnimationAttributePtr animAttr = entityClass->GetAttribute<Asset::AnimationAttribute>(); 
      Asset::AnimationSetAssetPtr animSet; 
      if(animAttr && animAttr->m_AnimationSetId != TUID::Null)
      { 
        Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass(animAttr->m_AnimationSetId); 
        animSet = Reflect::ObjectCast<Asset::AnimationSetAsset>(assetClass); 
      }

      entityData->m_AnimSet = animSet; 

      // need to load up the exported scene, in order to get the joints
      Asset::ArtFileAttributePtr artFile = entityClass->GetAttribute<Asset::ArtFileAttribute>(); 
      std::string exportPath; 
      if(artFile )
      {
        exportPath = FinderSpecs::Content::RIGGED_DECORATION.GetExportFile( artFile->GetFilePath() ); 
      }

      
      if(artFile && FileSystem::Exists(exportPath))
      {
        entityData->m_Scene = new Content::Scene; 
        entityData->m_Scene->Load( exportPath ); 
      }
      else
      {
        std::strstream ss; 
        ss << "Could not load export file for entity.\n"; 
        ss << "Attachment joints can't be specified.\n"; 
        ss << "  entity: " << entityClass->GetShortName(); 

        wxMessageBox(ss.str(), "Could not load export file", wxICON_INFORMATION); 
      }
    }

    PopulateJointIndices(entityData); 
    PopulateAnimClips(); 

    if(flags & DoNotLoadRemote)
    {
      return; 
    }

    LoadRemoteEntities(); 
  }

  void MultiAnimPanel::UnloadEntityData(EntityData* entityData)
  {
    entityData->m_EntityClass = NULL; 
    entityData->m_AnimSet = NULL; 
    entityData->m_Scene = NULL; 

    PopulateJointIndices(entityData); 
    PopulateAnimClips(); 
  }

  void MultiAnimPanel::TweakAttachment()
  {
    if(!m_Attachment)
    {
      return; 
    }

    RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 

    // update the non-entity-class based stuff
    //

    m_Attachment->m_Mode = (m_EnableJointAttach) ? RPC::AttachmentModes::JointToJoint : RPC::AttachmentModes::LockAnims; 
    m_Attachment->m_AttachedJointID = (m_AttachData.m_Joint ? m_AttachData.m_Joint->GetDynamicJointID() : -1); 
    m_Attachment->m_MainJointID     = (m_MainData.m_Joint ? m_MainData.m_Joint->GetDynamicJointID() : -1); 

    remote->SendTweakAttachment(m_Attachment); 
  }

  void MultiAnimPanel::LoadRemoteEntities()
  {
    SessionManager::GetInstance()->GiveViewerControl( m_Manager->GetAssetEditor() ); 
    RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 

    if(m_Attachment)
    {
      remote->RemoveAttachment(m_Attachment); 
      if(m_Attachment->m_MainEntity)
      {
        remote->RemoveEntityInstance(m_Attachment->m_MainEntity); 
      }
      if(m_Attachment->m_AttachedEntity)
      {
        remote->RemoveEntityInstance(m_Attachment->m_AttachedEntity); 
      }
      m_Attachment = NULL; 
    }

    m_Attachment = remote->TriggerMultiMoby(m_MainData.m_EntityClass, m_AttachData.m_EntityClass); 

    TweakAttachment(); 
  }

  void MultiAnimPanel::OnMainEntityChoice( wxCommandEvent& event )
  {
    wxBusyCursor busy; 
    LoadEntityData(&m_MainData, (Asset::EntityAsset*) event.GetClientData(), LoadRemote); 
    event.StopPropagation(); 
  }
  
  void MultiAnimPanel::OnAttachEntityChoice( wxCommandEvent& event )
  {
    wxBusyCursor busy; 
    LoadEntityData(&m_AttachData, (Asset::EntityAsset*) event.GetClientData(), LoadRemote); 
    event.StopPropagation(); 

  }

  void MultiAnimPanel::OnMainJointChoice( wxCommandEvent& event )
  {
    m_MainData.m_Joint = (Content::JointTransform*) event.GetClientData(); 

    TweakAttachment(); 

    event.StopPropagation(); 
  }

  void MultiAnimPanel::OnAttachJointChoice( wxCommandEvent& event )
  {
    m_AttachData.m_Joint = (Content::JointTransform*) event.GetClientData(); 

    TweakAttachment(); 

    event.StopPropagation(); 
  }

  void MultiAnimPanel::OnAnimClipChoice( wxCommandEvent& event )
  {
    event.StopPropagation(); 

    void* clientData = event.GetClientData(); 
    tuid currentTuid = TUID::Null; 
    if(clientData)
    {
      currentTuid = *(tuid*) clientData; 
    }

    if(currentTuid == m_AnimClip)
    {
      return; 
    }

    m_AnimClip = currentTuid; 

    LoadRemoteAnimation();
  }

  void MultiAnimPanel::OnAnimClipTextChoice( wxCommandEvent& event )
  { 
    event.StopPropagation(); 

    void* clientData = event.GetClientData(); 
    int   index = event.GetSelection(); 

    wxString value = m_AnimClipCombo->GetValue();  

    for(u32 i = 0; i < m_AnimClipCombo->GetCount(); ++i)
    {
      if(m_AnimClipCombo->GetString(i) == value)
      {
        void* clientData = m_AnimClipCombo->GetClientData(i); 
        tuid currentTuid = TUID::Null; 
        if(clientData)
        {
          currentTuid = *(tuid*) clientData; 
        }

        if(currentTuid == m_AnimClip)
        {
          return; 
        }

        m_AnimClip = currentTuid; 

        LoadRemoteAnimation(); 

        return; 
      }
    }
  }


  void MultiAnimPanel::LoadRemoteAnimation()
  {
    if(m_MainData.m_EntityClass && m_AnimClip)
    {
      RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 

      // get the clip name
      Asset::AnimationClipData data; 
      data.m_ArtFile = m_AnimClip; 
      std::string clipName = data.GetTitle(); 

      // create a clip command
      RPC::AnimLoadParam clipCommand; 
      memset(&clipCommand, 0, sizeof(clipCommand)); 

      clipCommand.m_Command = RPC::AnimCommands::Play; 
      strncpy(clipCommand.m_ClipName, clipName.c_str(), sizeof(clipCommand.m_ClipName) - 1); 

      // update the moby and send the clip command
      // 
      remote->m_LunaViewHost->LoadAnimClip(&clipCommand); 
    }
  }

  void MultiAnimPanel::OnButtonPlayPause( wxCommandEvent& event )
  { 
    LoadRemoteAnimation(); 

    event.StopPropagation(); 
  }

  void MultiAnimPanel::OnButtonSnapBegin( wxCommandEvent& event )
  { 
    if(m_MainData.m_EntityClass && m_AnimClip)
    {
      RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 

      RPC::AnimStateParam stateParam; 
      stateParam.m_Command = RPC::AnimCommands::SetPercent; 
      stateParam.m_Payload = 0.0f; 

      remote->m_LunaViewHost->TweakAnimClip(&stateParam); 

      m_TimeSlider->SetValue( m_TimeSlider->GetMin() ); 
    }

    event.StopPropagation(); 
  }

  void MultiAnimPanel::OnButtonSnapEnd( wxCommandEvent& event )
  { 
    if(m_MainData.m_EntityClass && m_AnimClip)
    {
      RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 

      RPC::AnimStateParam stateParam; 
      stateParam.m_Command = RPC::AnimCommands::SetPercent; 
      stateParam.m_Payload = 1.0f; 

      remote->m_LunaViewHost->TweakAnimClip(&stateParam); 

      m_TimeSlider->SetValue( m_TimeSlider->GetMax() ); 
    }

    event.StopPropagation(); 
  }

  void MultiAnimPanel::OnScroll( wxScrollEvent& event )
  {
    if(m_MainData.m_EntityClass && m_AnimClip)
    {
      RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 

      RPC::AnimStateParam stateParam; 
      stateParam.m_Command = RPC::AnimCommands::SetPercent; 
      stateParam.m_Payload = float(event.GetPosition()) / float(m_TimeSlider->GetMax()); 

      remote->m_LunaViewHost->TweakAnimClip(&stateParam); 
    }

    event.StopPropagation(); 
  }


  void MultiAnimPanel::OnButtonEnableJointAttach( wxCommandEvent& event )
  { 
    m_EnableJointAttach = !m_EnableJointAttach; 

    if(m_EnableJointAttach)
    {
      m_ButtonEnableJointAttach->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "joint_32.png" ) );
    }
    else
    {
      m_ButtonEnableJointAttach->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "animation_32.png" ) ); 
    }

    m_MainJointChoice->Enable( m_EnableJointAttach ); 
    m_AttachJointChoice->Enable( m_EnableJointAttach ); 

    PopulateAnimClips(); 

    TweakAttachment(); 

    event.Skip(); 
  }

  void MultiAnimPanel::OnButtonForceLoop( wxCommandEvent& event )
  { 
    event.Skip(); 
  }
  
  void MultiAnimPanel::OnButtonFrameEntities( wxCommandEvent& event )
  { 
    event.Skip(); 
  }
  
  void MultiAnimPanel::OnSingleAssetView(Luna::AssetViewArgs& args)
  {
    // we know that the attachment has been removed because asset viewing clears the scene
    if(m_Attachment)
    {
      m_Attachment = NULL; 
    }

    Asset::EntityAsset* mainEntityClass = NULL; 
    Asset::EntityAsset* attachEntityClass = NULL; 

    if(args.m_entity->GetEntityAsset()->GetEngineType() == Asset::EngineTypes::Moby)
    {
      // the viewer will give us back a different entity class than the asset
      // editor did before. so convert to the entity class from before
      // 
      Asset::EntityAsset* weirdEntityClass = args.m_entity->GetEntityAsset(); 

      for(size_t i = 0; i < m_EntityClasses.size(); ++i)
      {
        if( m_EntityClasses[i]->GetFileID() == weirdEntityClass->GetFileID() )
        {
          mainEntityClass = m_EntityClasses[i]; 
          break; 
        }
      }

      // we should never get a case where mainEntityClass is not found
      NOC_ASSERT(mainEntityClass); 

      if(mainEntityClass)
      {
        Luna::AttachmentPtr attach = new Luna::Attachment; 
        attach->m_MainEntity = args.m_entity; 
        attach->m_Mode = (m_EnableJointAttach ? RPC::AttachmentModes::JointToJoint : RPC::AttachmentModes::LockAnims); 

        RemoteAsset* remote = m_Manager->GetAssetEditor()->GetRemoteInterface(); 
        remote->AddAttachment(attach); 

        m_Attachment = attach; 
      }
    }

    // update the ui and the extra data we keep for state
    //

    LoadEntityData(&m_MainData, mainEntityClass, DoNotLoadRemote); 
    LoadEntityData(&m_AttachData, attachEntityClass, DoNotLoadRemote); 

    UpdateEntityDropdown(m_MainEntityChoice, mainEntityClass); 
    UpdateEntityDropdown(m_AttachEntityChoice, attachEntityClass); 
    PopulateAnimClips(); 
    PopulateJointIndices(&m_MainData); 
    PopulateJointIndices(&m_AttachData); 
  }

}
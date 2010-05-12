#pragma once

#include "API.h"

#include "Common/Types.h"
#include "Common/memory/SmartPtr.h"
#include "TUID/TUID.h"

#include "Content/Scene.h"
#include "Asset/Entity.h"
#include "Asset/LevelAsset.h"

#include "rpc/interfaces/rpc_lunaview.h"

#include "UIToolKit/WorkerThreadDialog.h"

namespace Asset
{
  class AssetClass;
}

namespace Math
{
  class Matrix4;
}

namespace Content
{
  class Light;
}

namespace AssetBuilder
{
  class AssetBuiltArgs;
  typedef Nocturnal::SmartPtr< AssetBuiltArgs > AssetBuiltArgsPtr;
}

namespace Luna
{
  class Light;
  class Editor;
  struct RuntimeConnectionStatusArgs;

  ///////////////////////////////////////////////////////////////////////////
  // Dialog to show while processing an object.
  //
  class ObjectUpdateDialog : public UIToolKit::WorkerThreadDialog
  {
  public:
    ObjectUpdateDialog( wxWindow* parent, const std::string& title, const std::string& msg )
      : UIToolKit::WorkerThreadDialog( parent, wxID_ANY, title.c_str(), wxDefaultPosition, wxSize( 500, 100 ), wxCAPTION )
    {
      wxBoxSizer* sizerMain;
      sizerMain = new wxBoxSizer( wxVERTICAL );

      wxTextCtrl* textbox = new wxTextCtrl( this, wxID_ANY, msg.c_str(), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER );
      textbox->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );

      sizerMain->Add( textbox, 1, wxALL|wxEXPAND, 5 );

      SetSizer( sizerMain );
      Layout();
    }
  };

  struct TaskFinishedArgs;

  // Attachment, really should be in a different file

  class LUNA_LIVE_API Attachment : public Nocturnal::RefCountBase<Attachment>
  {
  public: 
    RPC::AttachmentMode m_Mode; 
    Asset::EntityPtr    m_MainEntity; 
    Asset::EntityPtr    m_AttachedEntity; 
    i32                 m_MainJointID; 
    i32                 m_AttachedJointID; 

    Attachment(); 
  }; 

  typedef Nocturnal::SmartPtr<Attachment> AttachmentPtr; 
  typedef std::vector<AttachmentPtr> V_Attachment; 

  // AssetView event support, so we can know when a single asset has been viewed. 
  // 
  struct LUNA_LIVE_API AssetViewArgs
  {
    Asset::Entity* m_entity; 
  }; 

  typedef Nocturnal::Signature<void, AssetViewArgs&> AssetViewSignature; 

  // RemoteEditor

  class LUNA_LIVE_API RemoteEditor
  {
  public:
    RemoteEditor( Editor* editor );
    virtual ~RemoteEditor();

    virtual void Enable( bool enable );
    virtual bool IsEnabled() { return m_Enabled; }

    void OnConnectionStatusChanged( const RuntimeConnectionStatusArgs& args );

    virtual void AddEntityInstance( Asset::Entity* entity );
    virtual void SendEntityInstance( Asset::Entity* entity );
    virtual void RemoveEntityInstance( Asset::Entity* entity );

    virtual void AddAttachment( AttachmentPtr attachment ); 
    virtual void RemoveAttachment( AttachmentPtr attachment ); 

    virtual void SendCreateAttachment( AttachmentPtr attachment ); 
    virtual void SendTweakAttachment( AttachmentPtr attachment ); 
    virtual void SendDeleteAttachment( AttachmentPtr attachment ); 

    virtual void AddLight( Content::Light* light );
    virtual void SendLight( Content::Light* light );
    virtual void RemoveLight( Content::Light* light );
    virtual void SetLightParams( Content::Light* light );
    virtual void SetLightAnimation( Content::Light* light );
    virtual void TransformLight( Content::Light* light );
    virtual void SetLinkedBakedLights( Luna::Light* light ){}
    virtual void SetLightingEnvironment( std::string& filePath );

    virtual void TweakShaderGroup( const Asset::Entity* entity );

    virtual void FrameEntityInstance( tuid assetId, const UniqueID::TUID& instanceId );

    virtual u8 GetAssetType( const Asset::AssetClass* assetClass );
    virtual void ReloadAsset( tuid assetId );
    virtual void ReloadLastBuiltAssets();

    void SetLevelWeatherAttributes(const Asset::LevelAssetPtr& level, u32 flags); 

    void SetLevelAttributes(const Asset::LevelAssetPtr& level); 
    void ClearLevelAttributes();

    virtual void SendScene();
    virtual void ClearScene();

    void BuildFinished( const TaskFinishedArgs& taskArgs );
    virtual void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args );
    virtual void ViewAsset( tuid assetId );

    virtual void SetMode( RPC::LunaViewMode mode );
    void SendMode();

    void SendMobyAnimJointMats( Asset::Entity* entity );

    virtual void SetInstanceCollision( const Asset::Entity* entity );

    virtual Editor* GetEditor() { return m_Editor; }

    void AddAssetViewListener(const AssetViewSignature::Delegate& listener)
    {
      m_AssetViewEvent.Add(listener); 
    }

    void RemoveAssetViewListener(const AssetViewSignature::Delegate& listener)
    {
      m_AssetViewEvent.Remove(listener); 
    }

  protected:
    void CreateMobyPayload( Asset::Entity* entity, u8*& payload, u32& payload_size );

  protected:

    bool                  m_Enabled;
    Editor*               m_Editor;
    Content::Scene        m_Scene;
    V_Attachment          m_Attachments; 
    Asset::EntityPtr      m_SelectedEntity;
    RPC::LunaViewMode     m_Mode;
    S_tuid                m_AssetsToReload;

    // this event will be raised when a single asset is viewed
    AssetViewSignature::Event m_AssetViewEvent; 

  };
}

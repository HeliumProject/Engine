#pragma once

#include "CharacterDocument.h" 
#include "Content/Scene.h" 
#include "Content/JointAttribute.h" 

#include "Asset/EntityAsset.h" 
#include "Asset/SkeletonAsset.h" 
#include "Editor/DocumentManager.h"

namespace Content
{
  class JointOrdering; 
  typedef Nocturnal::SmartPtr<JointOrdering> JointOrderingPtr; 
}

namespace RPC
{
  struct PhysicsJointParam;
}

namespace Luna
{
  class CharacterEditor; 

  struct EmptyArgs
  {

  }; 

  struct JointSelectionArgs
  {
    const Content::JointTransformPtr prevJoint; 
    const Content::JointTransformPtr nextJoint; 
  }; 

  struct AttributeSelectionArgs
  { 
    const Attribute::AttributePtr prevAttr; 
    const Attribute::AttributePtr nextAttr; 
  }; 

  struct StatusChangeArgs
  {
    StatusChangeArgs(std::string& string) 
      : m_Message(string)
    {

    }
  
    std::string m_Message; 
  }; 

  typedef Nocturnal::Signature<void, EmptyArgs&> GenericSignature; 
  typedef GenericSignature::Delegate GenericDelegate; 
  typedef GenericSignature::Event    GenericEvent; 

  typedef Nocturnal::Signature<void, StatusChangeArgs&> StatusChangeSignature; 
  typedef StatusChangeSignature::Delegate StatusChangeDelegate; 
  typedef StatusChangeSignature::Event    StatusChangeEvent; 

  typedef Nocturnal::Signature<void, JointSelectionArgs&> JointSelectionSignature; 
  typedef JointSelectionSignature::Delegate JointSelectionDelegate; 
  typedef JointSelectionSignature::Event    JointSelectionEvent; 

  typedef Nocturnal::Signature<void, AttributeSelectionArgs&> AttributeSelectionSignature; 
  typedef AttributeSelectionSignature::Delegate AttributeSelectionDelegate; 
  typedef AttributeSelectionSignature::Event    AttributeSelectionEvent; 
  

  class CharacterManager : public DocumentManager
  {
  public: 
    CharacterManager(CharacterEditor*); 
    ~CharacterManager(); 

    const CharacterDocumentPtr&   GetFile(); 
    const Content::ScenePtr&       GetScene(); 
    const Content::ScenePtr&       GetCollisionScene();
    const Asset::SkeletonAssetPtr& GetSkeletonAsset(); 

    virtual DocumentPtr OpenPath(const std::string& path, std::string& error) NOC_OVERRIDE;
    virtual bool         Save(DocumentPtr file, std::string& error) NOC_OVERRIDE;

    bool  Save( std::string& error );
    bool  Close(); 

    void  ClearSelections(); 

    void  SetSelectedJoint(const Content::JointTransformPtr& node); 
    const Content::JointTransformPtr& GetSelectedJoint() const; 

    void  SetSelectedAttribute(const Content::JointAttributePtr& attr); 
    const Content::JointAttributePtr& GetSelectedAttribute() const; 

    void  AddPhysicsAttribute(const Content::JointTransformPtr& node); 
    void  RemovePhysicsAttribute(const Content::JointTransformPtr& node); 

    void  AddIKAttribute(const Content::JointTransformPtr& node); 
    void  RemoveIKAttribute(const Content::JointTransformPtr& node); 

    void  ImportFromExportData(); 
    void  RefreshExportData(); 
    void  RecoverAttributesByName();
    void  CopyAllPhysicsAttributesToIK(); 
    void  CopyOnePhysicsAttributeToIK(); 

		bool  AddLooseAttachChain(const Content::LooseAttachChainPtr& chain); 
    void  RemoveLooseAttachChain(const Content::LooseAttachChainPtr& chain); 
    void  AddLooseAttachAttribute(const Content::JointTransformPtr& node); 
    void  RemoveLooseAttachAttribute(const Content::JointTransformPtr& node); 
    void  AddLooseAttachCollisionAttribute(const Content::JointTransformPtr& node); 
    void  RemoveLooseAttachCollisionAttribute(const Content::JointTransformPtr& node); 

    // called by RemoteCharacter
    void RemotePhysicsJointUpdate(RPC::PhysicsJointParam* param); 

    // event handling prototypes... 

    GenericEvent*             OpenedEvent()    { return &m_Opened; }
    GenericEvent*             ClosedEvent()    { return &m_Closed; }
    GenericEvent*             ForceUpdateEvent() { return &m_ForceUpdate; }
    
    GenericEvent*             LooseAttachmentAddedEvent()   { return &m_LooseAttachmentAdded; }
    GenericEvent*             LooseAttachmentRemovedEvent() { return &m_LooseAttachmentRemoved; }

    JointSelectionEvent*      JointSelectedEvent()     { return &m_JointSelected;     }
    AttributeSelectionEvent*  AttributeSelectedEvent() { return &m_AttributeSelected; }
    StatusChangeEvent*        StatusChangedEvent()     { return &m_StatusChanged;     }

    const Asset::EntityAssetPtr& GetMobyClass()
    {
      return m_MobyClass; 
    }

  private:     
    CharacterEditor*          m_Editor; 
    CharacterDocumentPtr      m_File; 
    Content::ScenePtr          m_CollisionScene;
    Content::ScenePtr          m_LocalScene; 
    Content::ScenePtr          m_MasterScene; 
    Content::JointOrderingPtr  m_JointOrdering; 
    Asset::EntityAssetPtr      m_MobyClass; 
    Asset::SkeletonAssetPtr    m_SkeletonAsset; 
    
    Content::JointTransformPtr m_SelectedJoint; 
    Content::JointAttributePtr m_SelectedAttribute; 

    bool                       m_DispatchingRPC; 
    bool                       m_HaveChanges; 

    // events
    // 
    GenericEvent               m_Opened; 
    GenericEvent               m_Closed; 
    GenericEvent               m_ForceUpdate; 
    GenericEvent               m_LooseAttachmentAdded; 
    GenericEvent               m_LooseAttachmentRemoved; 
    JointSelectionEvent        m_JointSelected; 
    AttributeSelectionEvent    m_AttributeSelected; 
    StatusChangeEvent          m_StatusChanged;

    void OnAttributeChanged(const Reflect::ElementChangeArgs& args); 
    void OnAssetChanged(const Reflect::ElementChangeArgs& args); 
    void OnDocumentClosed( const DocumentChangedArgs& args );

  }; 

}

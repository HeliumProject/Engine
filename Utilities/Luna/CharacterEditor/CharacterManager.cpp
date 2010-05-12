#include "Precompile.h" 
#include "CharacterManager.h" 
#include "CharacterEditor.h" 

#include "Attribute/AttributeHandle.h" 

#include "Asset/EntityAsset.h" 
#include "Asset/SkeletonAsset.h" 
#include "Asset/ArtFileAttribute.h" 
#include "Asset/SkeletonFileAttribute.h" 

#include "Content/JointAttribute.h" 
#include "Content/SkeletonAttributeSet.h" 
#include "Content/PhysicsJointAttribute.h" 
#include "Content/IKJointAttribute.h"
#include "Content/LooseAttachStartAttribute.h" 
#include "Content/LooseAttachEndAttribute.h" 
#include "Content/LooseAttachJointAttribute.h" 
#include "Content/LooseAttachCollisionAttribute.h" 

#include "Finder/AssetSpecs.h" 
#include "Finder/ContentSpecs.h"

#include "File/Manager.h" 
#include "FileSystem/FileSystem.h" 
#include "FileBrowser/FileBrowser.h" 
#include "BuilderUtil/SharedAnimationHelper.h"

#include "Live/RuntimeConnection.h" 
#include "rpc/interfaces/rpc_lunaview_host.h"
#include "rpc/interfaces/rpc_lunaview.h"

#include <math.h> 

using namespace Asset; 
using namespace Content; 
using namespace Attribute;

inline f32 DegToRad(f32 rad)
{
  return rad * (M_PI / 180.0f); 
}

inline f32 RadToDeg(f32 deg)
{
  return deg * (180.0f / M_PI); 
}

#define ERROR_MSG_STYLE (wxOK | wxCENTER | wxICON_ERROR)

namespace Luna
{
  CharacterManager::CharacterManager(CharacterEditor* editor)
    : DocumentManager( editor )
    , m_Editor(editor)
    , m_DispatchingRPC(false)
    , m_HaveChanges(false)
  {
    NOC_ASSERT(m_Editor); 
  }

  CharacterManager::~CharacterManager()
  {
  }

  //-----------------------------------------------------------------
  // a few simple Getters
  // 

  const CharacterDocumentPtr& CharacterManager::GetFile()
  {
    return m_File; 
  }

  const Content::ScenePtr& CharacterManager::GetScene()
  {
    return m_MasterScene; 
  }

  const Asset::SkeletonAssetPtr& CharacterManager::GetSkeletonAsset()
  {
    return m_SkeletonAsset; 
  }

  const Content::ScenePtr& CharacterManager::GetCollisionScene()
  {
    return m_CollisionScene;
  }

  //-----------------------------------------------------------------
  // more interesting implementation
  // 

  void  CharacterManager::RemotePhysicsJointUpdate(RPC::PhysicsJointParam* param)
  {
    // early out if we don't have any data
    // 
    if(!m_MasterScene)
    {
      return; 
    }

    UniqueID::TUID id = param->m_ID; 

    // the remote editor is operating on the localScene
    // we are operating on the master scene
    // 
    // convert to the master joint id, which may fail. 
    // 
    try
    {
      id = m_JointOrdering->GetMasterJoint(id); 
    }
    catch( const Nocturnal::Exception& )
    {
      return; 
    }

    Content::JointTransformPtr joint = m_MasterScene->Get<Content::JointTransform>(id); 

    if(!joint)
      return; 

    SetSelectedJoint(joint); 

    // now convert the settings we have into the tools side of things
    // 


    if ( param->m_Type == RPC::PhysicsJointTypes::Procedural )
    {
      PhysicsJointAttributePtr physics = joint->GetAttribute<PhysicsJointAttribute>(); 

      if(!physics)
      {
        return; 
      }

      m_DispatchingRPC = true; 

      physics->m_HardLimitMin.x = RadToDeg(param->m_Limits[0].m_HardMin); 
      physics->m_HardLimitMax.x = RadToDeg(param->m_Limits[0].m_HardMax); 
      physics->m_SoftLimitMin.x = RadToDeg(param->m_Limits[0].m_SoftMin); 
      physics->m_SoftLimitMax.x = RadToDeg(param->m_Limits[0].m_SoftMax); 

      physics->m_HardLimitMin.y = RadToDeg(param->m_Limits[1].m_HardMin); 
      physics->m_HardLimitMax.y = RadToDeg(param->m_Limits[1].m_HardMax); 
      physics->m_SoftLimitMin.y = RadToDeg(param->m_Limits[1].m_SoftMin); 
      physics->m_SoftLimitMax.y = RadToDeg(param->m_Limits[1].m_SoftMax); 

      physics->m_HardLimitMin.z = RadToDeg(param->m_Limits[2].m_HardMin); 
      physics->m_HardLimitMax.z = RadToDeg(param->m_Limits[2].m_HardMax); 
      physics->m_SoftLimitMin.z = RadToDeg(param->m_Limits[2].m_SoftMin); 
      physics->m_SoftLimitMax.z = RadToDeg(param->m_Limits[2].m_SoftMax); 

      physics->m_SpringConst       = param->m_SpringConstant; 
      physics->m_SpringDamp        = param->m_SpringDamping; 
      physics->m_CoefRest          = param->m_SoftLimitCoR; 
      physics->m_MinVelCoefRest    = param->m_SoftLimitCoR_EnableVelocity; 

      physics->m_EnableLimitX = !(param->m_EnableX == 0); 
      physics->m_EnableLimitY = !(param->m_EnableY == 0); 
      physics->m_EnableLimitZ = !(param->m_EnableZ == 0); 

      physics->RaiseChanged(NULL); 

      m_DispatchingRPC = false; 
    }
    else if ( param->m_Type == RPC::PhysicsJointTypes::IK )
    {
      IKJointAttributePtr ik = joint->GetAttribute<IKJointAttribute>(); 

      if(!ik)
      {
        return; 
      }

      m_DispatchingRPC = true; 

      ik->m_HardLimitMin.x = RadToDeg(param->m_Limits[0].m_HardMin); 
      ik->m_HardLimitMax.x = RadToDeg(param->m_Limits[0].m_HardMax); 

      ik->m_HardLimitMin.y = RadToDeg(param->m_Limits[1].m_HardMin); 
      ik->m_HardLimitMax.y = RadToDeg(param->m_Limits[1].m_HardMax); 

      ik->m_HardLimitMin.z = RadToDeg(param->m_Limits[2].m_HardMin); 
      ik->m_HardLimitMax.z = RadToDeg(param->m_Limits[2].m_HardMax); 

      ik->m_CoefRest          = param->m_SoftLimitCoR; 
      ik->m_MinVelCoefRest    = param->m_SoftLimitCoR_EnableVelocity; 

      ik->m_EnableLimitX = !(param->m_EnableX == 0); 
      ik->m_EnableLimitY = !(param->m_EnableY == 0); 
      ik->m_EnableLimitZ = !(param->m_EnableZ == 0); 

      ik->RaiseChanged(NULL); 

      m_DispatchingRPC = false; 
    }
  }

  void CharacterManager::OnAssetChanged(const Reflect::ElementChangeArgs& args)
  {
    m_File->SetModified(true); 
  }

  void CharacterManager::OnAttributeChanged(const Reflect::ElementChangeArgs& args)
  {
    m_File->SetModified(true); 

    // if we got this from RPC, we can just get out of here
    // otherwise we are going to send it over the network to the PS3
    // 
    if(m_DispatchingRPC)
    {
      return; 
    }

    Reflect::Element*        raw     = const_cast<Reflect::Element*>(args.m_Element); 
    PhysicsJointAttributePtr physics = Reflect::ObjectCast<PhysicsJointAttribute>(raw);
    IKJointAttributePtr      ik      = Reflect::ObjectCast<IKJointAttribute>(raw);
    RPC::PhysicsJointParam   param;

    if(physics)
    {
      JointTransformPtr node = m_SelectedJoint; 
      NOC_ASSERT(node); 

      // convert to local UID for viewer
      param.m_ID = m_JointOrdering->GetLocalJoint(node->m_ID); 

      param.m_Type = RPC::PhysicsJointTypes::Procedural;

      param.m_Limits[0].m_HardMin  = DegToRad(physics->m_HardLimitMin.x); 
      param.m_Limits[0].m_HardMax  = DegToRad(physics->m_HardLimitMax.x); 
      param.m_Limits[0].m_SoftMin  = DegToRad(physics->m_SoftLimitMin.x); 
      param.m_Limits[0].m_SoftMax  = DegToRad(physics->m_SoftLimitMax.x); 

      param.m_Limits[1].m_HardMin  = DegToRad(physics->m_HardLimitMin.y); 
      param.m_Limits[1].m_HardMax  = DegToRad(physics->m_HardLimitMax.y); 
      param.m_Limits[1].m_SoftMin  = DegToRad(physics->m_SoftLimitMin.y); 
      param.m_Limits[1].m_SoftMax  = DegToRad(physics->m_SoftLimitMax.y); 

      param.m_Limits[2].m_HardMin  = DegToRad(physics->m_HardLimitMin.z); 
      param.m_Limits[2].m_HardMax  = DegToRad(physics->m_HardLimitMax.z); 
      param.m_Limits[2].m_SoftMin  = DegToRad(physics->m_SoftLimitMin.z); 
      param.m_Limits[2].m_SoftMax  = DegToRad(physics->m_SoftLimitMax.z); 

      param.m_SpringConstant              = physics->m_SpringConst; 
      param.m_SpringDamping               = physics->m_SpringDamp; 
      param.m_SoftLimitCoR                = physics->m_CoefRest; 
      param.m_SoftLimitCoR_EnableVelocity = physics->m_MinVelCoefRest; 

      param.m_EnableX = physics->m_EnableLimitX; 
      param.m_EnableY = physics->m_EnableLimitY; 
      param.m_EnableZ = physics->m_EnableLimitZ; 

      // send it over the network... 
      // 
      Luna::RuntimeConnection::GetRemoteLevelView()->SetPhysicsJointParams(&param); 
    }

    if(ik)
    {
      JointTransformPtr node = m_SelectedJoint; 
      NOC_ASSERT(node); 

      // convert to local UID for viewer
      param.m_ID = m_JointOrdering->GetLocalJoint(node->m_ID); 

      param.m_Type = RPC::PhysicsJointTypes::IK;

      param.m_Limits[0].m_HardMin  = DegToRad(ik->m_HardLimitMin.x); 
      param.m_Limits[0].m_HardMax  = DegToRad(ik->m_HardLimitMax.x); 

      param.m_Limits[1].m_HardMin  = DegToRad(ik->m_HardLimitMin.y); 
      param.m_Limits[1].m_HardMax  = DegToRad(ik->m_HardLimitMax.y); 

      param.m_Limits[2].m_HardMin  = DegToRad(ik->m_HardLimitMin.z); 
      param.m_Limits[2].m_HardMax  = DegToRad(ik->m_HardLimitMax.z); 

      param.m_SoftLimitCoR                = ik->m_CoefRest; 
      param.m_SoftLimitCoR_EnableVelocity = ik->m_MinVelCoefRest; 

      param.m_EnableX = ik->m_EnableLimitX; 
      param.m_EnableY = ik->m_EnableLimitY; 
      param.m_EnableZ = ik->m_EnableLimitZ; 

      // send it over the network... 
      // 
      Luna::RuntimeConnection::GetRemoteLevelView()->SetPhysicsJointParams(&param); 
    }
  }

  void CharacterManager::ClearSelections()
  {
    // this is written in such a way that we force the 
    // events to be sent. general usage is to check that we are 
    // actually changing something before we send the events..

    if(m_SelectedAttribute)
    {
      m_SelectedAttribute->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterManager::OnAttributeChanged)); 
    }

    AttributeSelectionArgs attrArgs  = { m_SelectedAttribute, NULL }; 
    JointSelectionArgs     jointArgs = { m_SelectedJoint, NULL }; 

    m_SelectedAttribute = NULL; 
    m_SelectedJoint = NULL; 

    m_AttributeSelected.Raise(attrArgs); 
    m_JointSelected.Raise(jointArgs); 

  }

  void CharacterManager::SetSelectedJoint(const Content::JointTransformPtr& node)
  {
    if(node != m_SelectedJoint)
    {
      // invalidate the selected attribute before we set the selected
      // joint. this allows our listeners to respond to the joint selection
      // by selecting an attribute on the joint
      // 
      SetSelectedAttribute( JointAttributePtr(NULL) ); 

      JointSelectionArgs args = { m_SelectedJoint, node }; 
      m_SelectedJoint = node; 

      // handle remote selection
      if(m_SelectedJoint)
      {
        RPC::ILunaViewHostRemote* remote = RuntimeConnection::GetRemoteLevelView();
        RPC::SelectJointParam joint; 

        joint.m_ID = m_JointOrdering->GetLocalJoint(m_SelectedJoint->m_ID); 

        // TODO: how do I figure out what kind of type to pass?
        joint.m_Type = RPC::PhysicsJointTypes::Unknown;
        remote->SelectJoint(&joint); 
      }

      // now update GUI slow and sad 
      m_JointSelected.Raise( args ); 

    }
  }

  const Content::JointTransformPtr& CharacterManager::GetSelectedJoint() const
  {
    return m_SelectedJoint; 
  }

  void  CharacterManager::SetSelectedAttribute(const Content::JointAttributePtr& attr)
  {
    if(attr != m_SelectedAttribute)
    {
      const Content::JointTransformPtr& joint = GetSelectedJoint();

      if ( joint )
      {
        RPC::SelectJointParam param;
        param.m_ID = joint->m_ID;
        param.m_Type = RPC::PhysicsJointTypes::Unknown;

        if ( attr )
        {
          if ( attr->GetType() == Reflect::GetType<Content::PhysicsJointAttribute>() )
          {
            param.m_Type = RPC::PhysicsJointTypes::Procedural;
          }
          else if ( attr->GetType() == Reflect::GetType<Content::IKJointAttribute>() )
          {
            param.m_Type = RPC::PhysicsJointTypes::IK;
          }
        }

        RPC::ILunaViewHostRemote* remote = RuntimeConnection::GetRemoteLevelView();
        remote->SelectJoint( &param );
      }

      if(m_SelectedAttribute)
      {
        m_SelectedAttribute->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterManager::OnAttributeChanged)); 
      }

      AttributeSelectionArgs args = { m_SelectedAttribute, attr }; 
      m_SelectedAttribute = attr; 

      if(m_SelectedAttribute)
      {
        m_SelectedAttribute->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterManager::OnAttributeChanged)); 
      }

      m_AttributeSelected.Raise( args ); 


    }
  }

  const Content::JointAttributePtr& CharacterManager::GetSelectedAttribute() const
  {
    return m_SelectedAttribute; 
  }

  DocumentPtr CharacterManager::OpenPath( const std::string& path, std::string& error )
  {
    wxBusyCursor cursor; 

    // just a little note: i use all kinds of temporary stack variables 
    // (SmartPtrs) to keep track of the things i create. when i'm all done, 
    // and everything has suceeded, THEN i assign the temps to my member variables
    //
    // that way if i bail out in the middle, the CharacterManager state is still consistent
    // 

    tuid assetId = File::GlobalManager().Open( path );
    if ( assetId == TUID::Null )
    {
      error = "The file path (";
      error += path.c_str();
      error += ") does not exist in the resolver and the system failed to open it.";
      return NULL;
    }

    EntityAssetPtr entityClass = Reflect::ObjectCast<EntityAsset>( AssetClass::FindAssetClass( assetId, false ) ); 

    if(!entityClass)
    {
      error = path + " is not a valid entity."; 
      return NULL; 
    }

    if(entityClass->GetEngineType() != EngineTypes::Moby)
    {
      error = path + " is not a moby."; 
      return NULL; 
    }

    AttributeViewer<ArtFileAttribute>      modelFile(entityClass); 
    AttributeViewer<SkeletonFileAttribute> skeletonFile(entityClass); 

    if(!modelFile.Valid())
    {
      error = path + " does not have an art file. Add one in the Asset Editor."; 
      return NULL; 
    }

    if(!skeletonFile.Valid())
    {
      error = path + " does not have a skeleton file attribute. Add one in the Asset Editor."; 
      return NULL; 
    }

    std::string modelPath          = modelFile->GetFilePath(); 
    std::string skeletonPath       = skeletonFile->GetFilePath(); 

    if ( skeletonPath.empty() )
    {
      error = "The skeleton file attribute does not have a valid path to a skeleton file.  Fix this in the Asset Editor.";
      return NULL;
    }

    SkeletonAssetPtr skeletonClass;
    try
    {
      skeletonClass = Reflect::Archive::FromFile<SkeletonAsset>(skeletonPath); 
    }
    catch ( const Nocturnal::Exception& e )
    {
      error = "Failed to read skeleton file " + skeletonPath + ": " + e.Get();
      return NULL;
    }

    if ( !skeletonClass )
    {
      error = "Could not read skeleton file " + skeletonPath; 
      return NULL; 
    }

    AttributeViewer<ArtFileAttribute>    masterFile(skeletonClass); 

    if(!masterFile.Valid())
    {
      error = "Skeleton file does not reference an art file.  Fix this in the Asset Editor."; 
      return NULL; 
    }

    std::string masterPath       = masterFile->GetFilePath(); 

    std::string localRiggedPath  = FinderSpecs::Content::RIGGED_DECORATION.GetExportFile(modelPath); 
    std::string masterRiggedPath = FinderSpecs::Content::RIGGED_DECORATION.GetExportFile(masterPath); 
    std::string collisionPath    = FinderSpecs::Content::COLLISION_DECORATION.GetExportFile(masterPath);


    if(!FileSystem::Exists(localRiggedPath))
    {
      error = "Rigged file " + localRiggedPath + " does not exist. Please export it."; 
      return NULL; 
    }

    if(!FileSystem::Exists(masterRiggedPath))
    {
      error = "Rigged file " + masterRiggedPath + " does not exist. Please export it."; 
      return NULL; 
    }

    // use the skeleton path as the actual file, so that all the 
    // managed file / perforce integration works properly. 
    // 
    CharacterDocumentPtr document = new CharacterDocument(skeletonPath); 

    document->m_EntityPath        = path; 
    document->m_LocalModelPath    = modelPath; 
    document->m_MasterModelPath   = masterPath; 
    document->m_LocalRiggedPath   = localRiggedPath; 
    document->m_MasterRiggedPath  = masterRiggedPath; 
    document->m_SkeletonPath      = skeletonPath; 
    document->m_CollisionPath     = collisionPath;

    if ( !QueryOpen( document ) )
    {
      error = "Open operation cancelled.";
      return NULL;
    }

    // load necessary scene files. 

    Content::ScenePtr         collisionScene;
    Content::ScenePtr         localScene; 
    Content::ScenePtr         masterScene; 
    Content::JointOrderingPtr jointOrdering; 

    m_StatusChanged.Raise( StatusChangeArgs("Loading " + document->m_LocalRiggedPath) ); 

    localScene = new Content::Scene( document->m_LocalRiggedPath ); 
    localScene->Update(); 

    // only load the master scene from the disk if we have a different filepath
    // otherwise, we'll just use the same data
    // 
    if(document->m_LocalRiggedPath == document->m_MasterRiggedPath)
    {
      masterScene   = localScene; 
      jointOrdering = localScene->GetJointOrdering(); 
    }
    else
    {
      m_StatusChanged.Raise( StatusChangeArgs("Loading " + document->m_MasterRiggedPath) ); 

      masterScene = new Content::Scene( document->m_MasterRiggedPath ); 
      masterScene->Update(); 

      jointOrdering = BuilderUtil::CalculateJointOrdering(*localScene, *masterScene); 
    }

    collisionScene = new Content::Scene;

    if(FileSystem::Exists(document->m_CollisionPath ))
    {
      collisionScene->Load( document->m_CollisionPath );
      collisionScene->Update();
    }

    // clear status now. not entirely clear what we should do overall, 
    // since i have so many "exit" points of this function that it's hard to say
    // Raise("failed")
    // 
    m_StatusChanged.Raise( StatusChangeArgs( std::string("") ) ); 

    if(!jointOrdering.ReferencesObject())
    {
      error = "Joints do not match across models:\n\n" + 
        document->m_LocalRiggedPath + "\n" + 
        document->m_MasterRiggedPath + "\n\n"
        "Perhaps you need to re-export?";
      return NULL; 
    }

    // we give it the NULL joint ordering because we store the joint attributes with 'master' uids
    skeletonClass->InitSession(*masterScene, NULL); 
    bool ok = skeletonClass->Sync(); 

    if(!ok)
    {
      error = "There were some issues matching up the Skeleton Asset to the exported asset.\n"
        "Please see the output window for more information!"; 

      document->SetModified(true); 
    }

    // update our member storage to point to the local values
    // this offically sets up our state. 

    m_File            = document; 
    m_LocalScene      = localScene; 
    m_MasterScene     = masterScene; 
    m_CollisionScene  = collisionScene;
    m_MobyClass       = entityClass; 
    m_SkeletonAsset   = skeletonClass; 
    m_JointOrdering   = jointOrdering;  

    // is the order between the raise and the clear important? 
    // maybe maybe not, could use some thinking on it. 

    m_SkeletonAsset->AddChangedListener( Reflect::ElementChangeSignature::Delegate(this, &CharacterManager::OnAssetChanged )); 

    m_Opened.Raise(EmptyArgs()); 

    ClearSelections(); 

    m_File->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &CharacterManager::OnDocumentClosed ) ); 
    AddDocument( m_File );

    return m_File; 
  }

  bool CharacterManager::Save(DocumentPtr file, std::string& error)
  {
    if(!m_MasterScene || !m_SkeletonAsset)
    {
      return false; 
    }

    CharacterDocumentPtr document = Reflect::ObjectCast<CharacterDocument>(file); 
    m_SkeletonAsset->GetSkeletonAttributes()->ExtractFrom(*m_MasterScene); 

    if ( !IsCheckedOut( document ) )
    {
      if ( !CheckOut( document ) )
      {
        error = "Failed to check out " + document->GetFilePath() + ".";
        return false;
      }
    }

    try
    {
      Reflect::Archive::ToFile(m_SkeletonAsset, document->m_SkeletonPath);
    }
    catch ( Nocturnal::Exception& ex )
    {
      error = "Failed to save " + document->m_SkeletonPath + ": " + ex.Get();
      return false;
    }

    m_File->SetModified(false); 
    return true; 
  }

  bool CharacterManager::Save( std::string& error )
  {
    if ( !m_File )
    {
      return false;
    }
    return Save( m_File, error );
  }

  bool CharacterManager::Close()
  {
    if(!m_File)
    {
      return true; 
    }

    return CloseDocument( m_File );
  }

  void CharacterManager::ImportFromExportData()
  {
    if(!m_MasterScene)
    {
      return; 
    }

    Content::ScenePtr& scene = m_MasterScene; 

    for(u32 j = 0; j < scene->m_Joints.size(); j++)
    {
      const JointTransformPtr& joint = scene->m_Joints[j]; 

      // clear out the existing attribute (if any); 
      joint->RemoveAttribute( Reflect::GetType<Content::PhysicsJointAttribute>() ); 

      if(joint->IsPhysicsJoint())
      {
        const PhysicsJointPtr& physicsJoint = Reflect::ObjectCast<PhysicsJoint>(joint); 
        PhysicsJointAttributePtr attribute = new PhysicsJointAttribute(physicsJoint); 
        physicsJoint->SetAttribute(attribute); 
      }

      m_File->SetModified(true); 

    }

    ClearSelections(); 

    m_ForceUpdate.Raise( EmptyArgs() ); 
  } 

  void CharacterManager::RefreshExportData()
  {


  }

  void CharacterManager::RecoverAttributesByName()
  {
    if(!m_MasterScene)
    {
      return; 
    }

    m_SkeletonAsset->GetSkeletonAttributes()->RecoverAttributesByJointName(*m_MasterScene); 
    bool ok = m_SkeletonAsset->Sync(); 

    if(!ok)
    {
      wxMessageBox("There were some issues matching up the Skeleton Asset to the exported asset.\n"
        "Please see the output window for more information!", 
        "Problems Resolving", ERROR_MSG_STYLE, m_Editor); 
    }

    m_File->SetModified(true); 
    ClearSelections(); 
    m_ForceUpdate.Raise( EmptyArgs() ); 

  }

  void CharacterManager::AddPhysicsAttribute(const Content::JointTransformPtr& joint)
  {
    NOC_ASSERT(joint); 

    if(!joint->IsDynamicJoint())
    {
      wxMessageBox("Joint " + joint->GetName() + " is not a Dynamic Joint.\n" + 
        "Only Dynamic Joints can be physics joints", 
        "Not a Dynamic Joint", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }

    if(joint->GetAttribute<PhysicsJointAttribute>())
    {
      int reply = wxMessageBox("Joint " + joint->GetName() + " is already a Physics Joint.\n" + 
        "Do you want to clobber the old settings?", 
        "Already a Physics Joint", wxYES_NO|wxCENTRE, m_Editor); 

      if(reply == wxYES)
      {
        joint->RemoveAttribute( Reflect::GetType<Content::PhysicsJointAttribute>() ); 
      }
      else
      {
        return; 
      }
    }

    // this will raise events for us. 
    // 
    joint->SetAttribute( new PhysicsJointAttribute() ); 
    m_File->SetModified(true); 

  }

  void CharacterManager::RemovePhysicsAttribute(const Content::JointTransformPtr& joint)
  {
    NOC_ASSERT(joint); 

    // no complaints, no tests, just do it. 
    // 
    joint->RemoveAttribute( Reflect::GetType<Content::PhysicsJointAttribute>() ); 
    m_File->SetModified(true); 
  }

  void CharacterManager::AddIKAttribute(const Content::JointTransformPtr& joint)
  {
    NOC_ASSERT(joint); 

    if(!joint->IsDynamicJoint())
    {
      wxMessageBox("Joint " + joint->GetName() + " is not a Dynamic Joint.\n" + 
        "Only Dynamic Joints can be IK joints", 
        "Not a Dynamic Joint", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }

    if(joint->GetAttribute<IKJointAttribute>())
    {
      int reply = wxMessageBox("Joint " + joint->GetName() + " is already an IK Joint.\n" + 
        "Do you want to clobber the old settings?", 
        "Already an IK Joint", wxYES_NO|wxCENTRE, m_Editor); 

      if(reply == wxYES)
      {
        joint->RemoveAttribute( Reflect::GetType<Content::IKJointAttribute>() ); 
      }
      else
      {
        return; 
      }
    }

    // this will raise events for us. 
    // 
    joint->SetAttribute( new IKJointAttribute() ); 
    m_File->SetModified(true); 

  }

  void CharacterManager::RemoveIKAttribute(const Content::JointTransformPtr& joint)
  {
    NOC_ASSERT(joint); 

    // no complaints, no tests, just do it. 
    // 
    joint->RemoveAttribute( Reflect::GetType<Content::IKJointAttribute>() ); 
    m_File->SetModified(true); 
  }

  bool CharacterManager::AddLooseAttachChain(const Content::LooseAttachChainPtr& chain)
  {
    std::string message; 
    Content::V_LooseAttachChain& chains = m_SkeletonAsset->GetLooseAttachChains(); 

    bool valid = chain->Validate( *GetScene(), chains, message); 

    if(!valid)
    {
      wxMessageBox(message, "Could not create chain", ERROR_MSG_STYLE, m_Editor); 
      return false; 
    }

    // add the chain... 
    //
    m_SkeletonAsset->AddLooseAttachChain(chain); 

    m_LooseAttachmentAdded.Raise( EmptyArgs() ); 
    m_File->SetModified(true); 

    return true; 
  }

  void CharacterManager::RemoveLooseAttachChain(const Content::LooseAttachChainPtr& chain)
  {
    m_SkeletonAsset->RemoveLooseAttachChain(chain); 

    const L_JointTransform& joints = chain->GetJointsInChain(); 

    // first, check to see if there are any loose attach attributes in this chain
    // skipping the start joint, because that may be an end joint in another chain
    // 

    bool hasAttributes = false; 

    for(L_JointTransform::const_iterator it = joints.begin(); it != joints.end(); ++it)
    {
      const JointTransformPtr& joint = *it; 
      if(joint->GetAttribute<LooseAttachJointAttribute>())
      {
        hasAttributes = true; 
        break; 
      }
    }

    if(hasAttributes)
    {
      int response = wxMessageBox("Do you want to delete all loose attachment settings for joints in this chain?", 
        "Delete all settings?", 
        wxYES_NO, m_Editor); 

      if(response == wxYES)
      {
        for(L_JointTransform::const_iterator it = joints.begin(); it != joints.end(); ++it)
        {
          const JointTransformPtr& joint = *it; 
          joint->RemoveAttribute<LooseAttachJointAttribute>(); 

        }
      }
    }

    m_LooseAttachmentRemoved.Raise( EmptyArgs() ); 
    m_File->SetModified(true); 

  }

  void CharacterManager::AddLooseAttachAttribute(const Content::JointTransformPtr& node)
  {
    if(!node)
    {
      return; 
    }

    Content::V_LooseAttachChain::iterator it = m_SkeletonAsset->GetLooseAttachChains().begin(); 
    Content::V_LooseAttachChain::iterator end = m_SkeletonAsset->GetLooseAttachChains().end(); 

    LooseAttachChainPtr containingChain = NULL; 

    for( ; it != end; ++it)
    {
      if( (*it)->ContainsJoint(node))
      {
        containingChain = *it; 
        break; 
      }
    }

    if(!containingChain)
    {
      wxMessageBox("Joint " + node->GetName() + " is not in any loose attachment chains", 
        "Not in a chain", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }

#if 0 
    if(node->GetAttribute<Content::LooseAttachStartAttribute>())
    {
      wxMessageBox("Joint " + node->m_Name + " is the start joint of another chain", 
        "Already a start joint", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }
#endif

    if(node->GetAttribute<Content::LooseAttachEndAttribute>())
    {
      wxMessageBox("Joint " + node->GetName() + " is the end joint of another chain", 
        "Already an end joint", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }

    // add the attribute
    node->SetAttribute( new Content::LooseAttachJointAttribute() ); 
    m_File->SetModified(true); 

  }

  void CharacterManager::RemoveLooseAttachAttribute(const Content::JointTransformPtr& node)
  {
    if(!node)
    {
      return; 
    }

    node->RemoveAttribute( Reflect::GetType<Content::LooseAttachJointAttribute>() ); 
    m_File->SetModified(true); 

  }

  void CharacterManager::AddLooseAttachCollisionAttribute(const Content::JointTransformPtr& node)
  {
    if(!node)
    {
      return; 
    }

    Content::V_LooseAttachChain::iterator it = m_SkeletonAsset->GetLooseAttachChains().begin(); 
    Content::V_LooseAttachChain::iterator end = m_SkeletonAsset->GetLooseAttachChains().end(); 

    LooseAttachChainPtr containingChain = NULL; 

    for( ; it != end; ++it)
    {
      if( (*it)->ContainsJoint(node))
      {
        containingChain = *it; 
        break; 
      }
    }

    if(!containingChain)
    {
      wxMessageBox("Joint " + node->GetName() + " is not in any loose attachment chains", 
        "Not in a chain", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }

    
    if(node->GetAttribute<Content::LooseAttachCollisionAttribute>())
    {
      wxMessageBox("Joint " + node->GetName() + " already has a LooseAttachCollisionAttribute", 
        "Already has Attribute", ERROR_MSG_STYLE, m_Editor); 
      return; 
    }

    // add the attribute
    node->SetAttribute( new Content::LooseAttachCollisionAttribute() ); 
    m_File->SetModified(true); 
  }
  
  void CharacterManager::RemoveLooseAttachCollisionAttribute(const Content::JointTransformPtr& node)
  {
    if(!node)
    {
      return; 
    }

    node->RemoveAttribute( Reflect::GetType<Content::LooseAttachCollisionAttribute>() ); 
    m_File->SetModified(true); 

  }


  void CharacterManager::CopyAllPhysicsAttributesToIK()
  {
    if(!m_MasterScene)
    {
      return; 
    }

    Content::ScenePtr& scene = m_MasterScene; 

    for(u32 j = 0; j < scene->m_Joints.size(); j++)
    {
      const JointTransformPtr& joint = scene->m_Joints[j]; 

      // clear out the existing attribute (if any); 
      joint->RemoveAttribute( Reflect::GetType<Content::IKJointAttribute>() ); 

      Content::PhysicsJointAttributePtr physics = joint->GetAttribute<Content::PhysicsJointAttribute>(); 

      if(physics)
      {
        joint->SetAttribute( new IKJointAttribute(physics) ); 
      }

      m_File->SetModified(true); 
    }

    ClearSelections(); 

    m_ForceUpdate.Raise( EmptyArgs() ); 

  }


  void CharacterManager::CopyOnePhysicsAttributeToIK()
  {
    if(!m_MasterScene)
    {
      return; 
    }

    const JointTransformPtr& joint = GetSelectedJoint(); 

    if(joint)
    {
      Content::PhysicsJointAttributePtr physics = joint->GetAttribute<Content::PhysicsJointAttribute>(); 

      if(!physics)
      {
        wxMessageBox("Selected joint " + joint->GetName() + " has no physics attribute", 
          "No Physics Attribute", wxOK); 
        return; 
      }

      Content::IKJointAttributePtr ik = joint->GetAttribute<Content::IKJointAttribute>(); 

      if(ik)
      {
        int reply = wxMessageBox("Selected joint " + joint->GetName() + " is already an IK Joint.\nDo you want to clobber the old settings?\n", 
          "Already an IK Joint", wxYES_NO); 

        if(reply != wxYES)
        {
          return; 
        }
      }

      ik = new Content::IKJointAttribute(physics); 
      joint->RemoveAttribute( Reflect::GetType<Content::IKJointAttribute>() ); 
      joint->SetAttribute( ik ); 

      // change the selected attribute here. 
      SetSelectedAttribute( ik ); 

      // mark file 
      m_File->SetModified(true); 

    }
  }

  void CharacterManager::OnDocumentClosed(const DocumentChangedArgs& args)
  {
    NOC_ASSERT( args.m_Document == m_File.Ptr() );
    if ( m_File.ReferencesObject() )
    {
      m_File->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &CharacterManager::OnDocumentClosed ) );
    }

    m_LocalScene  = NULL; 
    m_MasterScene = NULL; 
    m_MobyClass   = NULL; 
    m_File        = NULL; 
    m_SkeletonAsset = NULL; 

    m_Closed.Raise(EmptyArgs()); 

    ClearSelections(); 
  }

}


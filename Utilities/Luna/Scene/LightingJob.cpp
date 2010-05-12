#include "Precompile.h"
#include "LightingJob.h"

#include "Entity.h"
#include "CubeMapProbe.h"
#include "Light.h"
#include "LightingJobCommand.h"
#include "LightingVolume.h"
#include "Scene.h"
#include "SceneManager.h"
#include "LightingEnvironment.h"
#include "Zone.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/BakedLightingAttribute.h"
#include "Asset/SpatialLightingAttribute.h"
#include "UIToolKit/ImageManager.h"

// Using
using namespace Math;
using namespace Luna;

// RTTI
LUNA_DEFINE_TYPE(Luna::LightingJob);

void LightingJob::InitializeType()
{
  Reflect::RegisterClass< Luna::LightingJob >( "Luna::LightingJob" );

  Enumerator::InitializePanel( "Lighting Job", CreatePanelSignature::Delegate( &LightingJob::CreatePanel ) );
}

void LightingJob::CleanupType()
{
  Reflect::UnregisterClass< Luna::LightingJob >();
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified entity can be a Render Target or Shadow Caster
// for a lighting job.
// 
bool LightingJob::IsLightable( Luna::SceneNode* instance )
{
  if ( instance )
  {
    Attribute::AttributeCollectionPtr collection = instance->GetPackage< Attribute::AttributeCollection >();
    Attribute::AttributeViewer< Asset::BakedLightingAttribute > bakedLighting( collection );
    return bakedLighting.Valid();
  }
  return false;
}


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingJob::LightingJob( Luna::Scene* scene )
: Luna::SceneNode ( scene, new Content::LightingJob() )
{
  Init();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
LightingJob::LightingJob( Luna::Scene* scene, Content::LightingJob* light )
: Luna::SceneNode ( scene, light )
{
  Init();
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
LightingJob::~LightingJob()
{
  m_Scene->GetManager()->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingJob::SceneAdded ) );
  m_Scene->GetManager()->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingJob::SceneRemoving ) );

  RemoveAllSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Common constructor initialization code.
// 
void LightingJob::Init()
{
  m_Scene->GetManager()->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &LightingJob::SceneAdded ) );
  m_Scene->GetManager()->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &LightingJob::SceneRemoving ) );

  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  if( !contentJob->m_RenderOptions.ReferencesObject() )
  {
    contentJob->m_RenderOptions = new Content::MentalRayOptions();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Common add function for adding an object to the lighting job into one of the
// collections.  Uses a pointer to member to determine which set to insert the
// node's id into.  Returns true if the node was successfully added to the
// specified container.
// 
bool LightingJob::Add( const tuid& zoneID, Luna::SceneNode* node, LightingJobMemberChangeSignature::Event& evt, UniqueID::S_TUID Content::LightingJobZoneItems::* pointerToMember )
{
  bool wasAdded = false;
  Content::LightingJob* job = GetPackage< Content::LightingJob >();
  Content::M_LightingJobZoneItems::iterator jobElementItr = job->m_JobElements.insert( Content::M_LightingJobZoneItems::value_type( zoneID, new Content::LightingJobZoneItems( zoneID ) ) ).first;
  if ( jobElementItr != job->m_JobElements.end() )
  {
    UniqueID::S_TUID& itemSet = ( *jobElementItr->second.Ptr() ).*pointerToMember;
    wasAdded = itemSet.insert( node->GetID() ).second;

    if ( wasAdded )
    {
      AddSceneListeners( node->GetScene() );
      evt.Raise( LightingJobMemberChangeArgs( this, node ) );
    }
  }
  return wasAdded;
}

///////////////////////////////////////////////////////////////////////////////
// Common remove function for removing the specified node from a particular
// container in the lighting job.  Uses a pointer to member to indicate which
// container will be searched for removing the item.  Returns true if the node
// was removed from the container successfully.  Returns false if the node was
// not in the container to begin with.
// 
bool LightingJob::Remove( const tuid& zoneID, Luna::SceneNode* node, LightingJobMemberChangeSignature::Event& evt, UniqueID::S_TUID Content::LightingJobZoneItems::* pointerToMember )
{
  bool wasRemoved = false;
  Content::LightingJob* job = GetPackage< Content::LightingJob >();
  Content::M_LightingJobZoneItems::iterator jobElementItr = job->m_JobElements.find( zoneID );
  if ( jobElementItr != job->m_JobElements.end() )
  {
    wasRemoved = ( ( *jobElementItr->second.Ptr() ).*pointerToMember ).erase( node->GetID() ) > 0;

    if ( wasRemoved )
    {
      evt.Raise( LightingJobMemberChangeArgs( this, node ) );
    }
  }
  return wasRemoved;
}

///////////////////////////////////////////////////////////////////////////////
// Common code for fetching a particular container off the Content::LightingJob.
// Uses a pointer to member to indicate which container you want to fetch.  Returns
// the container, or an empty set if the container was not found.
// 
const UniqueID::S_TUID& LightingJob::Get( const tuid& zoneID, UniqueID::S_TUID Content::LightingJobZoneItems::* pointerToMember ) const
{
  static const UniqueID::S_TUID emptySet;
  const Content::LightingJob* job = GetPackage< Content::LightingJob >();
  Content::M_LightingJobZoneItems::const_iterator jobElementItr = job->m_JobElements.find( zoneID );
  if ( jobElementItr != job->m_JobElements.end() )
  {
    return ( *jobElementItr->second.Ptr() ).*pointerToMember;
  }
  return emptySet;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the index of the icon to use in the UI for this object.
// 
i32 LightingJob::GetImageIndex() const
{
  return UIToolKit::GlobalImageManager().GetImageIndex( "light_16.png" );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a friendly name to be used to refer to this type of object in the
// UI.
// 
std::string LightingJob::GetApplicationTypeName() const
{
  return "LightingJob";
}

///////////////////////////////////////////////////////////////////////////////
// Populates the specified set with the tuids of all the zones contained by
// this lighting job.
// 
void LightingJob::GetZoneIDs( S_tuid& outZones ) const
{
  const Content::LightingJob* job = GetPackage< Content::LightingJob >();
  Content::M_LightingJobZoneItems::const_iterator itr = job->m_JobElements.begin();
  Content::M_LightingJobZoneItems::const_iterator end = job->m_JobElements.end();
  for ( ; itr != end; ++itr )
  {
    outZones.insert( itr->first );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds a light to the list maintained by this class.  You must specify the
// zone that the light belongs to.  An event will be raised to notify interested
// listeners.
// 
bool LightingJob::AddLight( const tuid& zoneID, Luna::Light* light )
{
  return Add( zoneID, light, m_LightAdded, &Content::LightingJobZoneItems::m_Lights );
}


///////////////////////////////////////////////////////////////////////////////
// Remove a light from this job.  An event will be fired to notify interested
// listeners.
// 
bool LightingJob::RemoveLight( const tuid& zoneID, Luna::Light* light )
{
  return Remove( zoneID, light, m_LightRemoved, &Content::LightingJobZoneItems::m_Lights );
}

///////////////////////////////////////////////////////////////////////////////
// Return the set of lights that all belong to the specified zone (and are members
// of this job).
// 
const UniqueID::S_TUID& LightingJob::GetLights( const tuid& zoneID ) const
{
  return Get( zoneID, &Content::LightingJobZoneItems::m_Lights );
}

///////////////////////////////////////////////////////////////////////////////
// Add a shadow caster to this lighting job.  Pass in the tuid of the file
// where the instance can be found.
// 
bool LightingJob::AddShadowCaster( const tuid& zoneID, Luna::Entity* instance )
{
  return Add( zoneID, instance, m_ShadowCasterAdded, &Content::LightingJobZoneItems::m_ExcludedShadowCasters );
}

///////////////////////////////////////////////////////////////////////////////
// Remove a shadow caster from the lighting job.  Pass in the tuid of the file
// where the instance can be found.
// 
bool LightingJob::RemoveShadowCaster( const tuid& zoneID, Luna::Entity* instance )
{
  return Remove( zoneID, instance, m_ShadowCasterRemoved, &Content::LightingJobZoneItems::m_ExcludedShadowCasters );
}

///////////////////////////////////////////////////////////////////////////////
// Get the set of IDs for all the shadow casters that are in this lighting job,
// and within the specified zone.
// 
const UniqueID::S_TUID& LightingJob::GetShadowCasters( const tuid& zoneID ) const
{
  return Get( zoneID, &Content::LightingJobZoneItems::m_ExcludedShadowCasters );
}

///////////////////////////////////////////////////////////////////////////////
// Add a render target to this lighting job.  You must specify what zone this
// item belongs to.
// 
bool LightingJob::AddRenderTarget( const tuid& zoneID, Luna::Entity* instance )
{
  return Add( zoneID, instance, m_RenderTargetAdded, &Content::LightingJobZoneItems::m_RenderTargets );
}

///////////////////////////////////////////////////////////////////////////////
// Remove a render target from this lighting job.  You must specify what zone
// this item belongs to.
// 
bool LightingJob::RemoveRenderTarget( const tuid& zoneID, Luna::Entity* instance )
{
  return Remove( zoneID, instance, m_RenderTargetRemoved, &Content::LightingJobZoneItems::m_RenderTargets );
}

///////////////////////////////////////////////////////////////////////////////
// Returns a set of the render targets that are in this lighting job, and part
// of the specified zone.
// 
const UniqueID::S_TUID& LightingJob::GetRenderTargets( const tuid& zoneID ) const
{
  return Get( zoneID, &Content::LightingJobZoneItems::m_RenderTargets );
}

///////////////////////////////////////////////////////////////////////////////
// Add a cubemap probe.
// 
bool LightingJob::AddCubeMapProbe( const tuid& zoneID, Luna::CubeMapProbe* probe )
{
  return Add( zoneID, probe, m_ProbeAdded, &Content::LightingJobZoneItems::m_CubeMapProbes );
}

///////////////////////////////////////////////////////////////////////////////
// Remove a cubemap probe.
// 
bool LightingJob::RemoveCubeMapProbe( const tuid& zoneID, Luna::CubeMapProbe* probe )
{
  return Remove( zoneID, probe, m_ProbeRemoved, &Content::LightingJobZoneItems::m_CubeMapProbes );
}

///////////////////////////////////////////////////////////////////////////////
// Get the IDs of all the probes that belong to this lighting job and are in
// the specified zone.
// 
const UniqueID::S_TUID& LightingJob::GetCubeMapProbes( const tuid& zoneID ) const
{
  return Get( zoneID, &Content::LightingJobZoneItems::m_CubeMapProbes );
}

///////////////////////////////////////////////////////////////////////////////
// Adds a light to the list maintained by this class.  You must specify the
// zone that the light belongs to.  An event will be raised to notify interested
// listeners.
// 
bool LightingJob::AddLightingEnvironment( const tuid& zoneID, Luna::LightingEnvironment* env )
{
  return Add( zoneID, env, m_LightingEnvironmentAdded, &Content::LightingJobZoneItems::m_LightingEnvironments );
}


///////////////////////////////////////////////////////////////////////////////
// Remove a light from this job.  An event will be fired to notify interested
// listeners.
// 
bool LightingJob::RemoveLightingEnvironment( const tuid& zoneID, Luna::LightingEnvironment* env )
{
  return Remove( zoneID, env, m_LightingEnvironmentRemoved, &Content::LightingJobZoneItems::m_LightingEnvironments );
}

///////////////////////////////////////////////////////////////////////////////
// Return the set of lights that all belong to the specified zone (and are members
// of this job).
//

const UniqueID::S_TUID& LightingJob::GetZones( const tuid& zoneID ) const
{
  const Content::LightingJob* job = GetPackage< Content::LightingJob >();

  return job->m_ExcludedZoneUIDs;
  
}

///////////////////////////////////////////////////////////////////////////////
// Adds a light to the list maintained by this class.  You must specify the
// zone that the light belongs to.  An event will be raised to notify interested
// listeners.
// 
bool LightingJob::AddZone( const tuid& zoneID, Zone* zone )
{
  Content::LightingJob* job = GetPackage< Content::LightingJob >();
  bool wasAdded = job->m_ExcludedZoneFileIDs.insert( zone->GetFileID() ).second;
  job->m_ExcludedZoneUIDs.insert( zone->GetID() );
 
    if ( wasAdded )
    {
      AddSceneListeners( zone->GetScene() );
      m_ZoneAdded.Raise( LightingJobMemberChangeArgs( this, zone ) );
    }

  return wasAdded;
}


///////////////////////////////////////////////////////////////////////////////
// Remove a light from this job.  An event will be fired to notify interested
// listeners.
// 
bool LightingJob::RemoveZone( const tuid& zoneID, Zone* zone )
{
  bool wasRemoved = false;
  Content::LightingJob* job = GetPackage< Content::LightingJob >();
  S_tuid::iterator itor = job->m_ExcludedZoneFileIDs.find( zone->GetFileID() );
  
  if ( itor != job->m_ExcludedZoneFileIDs.end() )
  {
    wasRemoved = true;
    
    job->m_ExcludedZoneFileIDs.erase( itor );
    job->m_ExcludedZoneUIDs.erase( zone->GetID() );

    m_ZoneRemoved.Raise( LightingJobMemberChangeArgs( this, zone ) );

  }
  return wasRemoved;
}

///////////////////////////////////////////////////////////////////////////////
// Return the set of lights that all belong to the specified zone (and are members
// of this job).
// 
const UniqueID::S_TUID& LightingJob::GetLightingEnvironments( const tuid& zoneID ) const
{
  return Get( zoneID, &Content::LightingJobZoneItems::m_LightingEnvironments );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to add callbacks to the scene.
// 
void LightingJob::AddSceneListeners( Luna::Scene* scene )
{
  scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingJob::SceneNodeAdded ) );
  scene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LightingJob::SceneNodeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to remove callbacks from the specified scene.
// 
void LightingJob::RemoveSceneListeners( Luna::Scene* scene )
{
  scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &LightingJob::SceneNodeAdded ) );
  scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &LightingJob::SceneNodeRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to remove all callbacks from all scenes.
// 
void LightingJob::RemoveAllSceneListeners()
{
  M_SceneSmartPtr::const_iterator sceneItr = m_Scene->GetManager()->GetScenes().begin();
  M_SceneSmartPtr::const_iterator sceneEnd = m_Scene->GetManager()->GetScenes().end();
  for ( ; sceneItr != sceneEnd; ++sceneItr )
  {
    RemoveSceneListeners( sceneItr->second );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is added to the scene manager.  Hooks up listeners
// to that scene.
// 
void LightingJob::SceneAdded( const SceneChangeArgs& args )
{
  AddSceneListeners( args.m_Scene );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a scene is removed from the scene manager.  Disconnects
// our listeners to that scene.  This is probably a bit superfluous though... 
// the scene is being removed so it will never fire us an event.  Just here
// for completeness.
// 
void LightingJob::SceneRemoving( const SceneChangeArgs& args )
{
  RemoveSceneListeners( args.m_Scene );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is added to one of the scenes that we are listening
// to.  If the node was suppose to be within this lighting job, we have to put
// it back.
// 
void LightingJob::SceneNodeAdded( const NodeChangeArgs& args )
{
  tuid zoneID = TUID::Null;
  if ( args.m_Node->GetScene() )
  {
    zoneID = args.m_Node->GetScene()->GetFileID();
  }

  if ( zoneID != TUID::Null )
  {
    P_TuidToUid key( zoneID, args.m_Node->GetID() );
    M_NodeToRemoveCmd::iterator found = m_RemoveCmds.find( key );
    if ( found != m_RemoveCmds.end() )
    {
      const Undo::BatchCommandPtr& cmd = found->second;
      cmd->Undo();

      m_RemoveCmds.erase( found );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a node is deleted from one of the scenes that we are
// listening to.  If that node is contained anywhere in this lighting job,
// we have to remove it.
// 
void LightingJob::SceneNodeRemoved( const NodeChangeArgs& args )
{
  tuid zoneID = TUID::Null;
  if ( args.m_Node->GetScene() )
  {
    zoneID = args.m_Node->GetScene()->GetFileID();
  }

  if ( zoneID != TUID::Null )
  {
    Undo::BatchCommandPtr batch = new Undo::BatchCommand();
    if ( args.m_Node->HasType( Reflect::GetType< Luna::Light >() ) && RemoveLight( zoneID, Reflect::DangerousCast< Luna::Light >( args.m_Node ) ) )
    {
      batch->Push( new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Light, this, args.m_Node, false ) );
    }

    if ( args.m_Node->HasType( Reflect::GetType< Luna::CubeMapProbe >() ) && RemoveCubeMapProbe( zoneID, Reflect::DangerousCast< Luna::CubeMapProbe >( args.m_Node ) ) )
    {
      batch->Push( new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Probe, this, args.m_Node, false ) );
    }

    if ( args.m_Node->HasType( Reflect::GetType< Luna::Entity >() ) )
    {
      if ( RemoveRenderTarget( zoneID, Reflect::DangerousCast< Luna::Entity >( args.m_Node ) ) )
      {
        batch->Push( new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Render, this, args.m_Node, false ) );
      }

      if ( RemoveShadowCaster( zoneID, Reflect::DangerousCast< Luna::Entity >( args.m_Node ) ) )
      {
        batch->Push( new LightingJobCommand( LightingJobCommandActions::Remove, LightingJobCommandGroups::Shadow, this, args.m_Node, false ) );
      }
    }

    if ( !batch->IsEmpty() )
    {
      P_TuidToUid key( zoneID, args.m_Node->GetID() );
      m_RemoveCmds.insert( M_NodeToRemoveCmd::value_type( key, batch ) );
    }
  }
}

bool LightingJob::ValidatePanel(const std::string& name)
{
  if (name == "Lighting Job")
  {
    return true;
  }

  return __super::ValidatePanel( name );
}

void LightingJob::CreatePanel( CreatePanelArgs& args )
{
  args.m_Enumerator->PushPanel("Lighting Job", true);
  {
    args.m_Enumerator->PushPanel("Final Gather", true );
    {
      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("FinalGather Enabled");
        args.m_Enumerator->AddCheckBox<Luna::LightingJob, bool>( args.m_Selection, &LightingJob::GetFinalGather, &LightingJob::SetFinalGather );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Accuracy");
        args.m_Enumerator->AddValue<Luna::LightingJob, u32>( args.m_Selection, &LightingJob::GetFinalGatherAccuracy, &LightingJob::SetFinalGatherAccuracy );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Max Radius");
        args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetFinalGatherMaxRad, &LightingJob::SetFinalGatherMaxRad );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Min Radius");
        args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetFinalGatherMinRad, &LightingJob::SetFinalGatherMinRad );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Final Gather Scale");
        args.m_Enumerator->AddColorPicker<Luna::LightingJob, Color3>( args.m_Selection, &LightingJob::GetFinalGatherColor, &LightingJob::SetFinalGatherColor );

        Inspect::Slider* slider = args.m_Enumerator->AddSlider<Luna::LightingJob, float>( args.m_Selection, &LightingJob::GetFinalGatherScale, &LightingJob::SetFinalGatherScale );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 1000.0f );

        args.m_Enumerator->AddValue<Luna::LightingJob, float>( args.m_Selection, &LightingJob::GetFinalGatherScale, &LightingJob::SetFinalGatherScale );
      }
      args.m_Enumerator->Pop();
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushPanel("Globillum", true );
    {
      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Globillum Enabled");
        args.m_Enumerator->AddCheckBox<Luna::LightingJob, bool>( args.m_Selection, &LightingJob::GetGlobillum, &LightingJob::SetGlobillum );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Accuracy");
        args.m_Enumerator->AddValue<Luna::LightingJob, u32>( args.m_Selection, &LightingJob::GetGlobillumAccuracy, &LightingJob::SetGlobillumAccuracy );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Radius");
        args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetGlobillumRadius, &LightingJob::SetGlobillumRadius );
      }
      args.m_Enumerator->Pop();

      args.m_Enumerator->PushContainer();
      {
        args.m_Enumerator->AddLabel("Globillum Scale");
        args.m_Enumerator->AddColorPicker<Luna::LightingJob, Color3>( args.m_Selection, &LightingJob::GetGlobillumColor, &LightingJob::SetGlobillumColor );

        Inspect::Slider* slider = args.m_Enumerator->AddSlider<Luna::LightingJob, float>( args.m_Selection, &LightingJob::GetGlobillumScale, &LightingJob::SetGlobillumScale );
        slider->SetRangeMin( 0.0f );
        slider->SetRangeMax( 1000.0f );

        args.m_Enumerator->AddValue<Luna::LightingJob, float>( args.m_Selection, &LightingJob::GetGlobillumScale, &LightingJob::SetGlobillumScale );
      }
      args.m_Enumerator->Pop();
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Samples");
      args.m_Enumerator->AddValue<Luna::LightingJob, int>( args.m_Selection, &LightingJob::GetMinSamples, &LightingJob::SetMinSamples );
      args.m_Enumerator->AddValue<Luna::LightingJob, int>( args.m_Selection, &LightingJob::GetMaxSamples, &LightingJob::SetMaxSamples );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Contrast");
      args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetRContrast, &LightingJob::SetRContrast );
      args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetGContrast, &LightingJob::SetGContrast );
      args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetBContrast, &LightingJob::SetBContrast );
      args.m_Enumerator->AddValue<Luna::LightingJob, f32>( args.m_Selection, &LightingJob::GetAContrast, &LightingJob::SetAContrast );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Render Quality");
      Inspect::Choice* choice = args.m_Enumerator->AddChoice<Luna::LightingJob, int>(args.m_Selection, &LightingJob::GetRenderQuality, &LightingJob::SetRenderQuality);
      choice->SetDropDown( true );

      Inspect::V_Item items;
      {
        {
          std::ostringstream str;
          str << Content::RenderQualities::Lowest;
          items.push_back( Inspect::Item( "Lowest", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Content::RenderQualities::Low;
          items.push_back( Inspect::Item( "Low", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Content::RenderQualities::Medium;
          items.push_back( Inspect::Item( "Medium", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Content::RenderQualities::High;
          items.push_back( Inspect::Item( "High", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Content::RenderQualities::Highest;
          items.push_back( Inspect::Item( "Highest", str.str() ) );
        }
        {
          std::ostringstream str;
          str << Content::RenderQualities::Custom;
          items.push_back( Inspect::Item( "Custom", str.str() ) );
        }
      }
      choice->SetItems( items );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("BSP Size");
      args.m_Enumerator->AddValue<Luna::LightingJob, u32>( args.m_Selection, &LightingJob::GetBSPSize, &LightingJob::SetBSPSize );

      args.m_Enumerator->AddLabel("BSP Depth");
      args.m_Enumerator->AddValue<Luna::LightingJob, u32>( args.m_Selection, &LightingJob::GetBSPDepth, &LightingJob::SetBSPDepth );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Enable");
      args.m_Enumerator->AddCheckBox<Luna::LightingJob, bool>( args.m_Selection, &LightingJob::GetEnabled, &LightingJob::SetEnabled );
    }
    args.m_Enumerator->Pop();

    args.m_Enumerator->PushContainer();
    {
      args.m_Enumerator->AddLabel("Calculate Unoccluded Dir");
      args.m_Enumerator->AddCheckBox<Luna::LightingJob, bool>( args.m_Selection, &LightingJob::GetCalculateUnoccludedDir, &LightingJob::SetCalculateUnoccludedDir );
    }
    args.m_Enumerator->Pop();
  }
  args.m_Enumerator->Pop();
}

bool LightingJob::GetFinalGather() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_FinalGather;
}
void LightingJob::SetFinalGather( bool enable )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_FinalGather = enable;
}

u32 LightingJob::GetFinalGatherAccuracy() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_FinalGatherAccuracy;

}
void LightingJob::SetFinalGatherAccuracy( u32 accuracy )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_FinalGatherAccuracy = accuracy;
}

f32 LightingJob::GetFinalGatherMinRad() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_FinalGatherMinRadius;
}
void LightingJob::SetFinalGatherMinRad( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_FinalGatherMinRadius = value;
}

f32 LightingJob::GetFinalGatherMaxRad() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_FinalGatherMaxRadius;
}
void LightingJob::SetFinalGatherMaxRad( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_FinalGatherMaxRadius = value;
}

bool LightingJob::GetGlobillum() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_Globillum;
}
void LightingJob::SetGlobillum( bool enable )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_Globillum = enable;
}

f32 LightingJob::GetRContrast() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_Contrast.x;
}
f32 LightingJob::GetGContrast() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_Contrast.y;
}
f32 LightingJob::GetBContrast() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_Contrast.z;
}
f32 LightingJob::GetAContrast() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_Contrast.w;
}

void LightingJob::SetRContrast( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_Contrast.x = value;
}
void LightingJob::SetGContrast( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_Contrast.y = value;
}
void LightingJob::SetBContrast( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_Contrast.z = value;
}
void LightingJob::SetAContrast( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_Contrast.w = value;
}

i32 LightingJob::GetMinSamples() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_SamplesMin;

}
void LightingJob::SetMinSamples( i32 accuracy )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_SamplesMin = accuracy;
}

i32 LightingJob::GetMaxSamples() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_SamplesMax;

}
void LightingJob::SetMaxSamples( i32 accuracy )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_SamplesMax = accuracy;
}

i32 LightingJob::GetRenderQuality() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_RenderQuality;

}
void LightingJob::SetRenderQuality( i32 quality )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( static_cast< Content::RenderQuality >( quality ) );
}

Color3 LightingJob::GetFinalGatherColor() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_FinalGatherScale;
}

void LightingJob::SetFinalGatherColor( Color3 color )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->m_FinalGatherScale = color;
}

float LightingJob::GetFinalGatherScale() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_FinalGatherScale.s;
}
void LightingJob::SetFinalGatherScale( float intensity )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->m_FinalGatherScale.s = intensity;
}

Color3 LightingJob::GetGlobillumColor() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_GlobillumScale;
}

void LightingJob::SetGlobillumColor( Color3 color )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->m_GlobillumScale = color;
}

float LightingJob::GetGlobillumScale() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_GlobillumScale.s;
}
void LightingJob::SetGlobillumScale( float intensity )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->m_GlobillumScale.s = intensity;
}

u32 LightingJob::GetGlobillumAccuracy() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_GlobillumAccuracy;
}
void LightingJob::SetGlobillumAccuracy( u32 accuracy )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_GlobillumAccuracy = accuracy;
}

f32 LightingJob::GetGlobillumRadius() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_GlobillumRadius;
}
void LightingJob::SetGlobillumRadius( f32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_GlobillumRadius = value;
}

u32 LightingJob::GetBSPDepth() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_BSPDepth;
}
void LightingJob::SetBSPDepth( u32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->m_BSPDepth = value;
}

u32 LightingJob::GetBSPSize() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_BSPSize;
}
void LightingJob::SetBSPSize( u32 value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->m_BSPSize = value;
}

bool LightingJob::GetEnabled() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  return contentJob->m_Enabled;
}

void LightingJob::SetEnabled( bool enable )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  contentJob->m_Enabled = enable;
}

bool LightingJob::GetCalculateUnoccludedDir() const
{
  const Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  return contentJob->m_RenderOptions->m_CalculateUnoccludedDir;
}

void LightingJob::SetCalculateUnoccludedDir( bool value )
{
  Content::LightingJob* contentJob = GetPackage< Content::LightingJob >();
  NOC_ASSERT( contentJob->m_RenderOptions );
  contentJob->m_RenderOptions->SetRenderQuality( Content::RenderQualities::Custom );
  contentJob->m_RenderOptions->m_CalculateUnoccludedDir = value;
}
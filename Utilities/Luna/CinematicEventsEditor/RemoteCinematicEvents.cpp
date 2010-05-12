#include "Precompile.h"
#include "RemoteCinematicEvents.h"
#include "Live/RuntimeConnection.h"
#include "CinematicEventsManager.h"
#include "CinematicEventsDocument.h"

#include "ipc/tcp.h"
#include "ipc/pipe.h"
#include "File/Manager.h"
#include "AppUtils/AppUtils.h"
#include "Console/Console.h"
#include "igCore/igHeaders/ps3structs.h"
#include "IGSerializer/IGSerializer.h"
#include "igCore/igHeaders/FileChunkIDs.h"
#include "Attribute/AttributeHandle.h"
#include "Content/RuntimeDataAttribute.h"

#include "Symbol/Inheritance.h"
#include "Symbol/BasicDataHandler.h"

using namespace Luna;
using namespace Asset;
using namespace RPC;
using namespace Nocturnal;
using namespace Attribute;
using namespace Content;
using namespace Symbol;

namespace Luna
{
  namespace RemoteCinematicEvents
  {
    static bool g_Initialized = false;
    bool g_ValidCinematic = false;
    CinematicEventsManager* g_Manager = NULL;

    i32 g_CurrentClip = -1;
    i32 g_CurrentFrame = -1;

    ILunaCinematicEventsHost* g_LunaCinematicEventsHost = NULL;
    ILunaCinematicEventsTool* g_LunaCinematicEventsTool = NULL;

    struct LunaCinematicEventsTool : ILunaCinematicEventsTool
    {
      LOCAL_IMPL(LunaCinematicEventsTool, ILunaCinematicEventsTool);

      virtual void SetCinematic(SetCinematicParams* params, u8* data, u32 size);
      //virtual void SetCinematicSettings();
      virtual void SetFrame(i32* frame);
    };

    void LunaCinematicEventsTool::SetCinematic( SetCinematicParams* params, u8* data, u32 size )
    {
      CinematicEventsDocumentPtr file = g_Manager->GetFile();

      if ( file->m_CinematicAsset->GetShortName() == params->m_CinematicName )
      {
        RPC::CinematicInfo* cineInfo = (RPC::CinematicInfo*)data;
        
        cineInfo->m_frames = ConvertEndian( cineInfo->m_frames, true );
        cineInfo->m_start_frame = ConvertEndian( cineInfo->m_start_frame, true );
        cineInfo->m_end_frame = ConvertEndian( cineInfo->m_end_frame, true );

        g_ValidCinematic = g_Manager->ViewerConnected( cineInfo );

        if ( g_ValidCinematic )
        {

          //g_Manager->SetNumFrames( cineInfo->m_frames );
          //g_Manager->SetStartFrame( cineInfo->m_start_frame );
          //g_Manager->SetEndFrame( cineInfo->m_end_frame );
          g_Manager->CinematicLoaded();

          //i32 clip( g_Manager->GetCurrentClipIndex() );
          i32 frame( g_Manager->GetCurrentFrame() );
          //i32 updateClass( g_Manager->GetCinematicVTGenIndex() );
          //g_LunaCinematicEventsHost->SetCinematicSettings( );
          g_LunaCinematicEventsHost->SetFrame( &frame );
          
          //g_LunaCinematicEventsHost->SetCinematic( &updateClass );

          CinematicEventListPtr eventList = g_Manager->GetEventList();
          for( u32 i = 0 ; i < (u32)eventList->m_Events.size(); ++i)
          {
            Asset::CinematicEventPtr thisEvent = eventList->m_Events[ i ];
            if ( !thisEvent->RectifyRuntimeData() )
            {
              Console::Warning( "Runtime data, not rectified!\n" );
            }
          }

          UpdateEvents();
        }
      }
      else
      {
        g_ValidCinematic = false;
      }
    }
    
    //void LunaCinematicEventsTool::SetCinematicSettings( )
    //{
      //if ( g_ValidCinematic )
      //{
        //g_CurrentClip = *clip;
        //g_Manager->SetClip( g_CurrentClip );

        //UpdateEvents();
      //}
    //}

    void LunaCinematicEventsTool::SetFrame( i32* frame )
    {
      if ( g_ValidCinematic )
      {
        g_CurrentFrame = *frame;
        g_Manager->SetFrame( g_CurrentFrame );
      }
      else
      {
        g_CurrentFrame = g_Manager->GetStartFrame();
      }
    }

    bool Initialize( CinematicEventsManager& manager )
    {
      if( g_Initialized )
      {
        return true;
      }

      g_Initialized = true;

      g_Manager = &manager;

      RPC::Host* host = RuntimeConnection::GetHost();

      // create a local implementation of our functionality
      g_LunaCinematicEventsHost = RuntimeConnection::GetRemoteCinematicEventsView();
      g_LunaCinematicEventsTool = new LunaCinematicEventsTool( host );
      host->SetLocalInterface( kLunaCinematicEventsTool, g_LunaCinematicEventsTool );

      // set up event listeners
      g_Manager->AddCinematicChangeListener( CinematicChangeSignature::Delegate ( &ClipChanged ) );
      g_Manager->AddFrameChangedListener( FrameChangedSignature::Delegate ( &FrameChanged ) );
      g_Manager->AddEventExistenceListener( EventExistenceSignature::Delegate ( &EventExistence ) );
      g_Manager->AddEventsChangedListener( EventsChangedSignature::Delegate ( &EventsChanged ) );
      g_Manager->AddCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( &AssetOpened ) );
      g_Manager->AddCinematicClosedListener( CinematicFileChangeSignature::Delegate ( &AssetClosed ) );
      g_Manager->AddCinematicLoadedListener( CinematicLoadedSignature::Delegate ( &CinematicLoaded ) );

      return g_Initialized;
    }

    void Cleanup()
    {
      if (g_Initialized)
      {
        delete g_LunaCinematicEventsTool;

        g_LunaCinematicEventsHost = NULL;
        g_LunaCinematicEventsTool = NULL;

        // remove event listeners
        g_Manager->RemoveCinematicChangeListener( CinematicChangeSignature::Delegate ( &ClipChanged ) );
        g_Manager->RemoveFrameChangedListener( FrameChangedSignature::Delegate ( &FrameChanged ) );
        g_Manager->RemoveEventExistenceListener( EventExistenceSignature::Delegate ( &EventExistence ) );
        g_Manager->RemoveEventsChangedListener( EventsChangedSignature::Delegate ( &EventsChanged ) );
        g_Manager->RemoveCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( &AssetOpened ) );
        g_Manager->RemoveCinematicClosedListener( CinematicFileChangeSignature::Delegate ( &AssetClosed ) );
        g_Manager->RemoveCinematicLoadedListener( CinematicLoadedSignature::Delegate ( &CinematicLoaded ) );
        
        g_Initialized = false;
      }
    }

    void SetPlayRate( float rate )
    {
      if ( g_ValidCinematic )
      {
        g_LunaCinematicEventsHost->SetPlayRate( &rate );
      }
    }

    void PlayEvent( const std::string& eventType, const float value, bool untrigger, bool infinite, bool dominantOnly, bool gameplay )
    {
      if ( g_ValidCinematic )
      {
        i32 eventTypeIndex;

        const Symbol::EnumPtr& event_types_enum= g_Manager->GetEventTypesEnum(gameplay);
        if (event_types_enum && !event_types_enum->GetElemValue(eventType, eventTypeIndex))
        {
          std::stringstream text;
          text << "Unrecognized Event Type " << eventType << ".";
          Console::Warning(text.str().c_str());

          return;
        }

        //RPC::CinematicEvent event;
        /*event.m_event_type = eventTypeIndex;
        event.m_untrigger = untrigger;
        event.m_value = (i32) value;
        event.m_time = 0;
        event.m_infinite_duration = infinite;
        event.m_dominant_only = dominantOnly;
        event.m_gameplay_event = gameplay;*/

        //g_LunaCinematicEventsHost->TriggerCinematicEvent( &event );

      }
    }

    void StopAll()
    {
      if ( g_ValidCinematic )
      {
        g_LunaCinematicEventsHost->StopAll();
      }
    }

    void SetValidCinematic( bool valid )
    {
      g_ValidCinematic = valid;
    }

    void InitRemote()
    {
      if ( RuntimeConnection::IsConnected() )
      {
        g_LunaCinematicEventsHost->Init();
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void CinematicLoaded( const CinematicLoadedArgs& args )
    {
      if ( g_ValidCinematic )
      {
        //i32 index( args.m_Index );
        //g_LunaCinematicEventsHost->SetCinematicSettings( );
      }
    }

    void ClipChanged( const CinematicChangeArgs& args )
    {
      if ( g_ValidCinematic )
      {
        i32 clip = g_Manager->GetCurrentClipIndex();

        if ( clip != -1 && clip != g_CurrentClip )
        {
          i32 rpcClip( clip );
          //g_LunaCinematicEventsHost->SetCinematicClip( &rpcClip );
        }

        UpdateEvents();

        g_CurrentClip = clip;
      }
    }

    void FrameChanged( const FrameChangedArgs& args )
    {
      if ( g_ValidCinematic )
      {
        if ( args.m_Frame != g_CurrentFrame )
        {
          // not sure why this is needed.  it won't let me pass &args.m_Frame into SetFrame
          i32 frame( args.m_Frame );
          g_LunaCinematicEventsHost->SetFrame( &frame );
        }

        g_CurrentFrame = args.m_Frame;
      }
      else
      {
        g_CurrentFrame = g_Manager->GetStartFrame();
      }
    }

    void ProcessCinematicEvents()
    {
      IG::IGSerializer outFile( true );

      CinematicEventListPtr eventList = g_Manager->GetEventList();

      //create our chunk for the CineTrack and add the keyframe count and reserve the keyframes pointer
      Nocturnal::BasicBufferPtr track_buffer = outFile.AddChunk( IGG::FILECHUNK_CINEMATIC_CINETRACK, IG::CHUNK_TYPE_SINGLE , sizeof(IGPS3::CineTrack), 0 );
      track_buffer->AddU32( (u32)(u32)eventList->m_Events.size(), "CineTrack::m_keyframe_cnt" );//# of events
      Nocturnal::BasicBuffer::Location keyframesLoc;
      track_buffer->ReservePointer( keyframesLoc, 4, "CineTrack::m_keyframes" );

      //begin cycling the actual list of cinematic events
      for( u32 i = 0 ; i < (u32)eventList->m_Events.size(); ++i)
      {

        Asset::CinematicEventPtr thisEvent = eventList->m_Events[ i ];

        //check for a "-" in the actor name and resolve it to the proper actor (eg. human_base-2)
        std::string actorString = thisEvent->m_ActorName;
        size_t locDash = actorString.find( "-" );
        std::string actorName = actorString;
        int actorCount = 1;
        if ( locDash != std::string::npos  )
        {
          actorName = actorString.substr( 0, locDash );
          actorCount = atoi( actorString.substr( locDash + 1 ).c_str() );
        }

        //cycle the actor clips, and then cycle the joints once we've found the right actor
        int actorIndex = -1;
        int jointIndex = -1;

        V_string actorList = g_Manager->GetActors();
        for ( u32 ia = 0 ; ia < actorList.size() ; ++ia )
        {
          if ( actorList[ ia ] == actorString )
          {
            actorIndex = ia;
            break;
          }
        }


        /*V_AnimationClip::iterator clipIter = g_Manager->m_CinematicScene->m_AnimationClips.begin();
        V_AnimationClip::iterator clipEnd = g_Manager->m_CinematicScene->m_AnimationClips.end();
        for ( u32 clipCount = 0; clipIter != clipEnd; ++clipIter, ++clipCount )
        {
          if ( clipIter->m_EntityID != TUID::Null )
          {
            if ( clipIter->m_ClipName == actorName )
            {
              if ( actorCount == 1 )
              {
                //found the actor index
                actorIndex = clipCount;

                //now cycle the joints of the found actor for the joint index
                Content::Scene& scene = m_Processor.GetContentScene();
                Reflect::V_UID::const_iterator jointItr = clipIter->m_JointOrder->m_JointOrdering.begin();
                Reflect::V_UID::const_iterator jointEnd = clipIter->m_JointOrder->m_JointOrdering.end();
                for ( u32 jointCount = 0; jointItr != jointEnd; ++jointItr, ++jointCount )
                {
                  const Reflect::UID & jointId = (*jointItr);
                  Content::M_CompressedAnimation::const_iterator animItr = clipIter->m_Clip->m_JointCompressedAnimationMap.find( jointId );

                  Content::JointTransformPtr jt_trans = scene.Get< Content::JointTransform >( jointId );
                  std::string jointName = jt_trans.ReferencesObject() ? jt_trans->m_Name : "unknown";
                  if ( jointName == thisEvent->m_JointName )
                  {
                    //found the joint
                    jointIndex = jointCount;
                    break;
                  }
                }
                break;
              }
              else
              {
                actorCount--;
              }
            }
          }
        }*/

        //now that we've got all of our event data sorted, create the keyframes chunk, and add it's data and pointers
        Nocturnal::BasicBufferPtr keyframe_buffer = outFile.AddChunk( IGG::FILECHUNK_CINEMATIC_CINEKEYFRAME, IG::CHUNK_TYPE_ARRAY, sizeof( IGPS3::CineKeyframe ), 0 );
        if ( i == 0 )
        {
          Nocturnal::SmartBuffer::AddPointerFixup( keyframesLoc, keyframe_buffer->GetCurrentLocation() );
        }
        Nocturnal::BasicBuffer::Location eventLoc;
        keyframe_buffer->ReservePointer( eventLoc, 4, "CineKeyframe::m_event" );
        keyframe_buffer->AddI16( 1, "CineKeyframe::m_event_type" );//gets resolved at runtime
        keyframe_buffer->AddU16( thisEvent->m_Time, "CineKeyframe::m_frame" );

        //we're going to use the index here instead of a pointer and fix it up at run-time just like CineActor does
        keyframe_buffer->AddU32( actorIndex, "CineKeyframe::m_actor" );
        keyframe_buffer->AddU32( jointIndex, "CineKeyframe::m_joint" );

        //now sort out the runtime data (IGG::CineEvent)
        AttributeViewer< RuntimeDataAttribute > eventData( thisEvent );
        if ( !eventData.Valid() )
        {
          return;
        }
        std::string className = eventData->GetRuntimeData()->GetRuntimeClass();        
        UDTInstancePtr instance = eventData->GetRuntimeData()->GetRuntimeInstance(); 
        BasicBufferPtr runtime_buffer = outFile.AddChunk( IGG::FILECHUNK_CINEMATIC_CINEEVENT, IG::CHUNK_TYPE_SINGLE, 0, 0 );
        Nocturnal::SmartBuffer::AddPointerFixup( eventLoc, runtime_buffer->GetCurrentLocation() );
        if ( instance.ReferencesObject() )
        {  
          DataHandlers::WriteToBuffer( instance, runtime_buffer );
        }
      }
      std::string outFilePath = Finder::ProjectTemp() + "cineevent-temp.dat";
      outFile.WriteToFile( outFilePath.c_str() );
    }


    void UpdateEvents()
    {
      if ( g_ValidCinematic )
      {
        ProcessCinematicEvents();

        /*std::vector< RPC::CinematicEvent > rpcEvents;

        CinematicEventListPtr eventList = g_Manager->GetEventList();
        IGPS3::CineTrack rpcTrack;
        u32 keyframeCount = ConvertEndian( (u32)eventList->m_Events.size(), true );
        rpcTrack.m_keyframe_cnt = keyframeCount;
        rpcTrack.m_keyframes = new IGPS3::CineKeyframe;
        IGPS3::CineKeyframe* rpcKeyframe = rpcTrack.m_keyframes;
        for each ( const CinematicEventPtr& evt in eventList->m_Events )
        {
          //rpcKeyframe = rpcTrack.m_keyframes;
          rpcKeyframe->m_event_type = 0;
          rpcKeyframe->m_frame = ConvertEndian( (u16)evt->m_Time, true );
          u8 actorIndex = ConvertEndian( (u8)1, true );
          rpcKeyframe->m_actor = (IGPS3::MobyInstance *)actorIndex;
          rpcKeyframe->m_joint = ConvertEndian( (u32)1, true );
          rpcKeyframe++;


          //rpcEvents.push_back( event );
        }

        u8* data = NULL;

        //if ( rpcTrack->ReferencesObject() )
        //{
          data = (u8*)&rpcTrack;//.front();
        //}

          
        */
        g_LunaCinematicEventsHost->SetCinematicEvents();
        
      }
    }

    void EventExistence( const EventExistenceArgs& args )
    {
      UpdateEvents();
    }

    void EventsChanged( const EventsChangedArgs& args )
    {
      if ( g_Manager->GetUpdateProperties() )
      {
        UpdateEvents();
      }
    }

    void AssetOpened( const CinematicFileChangeArgs& args )
    {
      InitRemote();
    }

    void AssetClosed( const CinematicFileChangeArgs& args )
    {
      g_ValidCinematic = false;
    }
  }
}



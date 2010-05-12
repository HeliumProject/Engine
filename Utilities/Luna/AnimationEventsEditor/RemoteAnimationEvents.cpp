#include "Precompile.h"
#include "RemoteAnimationEvents.h"
#include "Live/RuntimeConnection.h"
#include "AnimationEventsManager.h"
#include "AnimationEventsDocument.h"

#include "IPC/TCP.h"
#include "IPC/Pipe.h"
#include "File/Manager.h"
#include "AppUtils/AppUtils.h"
#include "Console/Console.h"

using namespace Luna;
using namespace Asset;
using namespace RPC;

namespace Luna
{
  namespace RemoteAnimationEvents
  {
    static bool g_Initialized = false;
    bool g_ValidMoby = false;
    AnimationEventsManager* g_Manager = NULL;

    i32 g_CurrentClip = -1;
    i32 g_CurrentFrame = -1;

    ILunaAnimationEventsHost* g_LunaAnimationEventsHost = NULL;
    ILunaAnimationEventsTool* g_LunaAnimationEventsTool = NULL;

    struct LunaAnimationEventsTool : ILunaAnimationEventsTool
    {
      LOCAL_IMPL(LunaAnimationEventsTool, ILunaAnimationEventsTool);

      virtual void SetMoby(SetMobyParams* params, u8* data, u32 size);
      virtual void SetAnimClip(i32* clip);
      virtual void SetFrame(i32* frame);
    };

    void LunaAnimationEventsTool::SetMoby( SetMobyParams* params, u8* data, u32 size )
    {
      AnimationEventsDocumentPtr file = g_Manager->GetFile();

      if ( file->m_MobyClass->m_AssetClassID == params->m_Tuid )
      {
        RPC::AnimClipInfo* clipInfo = (RPC::AnimClipInfo*)data;
        for ( int i = 0;i < params->m_NumClips; ++i )
        {
          clipInfo[ i ].m_frames = ConvertEndian( clipInfo[ i ].m_frames, true );
        }

        g_ValidMoby = g_Manager->ViewerConnected( params->m_NumClips, clipInfo );

        if ( g_ValidMoby )
        {
          i32 clip( g_Manager->GetCurrentClipIndex() );
          i32 frame( g_Manager->GetCurrentFrame() );
          i32 updateClass( g_Manager->GetUpdateClassVTGenIndex() );
          g_LunaAnimationEventsHost->SetAnimClip( &clip );
          g_LunaAnimationEventsHost->SetFrame( &frame );
          g_LunaAnimationEventsHost->SetUpdateClass( &updateClass );

          UpdateEvents();
        }
      }
      else
      {
        g_ValidMoby = false;
      }
    }
    
    void LunaAnimationEventsTool::SetAnimClip( i32* clip )
    {
      if ( g_ValidMoby )
      {
        g_CurrentClip = *clip;
        g_Manager->SetClip( g_CurrentClip );

        UpdateEvents();
      }
    }

    void LunaAnimationEventsTool::SetFrame( i32* frame )
    {
      if ( g_ValidMoby )
      {
        g_CurrentFrame = *frame;
        g_Manager->SetFrame( g_CurrentFrame );
      }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool Initialize( AnimationEventsManager& manager )
    {
      if( g_Initialized )
      {
        return true;
      }

      g_Initialized = true;

      g_Manager = &manager;

      RPC::Host* host = RuntimeConnection::GetHost();

      // create a local implementation of our functionality
      g_LunaAnimationEventsHost = RuntimeConnection::GetRemoteAnimationEventsView();
      g_LunaAnimationEventsTool = new LunaAnimationEventsTool( host );
      host->SetLocalInterface( kLunaAnimationEventsTool, g_LunaAnimationEventsTool );

      // set up event listeners
      g_Manager->AddAnimClipChangeListener( AnimClipChangeSignature::Delegate ( &ClipChanged ) );
      g_Manager->AddFrameChangedListener( FrameChangedSignature::Delegate ( &FrameChanged ) );
      g_Manager->AddEventExistenceListener( EventExistenceSignature::Delegate ( &EventExistence ) );
      g_Manager->AddEventsChangedListener( EventsChangedSignature::Delegate ( &EventsChanged ) );
      g_Manager->AddMobyOpenedListener( MobyFileChangeSignature::Delegate ( &AssetOpened ) );
      g_Manager->AddMobyClosedListener( MobyFileChangeSignature::Delegate ( &AssetClosed ) );
      g_Manager->AddUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( &UpdateClassChanged ) );

      return g_Initialized;
    }

    void Cleanup()
    {
      if (g_Initialized)
      {
        delete g_LunaAnimationEventsTool;

        g_LunaAnimationEventsHost = NULL;
        g_LunaAnimationEventsTool = NULL;

        // remove event listeners
        g_Manager->RemoveAnimClipChangeListener( AnimClipChangeSignature::Delegate ( &ClipChanged ) );
        g_Manager->RemoveFrameChangedListener( FrameChangedSignature::Delegate ( &FrameChanged ) );
        g_Manager->RemoveEventExistenceListener( EventExistenceSignature::Delegate ( &EventExistence ) );
        g_Manager->RemoveEventsChangedListener( EventsChangedSignature::Delegate ( &EventsChanged ) );
        g_Manager->RemoveMobyOpenedListener( MobyFileChangeSignature::Delegate ( &AssetOpened ) );
        g_Manager->RemoveMobyClosedListener( MobyFileChangeSignature::Delegate ( &AssetClosed ) );
        g_Manager->RemoveUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( &UpdateClassChanged ) );
        
        g_Initialized = false;
      }
    }

    void SetPlayRate( float rate )
    {
      if ( g_ValidMoby )
      {
        g_LunaAnimationEventsHost->SetPlayRate( &rate );
      }
    }

    void PlayEvent( const std::string& eventType, const float value, const u32 value2, bool untrigger, bool infinite, bool dominantOnly, bool gameplay )
    {
      if ( g_ValidMoby )
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

        RPC::AnimationEvent event;
        memset( &event, 0, sizeof( event ) );
        event.m_event_type = eventTypeIndex;
        event.m_untrigger = untrigger;
        event.m_value = (i32) value;
        event.m_time = 0;
        event.m_infinite_duration = infinite;
        event.m_dominant_only = dominantOnly;
        event.m_gameplay_event = gameplay;

        g_LunaAnimationEventsHost->TriggerAnimationEvent( &event );

      }
    }

    void StopAll()
    {
      if ( g_ValidMoby )
      {
        g_LunaAnimationEventsHost->StopAll();
      }
    }

    void SetValidMoby( bool valid )
    {
      g_ValidMoby = valid;
    }

    void InitRemote()
    {
      if ( RuntimeConnection::IsConnected() )
      {
        g_LunaAnimationEventsHost->Init();
      }
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void UpdateClassChanged( const UpdateClassChangedArgs& args )
    {
      if ( g_ValidMoby )
      {
        i32 index( args.m_Index );
        g_LunaAnimationEventsHost->SetUpdateClass( &index );
      }
    }

    void ClipChanged( const AnimClipChangeArgs& args )
    {
      if ( g_ValidMoby )
      {
        i32 clip = g_Manager->GetCurrentClipIndex();

        if ( clip != -1 && clip != g_CurrentClip )
        {
          i32 rpcClip( clip );
          g_LunaAnimationEventsHost->SetAnimClip( &rpcClip );
        }

        UpdateEvents();

        g_CurrentClip = clip;
      }
    }

    void FrameChanged( const FrameChangedArgs& args )
    {
      if ( g_ValidMoby )
      {
        if ( args.m_Frame != g_CurrentFrame )
        {
          // not sure why this is needed.  it won't let me pass &args.m_Frame into SetFrame
          i32 frame( args.m_Frame );
          g_LunaAnimationEventsHost->SetFrame( &frame );
        }

        g_CurrentFrame = args.m_Frame;
      }
    }

    void UpdateEvents()
    {
      if ( g_ValidMoby )
      {
        std::vector< RPC::AnimationEvent > rpcEvents;

        const AnimationClipDataPtr& clip = g_Manager->GetCurrentClip();
        if ( clip.ReferencesObject() )
        {
          AnimationEventListPtr eventList = g_Manager->GetEventList();
          for each ( const AnimationEventPtr& evt in eventList->m_Events )
          {
            RPC::AnimationEvent event;

            i32 eventType;

            const Symbol::EnumPtr& event_types_enum= g_Manager->GetEventTypesEnum(evt->m_GameplayEvent);
            if (event_types_enum && !event_types_enum->GetElemValue(evt->m_EventType, eventType))
            {

              std::stringstream text;
              text << "Unrecognized Event Type " << evt->m_EventType << ".";
              Console::Warning(text.str().c_str());
              continue;
            }

            event.m_event_type = ConvertEndian( eventType, true );
            event.m_untrigger = evt->m_Untrigger;
            event.m_value = ConvertEndian( (int)evt->m_Value, true );
            event.m_gameplay_value_2 = ConvertEndian( evt->m_GameplayValue2, true );
            event.m_gameplay_value_3 = ConvertEndian( evt->m_GameplayValue3, true );
            event.m_gameplay_value_4 = ConvertEndian( evt->m_GameplayValue4, true );
            event.m_gameplay_float_value_1 = ConvertEndian( evt->m_GameplayFloatValue1, true );
            event.m_gameplay_float_value_2 = ConvertEndian( evt->m_GameplayFloatValue2, true );
            event.m_gameplay_float_value_3 = ConvertEndian( evt->m_GameplayFloatValue3, true );
            event.m_gameplay_float_value_4 = ConvertEndian( evt->m_GameplayFloatValue4, true );
            event.m_time = ConvertEndian( (u16)evt->m_Time, true );
            event.m_infinite_duration = evt->m_InfiniteDuration;
            event.m_dominant_only = evt->m_DominantOnly;
            event.m_gameplay_event = evt->m_GameplayEvent;

            rpcEvents.push_back( event );
          }

          u8* data = NULL;

          if ( !rpcEvents.empty() )
          {
            data = (u8*)&rpcEvents.front();
          }

          g_LunaAnimationEventsHost->SetAnimationEvents( data, (u32)rpcEvents.size() * sizeof( RPC::AnimationEvent ) );
        }
      }
    }

    void EventExistence( const EventExistenceArgs& args )
    {
      UpdateEvents();
    }

    void EventsChanged( const EventsChangedArgs& args )
    {
      UpdateEvents();
    }

    void AssetOpened( const MobyFileChangeArgs& args )
    {
      InitRemote();
    }

    void AssetClosed( const MobyFileChangeArgs& args )
    {
      g_ValidMoby = false;
    }
  }
}



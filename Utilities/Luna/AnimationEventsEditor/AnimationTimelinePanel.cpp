#include "Precompile.h"

#include "AnimationTimelinePanel.h"
#include "AnimationEventsPanels.h"
#include "AnimationTimelineSlider.h"
#include "AnimationEventsEditorIDs.h"
#include "AnimationEventsEditor.h"

#include "AnimationEventsUndoCommands.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;
using namespace Asset;

BEGIN_EVENT_TABLE( AnimationEventsTimelinePanel, wxEvtHandler )
EVT_COMMAND_SCROLL( AnimationEventsEditorIDs::TimelineSlider, OnSliderValueChanged )
END_EVENT_TABLE()

AnimationEventsTimelinePanel::AnimationEventsTimelinePanel( AnimationEventsManager& manager, AnimationTimelinePanel* panel )
: AnimationPanelBase( manager, panel )
, m_MarkerMouseDown( false )
, m_MovedEvents( false )
, m_SliderMouseDown( false )
, m_LastMouseTicLocation( 0 )
, m_LastWidth( 0 )
, m_LastHeight( 0 )
{
  m_Manager.AddAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationEventsTimelinePanel::ClipChanged ) );
  m_Manager.AddFrameChangedListener( FrameChangedSignature::Delegate ( this, &AnimationEventsTimelinePanel::FrameChanged ) );
  m_Manager.AddEventExistenceListener( EventExistenceSignature::Delegate ( this, &AnimationEventsTimelinePanel::EventExistence ) );
  m_Manager.AddSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &AnimationEventsTimelinePanel::SelectionChanged ) );
  m_Manager.AddEventsChangedListener( EventsChangedSignature::Delegate ( this, &AnimationEventsTimelinePanel::EventsChanged ) );
  m_Manager.AddMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsTimelinePanel::AssetOpened ) );
  m_Manager.AddMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsTimelinePanel::AssetClosed ) );


  m_Panel->m_TimeSlider->SetId( AnimationEventsEditorIDs::TimelineSlider );
  m_Panel->m_MarkersPanel->SetId( AnimationEventsEditorIDs::TimelineMarkersPanel );

  m_Panel->m_MarkersPanel->Connect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_PAINT, wxPaintEventHandler( AnimationEventsTimelinePanel::OnMarkersPaint ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationEventsTimelinePanel::OnMarkerMouseDown ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_UP, wxMouseEventHandler( AnimationEventsTimelinePanel::OnMarkerMouseUp ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_MOTION, wxMouseEventHandler( AnimationEventsTimelinePanel::OnMarkerMouseMove ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_SIZE, wxSizeEventHandler( AnimationEventsTimelinePanel::OnSize ), NULL, this );

  m_Panel->m_TimeSlider->Connect( AnimationEventsEditorIDs::TimelineSlider, wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationEventsTimelinePanel::OnSliderMouseDown ), NULL, this );
  m_Panel->m_TimeSlider->Connect( AnimationEventsEditorIDs::TimelineSlider, wxEVT_LEFT_UP, wxMouseEventHandler( AnimationEventsTimelinePanel::OnSliderMouseUp ), NULL, this );


  // char handlers
  m_Panel->m_MarkersPanel->Connect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_CHAR, wxKeyEventHandler( AnimationEventsTimelinePanel::OnChar ), NULL, this );
  m_Panel->m_TimeSlider->Connect( AnimationEventsEditorIDs::TimelineSlider, wxEVT_CHAR, wxKeyEventHandler( AnimationEventsTimelinePanel::OnChar ), NULL, this );

  m_Panel->m_PauseButton->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "pause_16.png" ) );
  m_Panel->m_PlayButton->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "play_16.png" ) );

  m_Panel->m_PauseButton->Connect( m_Panel->m_PauseButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsTimelinePanel::OnPause ), NULL, this );
  m_Panel->m_PlayButton->Connect( m_Panel->m_PlayButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsTimelinePanel::OnPlay ), NULL, this );

  m_Panel->m_GotoFrameTextBox->Connect( m_Panel->m_GotoFrameTextBox->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( AnimationEventsTimelinePanel::OnGotoFrame ), NULL, this );

  DisableAll();
}

AnimationEventsTimelinePanel::~AnimationEventsTimelinePanel()
{
  m_Manager.RemoveAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationEventsTimelinePanel::ClipChanged ) );
  m_Manager.RemoveFrameChangedListener( FrameChangedSignature::Delegate ( this, &AnimationEventsTimelinePanel::FrameChanged ) );
  m_Manager.RemoveEventExistenceListener( EventExistenceSignature::Delegate ( this, &AnimationEventsTimelinePanel::EventExistence ) );
  m_Manager.RemoveSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &AnimationEventsTimelinePanel::SelectionChanged ) );
  m_Manager.RemoveEventsChangedListener( EventsChangedSignature::Delegate ( this, &AnimationEventsTimelinePanel::EventsChanged ) );
  m_Manager.RemoveMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsTimelinePanel::AssetOpened ) );
  m_Manager.RemoveMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsTimelinePanel::AssetClosed ) );

  m_Panel->m_MarkersPanel->Disconnect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_PAINT, wxPaintEventHandler( AnimationEventsTimelinePanel::OnMarkersPaint ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationEventsTimelinePanel::OnMarkerMouseDown ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_UP, wxMouseEventHandler( AnimationEventsTimelinePanel::OnMarkerMouseUp ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_MOTION, wxMouseEventHandler( AnimationEventsTimelinePanel::OnMarkerMouseMove ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_SIZE, wxSizeEventHandler( AnimationEventsTimelinePanel::OnSize ), NULL, this );

  m_Panel->m_TimeSlider->Disconnect( AnimationEventsEditorIDs::TimelineSlider, wxEVT_LEFT_DOWN, wxMouseEventHandler( AnimationEventsTimelinePanel::OnSliderMouseDown ), NULL, this );
  m_Panel->m_TimeSlider->Disconnect( AnimationEventsEditorIDs::TimelineSlider, wxEVT_LEFT_UP, wxMouseEventHandler( AnimationEventsTimelinePanel::OnSliderMouseUp ), NULL, this );


  // char handlers
  m_Panel->m_MarkersPanel->Disconnect( AnimationEventsEditorIDs::TimelineMarkersPanel, wxEVT_KEY_DOWN, wxKeyEventHandler( AnimationEventsTimelinePanel::OnChar ), NULL, this );
  m_Panel->m_TimeSlider->Disconnect( AnimationEventsEditorIDs::TimelineSlider, wxEVT_KEY_DOWN, wxKeyEventHandler( AnimationEventsTimelinePanel::OnChar ), NULL, this );

  m_Panel->m_PauseButton->Disconnect( m_Panel->m_PauseButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsTimelinePanel::OnPause ) );
  m_Panel->m_PlayButton->Disconnect( m_Panel->m_PlayButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsTimelinePanel::OnPlay ) );

  m_Panel->m_GotoFrameTextBox->Disconnect( m_Panel->m_GotoFrameTextBox->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( AnimationEventsTimelinePanel::OnGotoFrame ) );
}

void AnimationEventsTimelinePanel::UpdateMarkers()
{
  m_Markers.clear();

  if ( m_CurrentClip.ReferencesObject() )
  {
    typedef std::map< u32, std::vector< AnimationEventPtr > > M_FrameToEventVector;

    M_FrameToEventVector events;
    const AnimationEventListPtr& eventList = m_Manager.GetEventList();
    for each ( const AnimationEventPtr& evt in eventList->m_Events )
    {
      events[ evt->m_Time ].push_back( evt );
    }

    OS_AnimationEvent& selection = m_Manager.GetSelection();

    for each ( M_FrameToEventVector::value_type val in events  )
    {
      // figure out the height of each rect, based on the number of events in the frame.
      wxRect sliderBounds = m_Panel->m_TimeSlider->GetRect();
      wxRect markerBounds = m_Panel->m_MarkersPanel->GetRect();

      int rectHeight = markerBounds.GetHeight() / (int)val.second.size();

      int i = 0;
      for each ( const AnimationEventPtr& evt in val.second )
      {
        int x = m_Panel->m_TimeSlider->ThumbPosToPixel( evt->m_Time );

        // convert from the time slider's pixel value to the marker panel's pixel value
        x += sliderBounds.GetLeft();
        x -= markerBounds.GetLeft();

        Marker marker;
        marker.m_Event = evt;
        marker.m_Color = m_Manager.GetEventColor( evt->m_EventType );
        marker.m_Rect = wxRect( x - 4, i*rectHeight, 8, rectHeight );

        if ( selection.Contains( marker.m_Event ) )
        {
          marker.m_Selected = true;
        }

        m_Markers.push_back( marker );

        ++i;
      }
    }

  }
}

void AnimationEventsTimelinePanel::SelectionChanged( const EventSelectionChangedArgs& args )
{
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void AnimationEventsTimelinePanel::EventExistence( const EventExistenceArgs& args )
{
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void AnimationEventsTimelinePanel::ClipChanged( const AnimClipChangeArgs& args )
{
  if ( args.m_Clip.Ptr() == m_CurrentClip.Ptr() )
  {
    return;
  }

  m_CurrentClip = args.m_Clip;

  m_Panel->m_TimeSlider->SetRange( 0, m_Manager.GetNumFrames( m_CurrentClip ) - 1 );

  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void AnimationEventsTimelinePanel::EventsChanged( const EventsChangedArgs& args )
{
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void AnimationEventsTimelinePanel::FrameChanged( const FrameChangedArgs& args )
{
  if ( args.m_Frame == m_Panel->m_TimeSlider->GetValue() )
  {
    return;
  }

  m_Panel->m_TimeSlider->SetValue( args.m_Frame );
}

void AnimationEventsTimelinePanel::EnableAll()
{
  m_Panel->m_TimeSlider->Enable();
  m_Panel->m_MarkersPanel->Enable();
  m_Panel->m_PauseButton->Enable();
  m_Panel->m_PlayButton->Enable();
  m_Panel->m_GotoFrameTextBox->Enable();
}

void AnimationEventsTimelinePanel::DisableAll()
{
  m_Panel->m_TimeSlider->SetRange( 0, 100 );
  m_Panel->m_TimeSlider->Disable();
  m_Panel->m_MarkersPanel->Disable();
  m_Panel->m_PauseButton->Disable();
  m_Panel->m_PlayButton->Disable();
  m_Panel->m_GotoFrameTextBox->Disable();

}

void AnimationEventsTimelinePanel::AssetOpened( const MobyFileChangeArgs& args )
{
  EnableAll();
}

void AnimationEventsTimelinePanel::AssetClosed( const MobyFileChangeArgs& args )
{
  DisableAll();

  m_Markers.clear();

  m_CurrentClip = NULL;

  m_Panel->m_MarkersPanel->Refresh();
}

void AnimationEventsTimelinePanel::OnSliderValueChanged( wxScrollEvent& e )
{
  m_Manager.SetPlayRate( 0.0 );
  m_Manager.SetFrame( e.GetInt() );
}

const AnimationEventPtr AnimationEventsTimelinePanel::FindEvent( int x, int y )
{
  for each ( const Marker& marker in m_Markers )
  {
    if ( marker.m_Rect.Inside( x, y ) )
    {
      return marker.m_Event;
    }
  }

  return NULL;
}

void AnimationEventsTimelinePanel::OnMarkerMouseDown( wxMouseEvent& e )
{
  e.Skip();

  m_MultiSelect = wxIsCtrlDown() || wxIsShiftDown();

  m_LastSelected = FindEvent( e.GetX(), e.GetY() );

  m_LastMouseTicLocation = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );

  m_UndoCommand = new AnimationEventsChangedCommand( m_Manager );

  m_MarkerMouseDown = true;
}

void AnimationEventsTimelinePanel::OnMarkerMouseUp( wxMouseEvent& e )
{
  e.Skip();

  if ( m_MarkerMouseDown )
  {
    if ( m_MovedEvents )
    {
      m_Manager.GetUndoQueue().Push( m_UndoCommand );
    }
    else
    {
      if ( m_LastSelected.ReferencesObject() )
      {
        if ( m_MultiSelect )
        {
          OS_AnimationEvent& selection = m_Manager.GetSelection();

          if ( selection.Contains( m_LastSelected ) )
          {
            m_Manager.RemoveSelection( m_LastSelected );
          }
          else
          {
            m_Manager.AddSelection( m_LastSelected );
          }
        }
        else
        {
          OS_AnimationEvent events;
          events.Append( m_LastSelected );

          m_Manager.SetSelection( events );
        }
      }
      else if ( !m_MultiSelect )
      {
        m_Manager.ClearSelection();
      }
    }

    m_MarkerMouseDown = false;
    m_MovedEvents = false;
  }
}

void AnimationEventsTimelinePanel::OnMarkerMouseMove( wxMouseEvent& e )
{
  e.Skip();

  if ( m_MarkerMouseDown )
  {
    m_MovedEvents = true;

    const OS_AnimationEvent& selection = m_Manager.GetSelection();

    int newTicLoc = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );

    if ( newTicLoc != m_LastMouseTicLocation )
    {
      int diff = newTicLoc - m_LastMouseTicLocation;
      int numFrames = m_Manager.GetNumFrames( m_CurrentClip );

      OS_AnimationEvent::Iterator eventIt = selection.Begin();
      OS_AnimationEvent::Iterator eventEnd = selection.End();
      for ( ; eventIt != eventEnd; ++eventIt )
      {
        const AnimationEventPtr& evt = *eventIt;
        evt->m_Time += diff;

        if ( evt->m_Time < 0 )
        {
          evt->m_Time = 0;
        }
        else if ( evt->m_Time > numFrames )
        {
          evt->m_Time = numFrames;
        }
      }

      m_Manager.EventsChanged( selection );
    }
  }

  m_LastMouseTicLocation = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );
}

void AnimationEventsTimelinePanel::OnMarkersPaint( wxPaintEvent& e )
{
  // this sucks, but for some reason we're not getting every size event
  wxRect rect = m_Panel->m_MarkersPanel->GetRect();
  if ( m_LastWidth != rect.GetWidth() || m_LastHeight != rect.GetHeight() )
  {
    UpdateMarkers();

    m_LastWidth = rect.GetWidth();
    m_LastHeight = rect.GetHeight();
  }

  wxPaintDC pdc( m_Panel->m_MarkersPanel );

#if wxUSE_GRAPHICS_CONTEXT
  wxGCDC gdc( pdc ) ;
  wxDC &dc = (wxDC&) gdc;
#else
  wxDC &dc = pdc ;
#endif

  m_Panel->m_MarkersPanel->PrepareDC( dc );

  dc.Clear();

  DrawEventMarkers( dc );
}

void AnimationEventsTimelinePanel::DrawEventMarkers( wxDC& dc )
{
  if ( m_CurrentClip.ReferencesObject() )
  { 
    // draw selection rectangles
    for each ( const Marker& marker in m_Markers )
    {
      if ( marker.m_Selected )
      {
        wxBrush selectBrush( *wxWHITE );
        dc.SetBrush( selectBrush );

        dc.DrawRectangle( marker.m_Rect.GetX() - 3, marker.m_Rect.GetY() - 3, marker.m_Rect.GetWidth() + 6, marker.m_Rect.GetHeight() + 6 );
      }
    }

    // now draw the main rectangles
    for each ( const Marker& marker in m_Markers )
    {
      wxBrush mainBrush( marker.m_Color );
      wxBrush gameplayBrush( marker.m_Color );
      gameplayBrush.SetStyle( wxCROSS_HATCH );
      dc.SetBrush( mainBrush );

      if ( marker.m_Event->m_Untrigger )
      {
        dc.DrawRoundedRectangle( marker.m_Rect, 20.0 );
      }
      else
      {
        if ( marker.m_Event->m_GameplayEvent )
        {
          dc.SetBrush( gameplayBrush ); 
        }

        dc.DrawRectangle( marker.m_Rect );
      }
    }
    
  }
}

void AnimationEventsTimelinePanel::OnSliderMouseDown( wxMouseEvent& e )
{
  e.Skip();

  if ( !m_SliderMouseDown )
  {
    m_SliderMouseDown = true;
    m_LastSliderValue = m_Panel->m_TimeSlider->GetValue();
  }
}

void AnimationEventsTimelinePanel::OnSliderMouseUp( wxMouseEvent& e )
{
  e.Skip();

  if ( m_SliderMouseDown )
  {
    m_SliderMouseDown = false;

    if ( m_LastSliderValue != m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() ) && m_Manager.GetCurrentClip().ReferencesObject() )
    {
      AnimationEventsChangedCommand* undoCommand = new AnimationEventsChangedCommand( m_Manager );
      undoCommand->m_Frame = m_LastSliderValue;
      m_Manager.GetUndoQueue().Push( undoCommand );
    }
  }
}

void AnimationEventsTimelinePanel::OnChar( wxKeyEvent& e )
{
  m_Manager.GetEditor()->ProcessEvent( e );
}

void AnimationEventsTimelinePanel::OnSize( wxSizeEvent& event )
{
  event.Skip();

  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void AnimationEventsTimelinePanel::OnPause( wxCommandEvent& e )
{
  m_Manager.SetPlayRate( 0.0 );
}

void AnimationEventsTimelinePanel::OnPlay( wxCommandEvent& e )
{
  m_Manager.SetPlayRate( 1.0 );
}

void AnimationEventsTimelinePanel::OnGotoFrame( wxCommandEvent& e )
{
  m_Manager.SetPlayRate( 0.0 );
  m_Manager.SetFrame( atoi( m_Panel->m_GotoFrameTextBox->GetValue().c_str() ) );
}
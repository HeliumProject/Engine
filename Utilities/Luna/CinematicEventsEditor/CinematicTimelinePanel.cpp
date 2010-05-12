#include "Precompile.h"

#include "CinematicTimelinePanel.h"
#include "CinematicEventsPanels.h"
#include "CinematicTimelineSlider.h"
#include "CinematicEventsEditorIDs.h"
#include "CinematicEventsEditor.h"

#include "CinematicEventsUndoCommands.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"
#include "UIToolKit/ImageManager.h"

using namespace Luna;
using namespace Asset;

BEGIN_EVENT_TABLE( CinematicEventsTimelinePanel, wxEvtHandler )
EVT_COMMAND_SCROLL( CinematicEventsEditorIDs::TimelineSlider, OnSliderValueChanged )
END_EVENT_TABLE()

CinematicEventsTimelinePanel::CinematicEventsTimelinePanel( CinematicEventsManager& manager, CinematicTimelinePanel* panel )
: CinematicPanelBase( manager, panel )
, m_MarkerMouseDown( false )
, m_MovedEvents( false )
, m_SliderMouseDown( false )
, m_LastMouseTicLocation( 0 )
, m_LastWidth( 0 )
, m_LastHeight( 0 )
, m_MouseWasDown( false )
, m_PreMoveMouseTicLocation( 0 )
{
  m_Manager.AddCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicEventsTimelinePanel::ClipChanged ) );
  m_Manager.AddFrameChangedListener( FrameChangedSignature::Delegate ( this, &CinematicEventsTimelinePanel::FrameChanged ) );
  m_Manager.AddEventExistenceListener( EventExistenceSignature::Delegate ( this, &CinematicEventsTimelinePanel::EventExistence ) );
  m_Manager.AddSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &CinematicEventsTimelinePanel::SelectionChanged ) );
  m_Manager.AddEventsChangedListener( EventsChangedSignature::Delegate ( this, &CinematicEventsTimelinePanel::EventsChanged ) );
  m_Manager.AddCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicEventsTimelinePanel::AssetOpened ) );
  m_Manager.AddCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicEventsTimelinePanel::AssetClosed ) );
  m_Manager.AddCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicEventsTimelinePanel::CinematicLoaded ) );

  m_Panel->m_TimeSlider->SetId( CinematicEventsEditorIDs::TimelineSlider );
  m_Panel->m_MarkersPanel->SetId( CinematicEventsEditorIDs::TimelineMarkersPanel );

  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_PAINT, wxPaintEventHandler( CinematicEventsTimelinePanel::OnMarkersPaint ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_DOWN, wxMouseEventHandler( CinematicEventsTimelinePanel::OnMarkerMouseDown ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_UP, wxMouseEventHandler( CinematicEventsTimelinePanel::OnMarkerMouseUp ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_MOTION, wxMouseEventHandler( CinematicEventsTimelinePanel::OnMarkerMouseMove ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_SIZE, wxSizeEventHandler( CinematicEventsTimelinePanel::OnSize ), NULL, this );
  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_MOUSEWHEEL, wxMouseEventHandler( CinematicEventsTimelinePanel::OnZoom ), NULL, this );

  m_Panel->m_TimeSlider->Connect( CinematicEventsEditorIDs::TimelineSlider, wxEVT_LEFT_DOWN, wxMouseEventHandler( CinematicEventsTimelinePanel::OnSliderMouseDown ), NULL, this );
  m_Panel->m_TimeSlider->Connect( CinematicEventsEditorIDs::TimelineSlider, wxEVT_LEFT_UP, wxMouseEventHandler( CinematicEventsTimelinePanel::OnSliderMouseUp ), NULL, this );


  // char handlers
  m_Panel->m_MarkersPanel->Connect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_CHAR, wxKeyEventHandler( CinematicEventsTimelinePanel::OnChar ), NULL, this );
  m_Panel->m_TimeSlider->Connect( CinematicEventsEditorIDs::TimelineSlider, wxEVT_CHAR, wxKeyEventHandler( CinematicEventsTimelinePanel::OnChar ), NULL, this );

  m_Panel->m_PauseButton->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "pause_16.png" ) );
  m_Panel->m_PlayButton->SetBitmapLabel( UIToolKit::GlobalImageManager().GetBitmap( "play_16.png" ) );

  m_Panel->m_PauseButton->Connect( m_Panel->m_PauseButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicEventsTimelinePanel::OnPause ), NULL, this );
  m_Panel->m_PlayButton->Connect( m_Panel->m_PlayButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicEventsTimelinePanel::OnPlay ), NULL, this );

  m_Panel->m_GotoFrameTextBox->Connect( m_Panel->m_GotoFrameTextBox->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CinematicEventsTimelinePanel::OnGotoFrame ), NULL, this );
  m_Panel->m_MoveSelectedFrames->Connect( m_Panel->m_MoveSelectedFrames->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CinematicEventsTimelinePanel::OnMoveSelectedFrames ), NULL, this );
  
  m_Panel->m_ZoomStart->Connect( m_Panel->m_ZoomStart->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CinematicEventsTimelinePanel::OnSetZoomStart ), NULL, this );
  m_Panel->m_ZoomEnd->Connect( m_Panel->m_ZoomEnd->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CinematicEventsTimelinePanel::OnSetZoomEnd ), NULL, this );

  DisableAll();
}

CinematicEventsTimelinePanel::~CinematicEventsTimelinePanel()
{
  m_Manager.RemoveCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicEventsTimelinePanel::ClipChanged ) );
  m_Manager.RemoveFrameChangedListener( FrameChangedSignature::Delegate ( this, &CinematicEventsTimelinePanel::FrameChanged ) );
  m_Manager.RemoveEventExistenceListener( EventExistenceSignature::Delegate ( this, &CinematicEventsTimelinePanel::EventExistence ) );
  m_Manager.RemoveSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &CinematicEventsTimelinePanel::SelectionChanged ) );
  m_Manager.RemoveEventsChangedListener( EventsChangedSignature::Delegate ( this, &CinematicEventsTimelinePanel::EventsChanged ) );
  m_Manager.RemoveCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicEventsTimelinePanel::AssetOpened ) );
  m_Manager.RemoveCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicEventsTimelinePanel::AssetClosed ) );
  m_Manager.RemoveCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicEventsTimelinePanel::CinematicLoaded ) );

  m_Panel->m_MarkersPanel->Disconnect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_PAINT, wxPaintEventHandler( CinematicEventsTimelinePanel::OnMarkersPaint ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_DOWN, wxMouseEventHandler( CinematicEventsTimelinePanel::OnMarkerMouseDown ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_LEFT_UP, wxMouseEventHandler( CinematicEventsTimelinePanel::OnMarkerMouseUp ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_MOTION, wxMouseEventHandler( CinematicEventsTimelinePanel::OnMarkerMouseMove ), NULL, this );
  m_Panel->m_MarkersPanel->Disconnect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_SIZE, wxSizeEventHandler( CinematicEventsTimelinePanel::OnSize ), NULL, this );

  m_Panel->m_TimeSlider->Disconnect( CinematicEventsEditorIDs::TimelineSlider, wxEVT_LEFT_DOWN, wxMouseEventHandler( CinematicEventsTimelinePanel::OnSliderMouseDown ), NULL, this );
  m_Panel->m_TimeSlider->Disconnect( CinematicEventsEditorIDs::TimelineSlider, wxEVT_LEFT_UP, wxMouseEventHandler( CinematicEventsTimelinePanel::OnSliderMouseUp ), NULL, this );


  // char handlers
  m_Panel->m_MarkersPanel->Disconnect( CinematicEventsEditorIDs::TimelineMarkersPanel, wxEVT_KEY_DOWN, wxKeyEventHandler( CinematicEventsTimelinePanel::OnChar ), NULL, this );
  m_Panel->m_TimeSlider->Disconnect( CinematicEventsEditorIDs::TimelineSlider, wxEVT_KEY_DOWN, wxKeyEventHandler( CinematicEventsTimelinePanel::OnChar ), NULL, this );

  m_Panel->m_PauseButton->Disconnect( m_Panel->m_PauseButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicEventsTimelinePanel::OnPause ) );
  m_Panel->m_PlayButton->Disconnect( m_Panel->m_PlayButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicEventsTimelinePanel::OnPlay ) );

  m_Panel->m_GotoFrameTextBox->Disconnect( m_Panel->m_GotoFrameTextBox->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CinematicEventsTimelinePanel::OnGotoFrame ) );
  m_Panel->m_MoveSelectedFrames->Disconnect( m_Panel->m_MoveSelectedFrames->GetId(), wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( CinematicEventsTimelinePanel::OnMoveSelectedFrames ) );
}

void CinematicEventsTimelinePanel::UpdateMarkers()
{
  m_Markers.clear();

  
  //if ( m_CurrentClip.ReferencesObject() )
  //{
    typedef std::map< int, std::vector< CinematicEventPtr > > M_FrameToEventVector;

  M_FrameToEventVector events;
  const CinematicEventListPtr& eventList = m_Manager.GetEventList();
  if ( eventList.ReferencesObject() )
  {
    for each ( const CinematicEventPtr& evt in eventList->m_Events )
    {
      events[ evt->m_Time ].push_back( evt );
    }

    OS_CinematicEvent& selection = m_Manager.GetSelection();

    for each ( M_FrameToEventVector::value_type val in events  )
    {
      if ( ( val.first < m_Manager.GetZoomStartFrame() ) || ( val.first > m_Manager.GetZoomEndFrame() ) )
      {
        continue;
      }

      // figure out the height of each rect, based on the number of events in the frame.
      wxRect sliderBounds = m_Panel->m_TimeSlider->GetRect();
      wxRect markerBounds = m_Panel->m_MarkersPanel->GetRect();

      int rectHeight = markerBounds.GetHeight() / (int)val.second.size();

      int i = 0;
      for each ( const CinematicEventPtr& evt in val.second )
      {
        int x = m_Panel->m_TimeSlider->ThumbPosToPixel( evt->m_Time - m_Manager.GetZoomStartFrame() );

        // convert from the time slider's pixel value to the marker panel's pixel value
        x += sliderBounds.GetLeft();
        x -= markerBounds.GetLeft();

        Marker marker;
        marker.m_Event = evt;
        marker.m_Color = m_Manager.GetEventColor( evt->m_ActorName + evt->m_JointName );
        marker.m_Rect = wxRect( x - 1, i*rectHeight, 3, rectHeight );

        if ( selection.Contains( marker.m_Event ) )
        {
          marker.m_Selected = true;
        }

        m_Markers.push_back( marker );

        ++i;
      }
    }
  }
  //}
}

void CinematicEventsTimelinePanel::SelectionChanged( const EventSelectionChangedArgs& args )
{
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::EventExistence( const EventExistenceArgs& args )
{
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::ClipChanged( const CinematicChangeArgs& args )
{
  if ( args.m_Clip.Ptr() == m_CurrentClip.Ptr() )
  {
    return;
  }

  m_CurrentClip = args.m_Clip;

  m_Panel->m_TimeSlider->SetRange( m_Manager.GetStartFrame(), m_Manager.GetEndFrame() );
  char frame[10];
  sprintf( frame, "%d", m_Manager.GetStartFrame() );
  m_Panel->m_StartFrame->SetLabel( frame );
  sprintf( frame, "%d", m_Manager.GetEndFrame() );
  m_Panel->m_EndFrame->SetLabel( frame );

  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::CinematicLoaded( const CinematicLoadedArgs& args )
{
  m_Panel->m_TimeSlider->SetRange( m_Manager.GetStartFrame(), m_Manager.GetEndFrame() );
  char frame[10];
  sprintf( frame, "%d", m_Manager.GetStartFrame() );
  m_Panel->m_StartFrame->SetLabel( frame );
  sprintf( frame, "%d", m_Manager.GetEndFrame() );
  m_Panel->m_EndFrame->SetLabel( frame );
}

void CinematicEventsTimelinePanel::EventsChanged( const EventsChangedArgs& args )
{
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::FrameChanged( const FrameChangedArgs& args )
{
  if ( args.m_Frame == m_Panel->m_TimeSlider->GetValue() )
  {
    return;
  }

  m_Panel->m_TimeSlider->SetValue( args.m_Frame );
}

void CinematicEventsTimelinePanel::EnableAll()
{
  m_Panel->m_TimeSlider->Enable();
  m_Panel->m_MarkersPanel->Enable();
  m_Panel->m_PauseButton->Enable();
  m_Panel->m_PlayButton->Enable();
  m_Panel->m_GotoFrameTextBox->Enable();
  m_Panel->m_MoveSelectedFrames->Enable();
  m_Panel->m_ZoomStart->Enable();
  m_Panel->m_ZoomEnd->Enable();
}

void CinematicEventsTimelinePanel::DisableAll()
{
  m_Panel->m_TimeSlider->SetRange( 0, 100 );
  m_Panel->m_TimeSlider->Disable();
  m_Panel->m_MarkersPanel->Disable();
  m_Panel->m_PauseButton->Disable();
  m_Panel->m_PlayButton->Disable();
  m_Panel->m_GotoFrameTextBox->Disable();
  m_Panel->m_MoveSelectedFrames->Disable();
  m_Panel->m_ZoomStart->Disable();
  m_Panel->m_ZoomEnd->Disable();
}

void CinematicEventsTimelinePanel::AssetOpened( const CinematicFileChangeArgs& args )
{
  m_Panel->m_TimeSlider->SetRange( m_Manager.GetStartFrame(), m_Manager.GetEndFrame() );
  char frame[10];
  sprintf( frame, "%d", m_Manager.GetStartFrame() );
  m_Panel->m_StartFrame->SetLabel( frame );
  sprintf( frame, "%d", m_Manager.GetEndFrame() );
  m_Panel->m_EndFrame->SetLabel( frame );
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
  EnableAll();
}

void CinematicEventsTimelinePanel::AssetClosed( const CinematicFileChangeArgs& args )
{
  DisableAll();

  m_Markers.clear();

  m_CurrentClip = NULL;

  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::OnSliderValueChanged( wxScrollEvent& e )
{
  m_Manager.SetPlayRate( 0.0 );
  m_Manager.SetFrame( e.GetInt() );
}

const CinematicEventPtr CinematicEventsTimelinePanel::FindEvent( int x, int y )
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

void CinematicEventsTimelinePanel::OnMarkerMouseDown( wxMouseEvent& e )
{
  e.Skip();

  m_MultiSelect = wxIsCtrlDown() || wxIsShiftDown();

  m_LastSelected = FindEvent( e.GetX(), e.GetY() );

  m_LastMouseTicLocation = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );

  m_PreMoveMouseTicLocation = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );

  m_UndoCommand = new CinematicEventsChangedCommand( m_Manager );

  m_MarkerMouseDown = true;
}

void CinematicEventsTimelinePanel::OnZoom( wxMouseEvent& e )
{
  e.Skip();

  int frames_to_zoom = 10;
  int current_mouse_loc = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );
  int diff_to_end = m_Manager.GetZoomEndFrame() - current_mouse_loc;
  int diff_to_start = current_mouse_loc - m_Manager.GetZoomStartFrame();
  int diff_between = abs( diff_to_start - diff_to_end );
  float diff_ratio = ( (float)diff_to_end / (float)diff_to_start > 1 ) ? (float)diff_to_start/(float)diff_to_end : (float)diff_to_end / (float)diff_to_start;

  //zoom in
  if ( e.m_wheelRotation > 0 )
  {
    if ( abs( m_Manager.GetZoomEndFrame() - m_Manager.GetZoomStartFrame() ) < 5 )
      return;
    if ( diff_to_end >= diff_to_start )
    {
      m_Manager.SetZoomEndFrame( m_Manager.GetZoomEndFrame() - frames_to_zoom );    
      m_Manager.SetZoomStartFrame( m_Manager.GetZoomStartFrame() + (int)( frames_to_zoom * diff_ratio) ); 
    }
    else
    {
      m_Manager.SetZoomStartFrame( m_Manager.GetZoomStartFrame() + frames_to_zoom );
      m_Manager.SetZoomEndFrame( m_Manager.GetZoomEndFrame() - (int)( frames_to_zoom * diff_ratio) );    
    }
    
  }
  //zoom out
  else
  {
    if ( m_Manager.GetZoomStartFrame() == m_Manager.GetStartFrame() && m_Manager.GetZoomEndFrame() == m_Manager.GetEndFrame() )
      return;

    if ( diff_to_end >= diff_to_start )
    {
      m_Manager.SetZoomEndFrame( m_Manager.GetZoomEndFrame() + frames_to_zoom );    
      m_Manager.SetZoomStartFrame( m_Manager.GetZoomStartFrame() - (int)( frames_to_zoom * diff_ratio) ); 
    }
    else
    {
      m_Manager.SetZoomStartFrame( m_Manager.GetZoomStartFrame() - frames_to_zoom );
      m_Manager.SetZoomEndFrame( m_Manager.GetZoomEndFrame() + (int)( frames_to_zoom * diff_ratio) );    
    }
  }

  if ( m_Manager.GetZoomStartFrame() < m_Manager.GetStartFrame() )
  {
    m_Manager.SetZoomStartFrame( m_Manager.GetStartFrame() );
  }
  if ( m_Manager.GetZoomEndFrame() > m_Manager.GetEndFrame() )
  {
    m_Manager.SetZoomEndFrame( m_Manager.GetEndFrame() );
  }

  m_Panel->m_TimeSlider->SetRange( m_Manager.GetZoomStartFrame(), m_Manager.GetZoomEndFrame() );
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::OnMarkerMouseUp( wxMouseEvent& e )
{
  e.Skip();
  m_Manager.SetUpdateProperties( true );

  if ( m_MarkerMouseDown )
  {
    if ( m_MovedEvents )
    {

      const OS_CinematicEvent& selection = m_Manager.GetSelection();
      int newTicLoc = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );

      //if nothing is selected when they drag over the timeline, we select everything under the drag
      if ( !m_LastSelected.ReferencesObject() )
      {
          OS_CinematicEvent events;
          const CinematicEventListPtr& eventList = m_Manager.GetEventList();
          for each ( const CinematicEventPtr& evt in eventList->m_Events )
          {
            if ( ( ( evt->m_Time >= m_PreMoveMouseTicLocation ) && ( evt->m_Time <= newTicLoc ) ) ||
              ( ( evt->m_Time <= m_PreMoveMouseTicLocation ) && ( evt->m_Time >= newTicLoc ) ) )
            {
              events.Append( evt );
              if ( m_MultiSelect )
              {
                m_Manager.AddSelection( evt );
              }
            }
          }
          
          if ( !m_MultiSelect )
          {
            m_Manager.SetSelection( events );
          }

      }
      //otherwise we move the selected events
      else
      {
        m_Manager.EventsChanged( selection );
      }

      m_Manager.GetUndoQueue().Push( m_UndoCommand );
    }
    else
    {
      if ( m_LastSelected.ReferencesObject() )
      {
        if ( m_MultiSelect )
        {
          OS_CinematicEvent& selection = m_Manager.GetSelection();

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
          OS_CinematicEvent events;
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

void CinematicEventsTimelinePanel::OnMarkerMouseMove( wxMouseEvent& e )
{

  e.Skip();

  m_Manager.SetUpdateProperties( true );

  if ( m_MarkerMouseDown )
  {
    m_Manager.SetUpdateProperties( false );

    m_MovedEvents = true;

    if ( !m_LastSelected.ReferencesObject() )
    {
      return;
    }

    const OS_CinematicEvent& selection = m_Manager.GetSelection();
    
    int newTicLoc = m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() );

    if ( newTicLoc != m_LastMouseTicLocation )
    {
      int diff = newTicLoc - m_LastMouseTicLocation;
      int numFrames = m_Manager.GetEndFrame();//m_Manager.GetNumFrames( m_CurrentClip );

      OS_CinematicEvent::Iterator eventIt = selection.Begin();
      OS_CinematicEvent::Iterator eventEnd = selection.End();
      for ( ; eventIt != eventEnd; ++eventIt )
      {
        const CinematicEventPtr& evt = *eventIt;
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

void CinematicEventsTimelinePanel::OnMarkersPaint( wxPaintEvent& e )
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

void CinematicEventsTimelinePanel::DrawEventMarkers( wxDC& dc )
{
  // draw selection rectangles
  for each ( const Marker& marker in m_Markers )
  {
    if ( marker.m_Selected )
    {
      wxBrush selectBrush( *wxWHITE );
      dc.SetBrush( selectBrush );
      dc.SetPen(*wxTRANSPARENT_PEN); //this loses the black border
      dc.DrawRectangle( marker.m_Rect.GetX() - 1, marker.m_Rect.GetY() - 3, marker.m_Rect.GetWidth() + 2, marker.m_Rect.GetHeight() + 6 );
    }
  }

  // now draw the main rectangles
  for each ( const Marker& marker in m_Markers )
  {
    wxBrush mainBrush( marker.m_Color );
    wxBrush gameplayBrush( marker.m_Color );
    gameplayBrush.SetStyle( wxCROSS_HATCH );
    dc.SetBrush( mainBrush );
    dc.SetPen(*wxTRANSPARENT_PEN); //this loses the black border

    if ( marker.m_Event->m_Untrigger )
    {
      dc.DrawRoundedRectangle( marker.m_Rect, 20.0 );
    }
    else
    {
      //if ( marker.m_Event->m_GameplayEvent )
      //{
      //  dc.SetBrush( gameplayBrush ); 
      //}

      dc.DrawRectangle( marker.m_Rect );
    }
  }
}

void CinematicEventsTimelinePanel::OnSliderMouseDown( wxMouseEvent& e )
{
  e.Skip();

  if ( !m_SliderMouseDown )
  {
    m_SliderMouseDown = true;
    m_LastSliderValue = m_Panel->m_TimeSlider->GetValue();
  }
}

void CinematicEventsTimelinePanel::OnSliderMouseUp( wxMouseEvent& e )
{
  e.Skip();

  if ( m_SliderMouseDown )
  {
    m_SliderMouseDown = false;

    if ( m_LastSliderValue != m_Panel->m_TimeSlider->PixelToThumbPos( e.GetX() ) )
    {
      CinematicEventsChangedCommand* undoCommand = new CinematicEventsChangedCommand( m_Manager );
      undoCommand->m_Frame = m_LastSliderValue;
      m_Manager.GetUndoQueue().Push( undoCommand );
    }
  }
}

void CinematicEventsTimelinePanel::OnChar( wxKeyEvent& e )
{
  m_Manager.GetEditor()->ProcessEvent( e );
}

void CinematicEventsTimelinePanel::OnSize( wxSizeEvent& event )
{
  event.Skip();

  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::OnPause( wxCommandEvent& e )
{
  m_Manager.SetPlayRate( 0.0 );
}

void CinematicEventsTimelinePanel::OnPlay( wxCommandEvent& e )
{
  m_Manager.SetPlayRate( 1.0 );
}

void CinematicEventsTimelinePanel::OnGotoFrame( wxCommandEvent& e )
{
  m_Manager.SetPlayRate( 0.0 );
  m_Manager.SetFrame( atoi( m_Panel->m_GotoFrameTextBox->GetValue().c_str() ) );
}

void CinematicEventsTimelinePanel::OnMoveSelectedFrames( wxCommandEvent& e )
{
  const OS_CinematicEvent& selection = m_Manager.GetSelection();
 
  OS_CinematicEvent::Iterator eventIt = selection.Begin();
  OS_CinematicEvent::Iterator eventEnd = selection.End();
  for ( ; eventIt != eventEnd; ++eventIt )
  {
    const CinematicEventPtr& evt = *eventIt;
    evt->m_Time += atoi( m_Panel->m_MoveSelectedFrames->GetValue().c_str() );
  }
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
  m_Manager.EventsChanged( selection );
}

void CinematicEventsTimelinePanel::OnSetZoomStart( wxCommandEvent& e )
{
  m_Manager.SetZoomStartFrame( atoi( m_Panel->m_ZoomStart->GetValue().c_str() ) );
  m_Panel->m_TimeSlider->SetRange( m_Manager.GetZoomStartFrame(), m_Manager.GetZoomEndFrame() );
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}

void CinematicEventsTimelinePanel::OnSetZoomEnd( wxCommandEvent& e )
{
  m_Manager.SetZoomEndFrame( atoi( m_Panel->m_ZoomEnd->GetValue().c_str() ) );
  m_Panel->m_TimeSlider->SetRange( m_Manager.GetZoomStartFrame(), m_Manager.GetZoomEndFrame() );
  UpdateMarkers();
  m_Panel->m_MarkersPanel->Refresh();
}
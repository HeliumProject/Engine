#include "Precompile.h"

#include "CinematicActiveEventsPanel.h"
#include "CinematicEventsPanels.h"
#include "CinematicEventsUndoCommands.h"
#include "CinematicEventsEditor.h"
#include "CinematicEventsEditorIDs.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"

#include <algorithm>

using namespace Luna;
using namespace Asset;

#define EVENT_COLUMN 0
#define VALUE_COLUMN 1

BEGIN_EVENT_TABLE( CinematicActiveEventsPanel, wxEvtHandler )
EVT_SIZE( OnSize )

END_EVENT_TABLE()

CinematicActiveEventsPanel::CinematicActiveEventsPanel( CinematicEventsManager& manager, CinematicActivePanel* panel )
: CinematicPanelBase( manager, panel )
, m_CurrentFrame( 0 )
, m_Freeze( false )
{
  m_Panel->m_EventGrid->SetRowLabelSize( 0 );

  // Don't let the horizontal scrollbar appear (there's a gutter on the right that
  // makes the scrollbar appear even when not needed).
  m_Panel->m_EventGrid->SetScrollLineX( 1 ); 

  // Prevent resizing rows and columns (headers and body of grid)
  m_Panel->m_EventGrid->DisableDragColSize();
  m_Panel->m_EventGrid->DisableDragGridSize();
  m_Panel->m_EventGrid->DisableDragRowSize();

  m_Panel->m_EventGrid->SetColLabelValue( EVENT_COLUMN, "Actor" );
  m_Panel->m_EventGrid->SetColLabelValue( VALUE_COLUMN, "Joint" );

  m_Panel->m_EventGrid->SetSelectionMode( wxGrid::wxGridSelectRows );

  m_Manager.AddCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicActiveEventsPanel::ClipChanged ) );
  m_Manager.AddFrameChangedListener( FrameChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::FrameChanged ) );
  m_Manager.AddEventExistenceListener( EventExistenceSignature::Delegate ( this, &CinematicActiveEventsPanel::EventExistence ) );
  m_Manager.AddSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::SelectionChanged ) );
  m_Manager.AddEventsChangedListener( EventsChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::EventsChanged ) );
  m_Manager.AddCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicActiveEventsPanel::CinematicLoaded ) );
  m_Manager.AddEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::EventTypesChanged ) );
  m_Manager.AddCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicActiveEventsPanel::AssetOpened ) );
  m_Manager.AddCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicActiveEventsPanel::AssetClosed ) );

  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_SELECT_CELL, wxCommandEventHandler( CinematicActiveEventsPanel::OnSelect ) );
  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler( CinematicActiveEventsPanel::OnRangeSelect ) );
  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( CinematicActiveEventsPanel::OnCellChange ) );

  // char handlers
  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler( CinematicActiveEventsPanel::OnChar ) );

  Connect( CinematicEventsEditorIDs::COMMAND_UPDATE_EVENT_LIST, wxCommandEventHandler( CinematicActiveEventsPanel::OnUpdateEventList ) );

  ResizeColumns();

  DisableAll();
}

CinematicActiveEventsPanel::~CinematicActiveEventsPanel()
{
  m_Manager.RemoveCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicActiveEventsPanel::ClipChanged ) );
  m_Manager.RemoveFrameChangedListener( FrameChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::FrameChanged ) );
  m_Manager.RemoveEventExistenceListener( EventExistenceSignature::Delegate ( this, &CinematicActiveEventsPanel::EventExistence ) );
  m_Manager.RemoveSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::SelectionChanged ) );
  m_Manager.RemoveEventsChangedListener( EventsChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::EventsChanged ) );
  m_Manager.RemoveCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicActiveEventsPanel::CinematicLoaded ) );
  m_Manager.RemoveEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &CinematicActiveEventsPanel::EventTypesChanged ) );
  m_Manager.RemoveCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicActiveEventsPanel::AssetOpened ) );
  m_Manager.RemoveCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicActiveEventsPanel::AssetClosed ) );


  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_SELECT_CELL, wxCommandEventHandler( CinematicActiveEventsPanel::OnSelect ) );
  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler( CinematicActiveEventsPanel::OnRangeSelect ) );
  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( CinematicActiveEventsPanel::OnCellChange ) );
  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler( CinematicActiveEventsPanel::OnChar ) );
  Disconnect( CinematicEventsEditorIDs::COMMAND_UPDATE_EVENT_LIST, wxCommandEventHandler( CinematicActiveEventsPanel::OnUpdateEventList ) );
}

void CinematicActiveEventsPanel::EnableAll()
{
  m_Panel->m_EventGrid->Enable();
}

void CinematicActiveEventsPanel::DisableAll()
{
  m_Panel->m_EventGrid->ClearGrid();
  m_Panel->m_EventGrid->Disable();
}

void CinematicActiveEventsPanel::AssetOpened( const CinematicFileChangeArgs& args )
{
  EnableAll();
}

void CinematicActiveEventsPanel::AssetClosed( const CinematicFileChangeArgs& args )
{
  DisableAll();
}

void CinematicActiveEventsPanel::EventExistence( const EventExistenceArgs& args )
{
  UpdateEventList();
}

void CinematicActiveEventsPanel::ClipChanged( const CinematicChangeArgs& args )
{
  if ( args.m_Clip.Ptr() == m_CurrentClip.Ptr() )
  {
    return;
  }

  m_CurrentClip = args.m_Clip;
}

void CinematicActiveEventsPanel::EventsChanged( const EventsChangedArgs& args )
{
  UpdateEventList();
}

void CinematicActiveEventsPanel::UpdateEventList()
{
  // the actual functionality behind this needs to happen on the *next* time through the event loop,
  // since this can get called as a result of a wxGrid callback
  // in the future it would be nice if this were not necessary, possibly by not needing to create new cell editors
  // each time through

  wxCommandEvent event( CinematicEventsEditorIDs::COMMAND_UPDATE_EVENT_LIST );
  wxPostEvent( this, event );
}

void CinematicActiveEventsPanel::OnUpdateEventList( wxCommandEvent& e )
{
  bool oldFreeze = m_Freeze;
  m_Freeze = true;

  m_Panel->m_EventGrid->BeginBatch();

  m_Panel->m_EventGrid->ClearGrid();
  if ( m_Panel->m_EventGrid->GetNumberRows() )
    m_Panel->m_EventGrid->DeleteRows( 0, m_Panel->m_EventGrid->GetNumberRows() );

  m_CurrentEvents.clear();


  //if ( m_CurrentClip.ReferencesObject() )
  //{
    int row = 0;
    const CinematicEventListPtr& eventList = m_Manager.GetEventList();
    for each ( const CinematicEventPtr& evt in eventList->m_Events )
    {
      if ( evt->m_Time == m_CurrentFrame )
      {
        m_Panel->m_EventGrid->AppendRows();

        m_Panel->m_EventGrid->SetCellBackgroundColour( row, EVENT_COLUMN, m_Manager.GetEventColor( evt->m_ActorName + evt->m_JointName ) );
        wxGridCellChoiceEditor* eventEditor = new wxGridCellChoiceEditor( 0, NULL, false );
        m_Panel->m_EventGrid->SetCellEditor( row, EVENT_COLUMN, eventEditor );

        // build up the list of events
        std::stringstream actors;
        actors << "None - Play 2D";
        for each ( std::string actorName in m_Manager.GetActors() )
        {
          if ( !actors.str().empty() )
          {
            actors << ",";
          }
          actors << actorName;
        }  
        eventEditor->SetParameters( actors.str() );

        m_Panel->m_EventGrid->SetCellValue( row, EVENT_COLUMN, evt->m_ActorName );

        wxGridCellChoiceEditor* valueEditor = new wxGridCellChoiceEditor( 0, NULL, true );
        m_Panel->m_EventGrid->SetCellEditor( row, VALUE_COLUMN, valueEditor );

        // build up the list of values
        std::stringstream joints;
        M_ActorToJoint actorJoints = m_Manager.GetActorJoints();
        M_ActorToJoint::iterator iter;
        std::pair< M_ActorToJoint::iterator, M_ActorToJoint::iterator > ret;
        ret = actorJoints.equal_range( evt->m_ActorName );
        for ( iter = ret.first; iter != ret.second; ++iter )
        {
          if ( !joints.str().empty() )
          {
            joints << ",";
          }
          joints << (*iter).second;
        } 
        valueEditor->SetParameters( joints.str() );

        m_Panel->m_EventGrid->SetCellValue( row, VALUE_COLUMN, evt->m_JointName );

        m_CurrentEvents.push_back( evt );

        ++row;
      }
    }
  //}

  m_Panel->m_EventGrid->EndBatch();

  m_Freeze = oldFreeze;

  // semi-hack
  SelectionChanged( EventSelectionChangedArgs( m_Manager.GetSelection() ) );
}

void CinematicActiveEventsPanel::FrameChanged( const FrameChangedArgs& args )
{
  m_CurrentFrame = args.m_Frame;

  UpdateEventList();
}

void CinematicActiveEventsPanel::SelectionChanged( const EventSelectionChangedArgs& args )
{
  // this selection event came from ourselves selecting something, which would cause an infinite loop
  if ( m_Freeze )
    return;

  m_Freeze = true;

  m_Panel->m_EventGrid->ClearSelection();

  for ( u32 i = 0; i < m_CurrentEvents.size(); ++i )
  {
    const CinematicEventPtr& evt = m_CurrentEvents[ i ];

    if ( args.m_Selection.Contains( evt ) )
    {
      m_Panel->m_EventGrid->SelectRow( i, true );
    }
  }

  m_Freeze = false;
}

void CinematicActiveEventsPanel::CinematicLoaded( const CinematicLoadedArgs& args )
{
  UpdateEventList();
}

void CinematicActiveEventsPanel::EventTypesChanged( const EventTypesChangedArgs& args )
{
  UpdateEventList();
}

void CinematicActiveEventsPanel::ResizeColumns()
{
  int width = m_Panel->GetSize().x;

  m_Panel->m_EventGrid->SetColSize( EVENT_COLUMN, width/2 );
  m_Panel->m_EventGrid->SetColSize( VALUE_COLUMN, width/2 );
}

void CinematicActiveEventsPanel::OnSize( wxSizeEvent& event )
{
  event.Skip();

  ResizeColumns();
  m_Panel->m_EventGrid->ForceRefresh();
}

void CinematicActiveEventsPanel::UpdateSelection()
{
  if ( m_Freeze )
    return;

  m_Freeze = true;

  wxGridCellCoordsArray selectionBlockTopLeft = m_Panel->m_EventGrid->GetSelectionBlockTopLeft();
  wxGridCellCoordsArray selectionBlockBottomRight = m_Panel->m_EventGrid->GetSelectionBlockBottomRight();

  OS_CinematicEvent selectedEvents;

  for ( u32 i = 0; i < selectionBlockTopLeft.size(); ++i )
  {
    for ( int selectedRow = selectionBlockTopLeft[ i ].GetRow(); selectedRow <= selectionBlockBottomRight[ i ].GetRow(); ++selectedRow )
    {
      selectedEvents.Append( m_CurrentEvents[ selectedRow ] );
    }
  }

  m_Manager.SetSelection( selectedEvents );

  m_Freeze = false;
}

void CinematicActiveEventsPanel::OnSelect( wxCommandEvent& event )
{
  event.Skip();

  UpdateSelection();
}

void CinematicActiveEventsPanel::OnRangeSelect( wxGridRangeSelectEvent& event )
{
  event.Skip();

  UpdateSelection();
}

void CinematicActiveEventsPanel::OnCellChange( wxGridEvent& event )
{
  int row = event.GetRow();
  int col = event.GetCol();

  CinematicEventPtr& cinematicEvent = m_CurrentEvents[ row ];
  std::string value = m_Panel->m_EventGrid->GetCellValue( row, col ).c_str();

  bool changed = false;

  OS_CinematicEvent events;
  events.Append( cinematicEvent );
  CinematicEventsChangedCommand* undoCommand = new CinematicEventsChangedCommand( m_Manager );

  if ( col == EVENT_COLUMN )
  {
    if ( cinematicEvent->m_ActorName != value )
    {
      cinematicEvent->m_ActorName= value;

      changed = true;
    }
  }
  else if ( col == VALUE_COLUMN )
  {
    if ( cinematicEvent->m_JointName != value )
    {
      cinematicEvent->m_JointName = value;

      changed = true;
    }
  }

  if ( changed )
  {
    m_Manager.GetUndoQueue().Push( undoCommand );
    m_Manager.EventChanged( cinematicEvent );
  }
  else
  {
    delete undoCommand;
  }
}

void CinematicActiveEventsPanel::OnChar( wxKeyEvent& e )
{
  e.Skip();

  m_Manager.GetEditor()->ProcessEvent( e );
}
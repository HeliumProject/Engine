#include "Precompile.h"

#include "AnimationActiveEventsPanel.h"
#include "AnimationEventsPanels.h"
#include "AnimationEventsUndoCommands.h"
#include "AnimationEventsEditor.h"
#include "AnimationEventsEditorIDs.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"

#include <algorithm>

using namespace Luna;
using namespace Asset;

#define EVENT_COLUMN 0
#define VALUE_COLUMN 1

BEGIN_EVENT_TABLE( AnimationActiveEventsPanel, wxEvtHandler )
EVT_SIZE( OnSize )

END_EVENT_TABLE()

AnimationActiveEventsPanel::AnimationActiveEventsPanel( AnimationEventsManager& manager, AnimationActivePanel* panel )
: AnimationPanelBase( manager, panel )
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

  m_Panel->m_EventGrid->SetColLabelValue( EVENT_COLUMN, "Event" );
  m_Panel->m_EventGrid->SetColLabelValue( VALUE_COLUMN, "Value" );

  m_Panel->m_EventGrid->SetSelectionMode( wxGrid::wxGridSelectRows );

  m_Manager.AddAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationActiveEventsPanel::ClipChanged ) );
  m_Manager.AddFrameChangedListener( FrameChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::FrameChanged ) );
  m_Manager.AddEventExistenceListener( EventExistenceSignature::Delegate ( this, &AnimationActiveEventsPanel::EventExistence ) );
  m_Manager.AddSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::SelectionChanged ) );
  m_Manager.AddEventsChangedListener( EventsChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::EventsChanged ) );
  m_Manager.AddUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::UpdateClassChanged ) );
  m_Manager.AddEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::EventTypesChanged ) );
  m_Manager.AddMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationActiveEventsPanel::AssetOpened ) );
  m_Manager.AddMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationActiveEventsPanel::AssetClosed ) );

  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_SELECT_CELL, wxCommandEventHandler( AnimationActiveEventsPanel::OnSelect ) );
  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler( AnimationActiveEventsPanel::OnRangeSelect ) );
  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( AnimationActiveEventsPanel::OnCellChange ) );

  // char handlers
  Connect( m_Panel->m_EventGrid->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler( AnimationActiveEventsPanel::OnChar ) );

  Connect( AnimationEventsEditorIDs::COMMAND_UPDATE_EVENT_LIST, wxCommandEventHandler( AnimationActiveEventsPanel::OnUpdateEventList ) );

  ResizeColumns();

  DisableAll();
}

AnimationActiveEventsPanel::~AnimationActiveEventsPanel()
{
  m_Manager.RemoveAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationActiveEventsPanel::ClipChanged ) );
  m_Manager.RemoveFrameChangedListener( FrameChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::FrameChanged ) );
  m_Manager.RemoveEventExistenceListener( EventExistenceSignature::Delegate ( this, &AnimationActiveEventsPanel::EventExistence ) );
  m_Manager.RemoveSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::SelectionChanged ) );
  m_Manager.RemoveEventsChangedListener( EventsChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::EventsChanged ) );
  m_Manager.RemoveUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::UpdateClassChanged ) );
  m_Manager.RemoveEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &AnimationActiveEventsPanel::EventTypesChanged ) );
  m_Manager.RemoveMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationActiveEventsPanel::AssetOpened ) );
  m_Manager.RemoveMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationActiveEventsPanel::AssetClosed ) );


  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_SELECT_CELL, wxCommandEventHandler( AnimationActiveEventsPanel::OnSelect ) );
  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_RANGE_SELECT, wxGridRangeSelectEventHandler( AnimationActiveEventsPanel::OnRangeSelect ) );
  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_GRID_CELL_CHANGE, wxGridEventHandler( AnimationActiveEventsPanel::OnCellChange ) );
  Disconnect( m_Panel->m_EventGrid->GetId(), wxEVT_KEY_DOWN, wxKeyEventHandler( AnimationActiveEventsPanel::OnChar ) );
  Disconnect( AnimationEventsEditorIDs::COMMAND_UPDATE_EVENT_LIST, wxCommandEventHandler( AnimationActiveEventsPanel::OnUpdateEventList ) );
}

void AnimationActiveEventsPanel::EnableAll()
{
  m_Panel->m_EventGrid->Enable();
}

void AnimationActiveEventsPanel::DisableAll()
{
  m_Panel->m_EventGrid->ClearGrid();
  m_Panel->m_EventGrid->Disable();
}

void AnimationActiveEventsPanel::AssetOpened( const MobyFileChangeArgs& args )
{
  EnableAll();
}

void AnimationActiveEventsPanel::AssetClosed( const MobyFileChangeArgs& args )
{
  DisableAll();
}

void AnimationActiveEventsPanel::EventExistence( const EventExistenceArgs& args )
{
  UpdateEventList();
}

void AnimationActiveEventsPanel::ClipChanged( const AnimClipChangeArgs& args )
{
  if ( args.m_Clip.Ptr() == m_CurrentClip.Ptr() )
  {
    return;
  }

  m_CurrentClip = args.m_Clip;
}

void AnimationActiveEventsPanel::EventsChanged( const EventsChangedArgs& args )
{
  UpdateEventList();
}

void AnimationActiveEventsPanel::UpdateEventList()
{
  // the actual functionality behind this needs to happen on the *next* time through the event loop,
  // since this can get called as a result of a wxGrid callback
  // in the future it would be nice if this were not necessary, possibly by not needing to create new cell editors
  // each time through

  wxCommandEvent event( AnimationEventsEditorIDs::COMMAND_UPDATE_EVENT_LIST );
  wxPostEvent( this, event );
}

void AnimationActiveEventsPanel::OnUpdateEventList( wxCommandEvent& e )
{
  bool oldFreeze = m_Freeze;
  m_Freeze = true;

  m_Panel->m_EventGrid->BeginBatch();

  m_Panel->m_EventGrid->ClearGrid();
  if ( m_Panel->m_EventGrid->GetNumberRows() )
    m_Panel->m_EventGrid->DeleteRows( 0, m_Panel->m_EventGrid->GetNumberRows() );

  m_CurrentEvents.clear();

  if ( m_CurrentClip.ReferencesObject() )
  {
    int row = 0;
    const AnimationEventListPtr& eventList = m_Manager.GetEventList();
    for each ( const AnimationEventPtr& evt in eventList->m_Events )
    {
      if ( evt->m_Time == m_CurrentFrame )
      {
        m_Panel->m_EventGrid->AppendRows();

        m_Panel->m_EventGrid->SetCellBackgroundColour( row, EVENT_COLUMN, m_Manager.GetEventColor( evt->m_EventType ) );
        wxGridCellChoiceEditor* eventEditor = new wxGridCellChoiceEditor( 0, NULL, false );
        m_Panel->m_EventGrid->SetCellEditor( row, EVENT_COLUMN, eventEditor );

        // build up the list of events
        const V_string& eventTypes = m_Manager.GetEventTypes( evt );

        std::stringstream events;
        for each ( const std::string& eventType in eventTypes )
        {
          if ( eventType != "None" && eventType != "NONE" )
          {
            if ( !events.str().empty() )
            {
              events << ",";
            }

            events << eventType;
          }
        }

        eventEditor->SetParameters( events.str() );

        m_Panel->m_EventGrid->SetCellValue( row, EVENT_COLUMN, evt->m_EventType );

        wxGridCellChoiceEditor* valueEditor = new wxGridCellChoiceEditor( 0, NULL, true );
        m_Panel->m_EventGrid->SetCellEditor( row, VALUE_COLUMN, valueEditor );

        // build up the list of values
        std::stringstream values;
        const S_u32& appliedValues = m_Manager.GetAppliedValues( evt->m_EventType );
        for each ( const u32 value in appliedValues )
        {
          if ( value != -1 )
          {
            if ( !values.str().empty() )
            {
              values << ",";
            }

            values << value;
          }
        }

        valueEditor->SetParameters( values.str() );

        char buf[16];
        _snprintf( buf, sizeof(buf), "%d", (int)evt->m_Value );
        buf[ sizeof(buf) - 1 ] = 0; 

        m_Panel->m_EventGrid->SetCellValue( row, VALUE_COLUMN, buf );

        m_CurrentEvents.push_back( evt );

        ++row;
      }
    }
  }

  m_Panel->m_EventGrid->EndBatch();

  m_Freeze = oldFreeze;

  // semi-hack
  SelectionChanged( EventSelectionChangedArgs( m_Manager.GetSelection() ) );
}

void AnimationActiveEventsPanel::FrameChanged( const FrameChangedArgs& args )
{
  m_CurrentFrame = args.m_Frame;

  UpdateEventList();
}

void AnimationActiveEventsPanel::SelectionChanged( const EventSelectionChangedArgs& args )
{
  // this selection event came from ourselves selecting something, which would cause an infinite loop
  if ( m_Freeze )
    return;

  m_Freeze = true;

  m_Panel->m_EventGrid->ClearSelection();

  for ( u32 i = 0; i < m_CurrentEvents.size(); ++i )
  {
    const AnimationEventPtr& evt = m_CurrentEvents[ i ];

    if ( args.m_Selection.Contains( evt ) )
    {
      m_Panel->m_EventGrid->SelectRow( i, true );
    }
  }

  m_Freeze = false;
}

void AnimationActiveEventsPanel::UpdateClassChanged( const UpdateClassChangedArgs& args )
{
  UpdateEventList();
}

void AnimationActiveEventsPanel::EventTypesChanged( const EventTypesChangedArgs& args )
{
  UpdateEventList();
}

void AnimationActiveEventsPanel::ResizeColumns()
{
  int width = m_Panel->GetSize().x;

  m_Panel->m_EventGrid->SetColSize( EVENT_COLUMN, width/2 );
  m_Panel->m_EventGrid->SetColSize( VALUE_COLUMN, width/2 );
}

void AnimationActiveEventsPanel::OnSize( wxSizeEvent& event )
{
  event.Skip();

  ResizeColumns();
  m_Panel->m_EventGrid->ForceRefresh();
}

void AnimationActiveEventsPanel::UpdateSelection()
{
  if ( m_Freeze )
    return;

  m_Freeze = true;

  wxGridCellCoordsArray selectionBlockTopLeft = m_Panel->m_EventGrid->GetSelectionBlockTopLeft();
  wxGridCellCoordsArray selectionBlockBottomRight = m_Panel->m_EventGrid->GetSelectionBlockBottomRight();

  OS_AnimationEvent selectedEvents;

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

void AnimationActiveEventsPanel::OnSelect( wxCommandEvent& event )
{
  event.Skip();

  UpdateSelection();
}

void AnimationActiveEventsPanel::OnRangeSelect( wxGridRangeSelectEvent& event )
{
  event.Skip();

  UpdateSelection();
}

void AnimationActiveEventsPanel::OnCellChange( wxGridEvent& event )
{
  int row = event.GetRow();
  int col = event.GetCol();

  AnimationEventPtr& effectEvent = m_CurrentEvents[ row ];
  std::string value = m_Panel->m_EventGrid->GetCellValue( row, col ).c_str();

  bool changed = false;

  OS_AnimationEvent events;
  events.Append( effectEvent );
  AnimationEventsChangedCommand* undoCommand = new AnimationEventsChangedCommand( m_Manager );

  if ( col == EVENT_COLUMN )
  {
    if ( effectEvent->m_EventType != value )
    {
      effectEvent->m_EventType = value;

      changed = true;
    }
  }
  else if ( col == VALUE_COLUMN )
  {
    float floatVal = atof( value.c_str() );
    if ( floatVal != effectEvent->m_Value )
    {
      effectEvent->m_Value = floatVal;
      changed = true;
    }
  }

  if ( changed )
  {
    m_Manager.GetUndoQueue().Push( undoCommand );
    m_Manager.EventChanged( effectEvent );
  }
  else
  {
    delete undoCommand;
  }
}

void AnimationActiveEventsPanel::OnChar( wxKeyEvent& e )
{
  e.Skip();

  m_Manager.GetEditor()->ProcessEvent( e );
}
#include "Precompile.h"

#include "CinematicEventsUndoCommands.h"
#include "CinematicEventsManager.h"
#include "CinematicEventsEditor.h"

using namespace Luna;
using namespace Asset;

CinematicEventsChangedCommand::CinematicEventsChangedCommand( CinematicEventsManager& manager )
  : m_Manager( manager )
{
  UpdateValues();
}

void CinematicEventsChangedCommand::UpdateValues()
{
  m_Clip = m_Manager.GetCurrentClip();
  m_Frame = m_Manager.GetCurrentFrame();

  OS_CinematicEvent& selection = m_Manager.GetSelection();

  m_Events.Clear();
  m_Selection.Clear();
  const CinematicEventListPtr& eventList = m_Manager.GetFile()->GetCinematicEventList();
  for each ( const CinematicEventPtr& event in eventList->m_Events )
  {
    CinematicEventPtr newEvent = Reflect::ObjectCast< CinematicEvent >( event->Clone() );
    
    m_Events.Append( newEvent );

    if ( selection.Contains( event ) )
    {
      m_Selection.Append( newEvent );
    }
  }
}

void CinematicEventsChangedCommand::Swap()
{
  m_Manager.GetEditor()->Freeze();

  int oldFrame = m_Frame;
  CinematicAssetPtr oldClip = m_Clip;
  OS_CinematicEvent oldSelection = m_Selection;
  OS_CinematicEvent oldEvents = m_Events;

  UpdateValues();

  const CinematicEventListPtr& oldEventList = m_Manager.GetFile()->GetCinematicEventList();
  oldEventList->m_Events.clear();

  OS_CinematicEvent::Iterator eventIt = oldEvents.Begin();
  OS_CinematicEvent::Iterator eventEnd = oldEvents.End();
  for ( ; eventIt != eventEnd; ++eventIt )
  {
    const CinematicEventPtr& event = *eventIt;

    oldEventList->m_Events.push_back( event );
  }

  m_Manager.SetClip( oldClip, false );
  m_Manager.EventsChanged( oldEvents );
  m_Manager.SetFrame( oldFrame );
  m_Manager.SetSelection( oldSelection );

  m_Manager.GetEditor()->Thaw();
}

void CinematicEventsChangedCommand::Undo()
{
  Swap();
}

void CinematicEventsChangedCommand::Redo()
{
  Swap();
}
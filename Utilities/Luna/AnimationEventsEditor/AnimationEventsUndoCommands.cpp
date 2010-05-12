#include "Precompile.h"

#include "AnimationEventsUndoCommands.h"
#include "AnimationEventsManager.h"
#include "AnimationEventsEditor.h"

using namespace Luna;
using namespace Asset;

AnimationEventsChangedCommand::AnimationEventsChangedCommand( AnimationEventsManager& manager )
  : m_Manager( manager )
{
  UpdateValues();
}

void AnimationEventsChangedCommand::UpdateValues()
{
  m_Clip = m_Manager.GetCurrentClip();
  m_Frame = m_Manager.GetCurrentFrame();

  OS_AnimationEvent& selection = m_Manager.GetSelection();

  m_Events.Clear();
  m_Selection.Clear();
  const AnimationEventListPtr& eventList = m_Manager.GetFile()->GetClipEventList( m_Clip );
  for each ( const AnimationEventPtr& event in eventList->m_Events )
  {
    AnimationEventPtr newEvent = Reflect::ObjectCast< AnimationEvent >( event->Clone() );
    
    m_Events.Append( newEvent );

    if ( selection.Contains( event ) )
    {
      m_Selection.Append( newEvent );
    }
  }
}

void AnimationEventsChangedCommand::Swap()
{
  m_Manager.GetEditor()->Freeze();

  int oldFrame = m_Frame;
  AnimationClipDataPtr oldClip = m_Clip;
  OS_AnimationEvent oldSelection = m_Selection;
  OS_AnimationEvent oldEvents = m_Events;

  UpdateValues();

  const AnimationEventListPtr& oldEventList = m_Manager.GetFile()->GetClipEventList( oldClip );
  oldEventList->m_Events.clear();

  OS_AnimationEvent::Iterator eventIt = oldEvents.Begin();
  OS_AnimationEvent::Iterator eventEnd = oldEvents.End();
  for ( ; eventIt != eventEnd; ++eventIt )
  {
    const AnimationEventPtr& event = *eventIt;

    oldEventList->m_Events.push_back( event );
  }

  m_Manager.SetClip( oldClip, false );
  m_Manager.EventsChanged( oldEvents );
  m_Manager.SetFrame( oldFrame );
  m_Manager.SetSelection( oldSelection );

  m_Manager.GetEditor()->Thaw();
}

void AnimationEventsChangedCommand::Undo()
{
  Swap();
}

void AnimationEventsChangedCommand::Redo()
{
  Swap();
}
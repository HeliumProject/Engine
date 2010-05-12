#include "Precompile.h"

#include "AnimationPropertyPanel.h"
#include "AnimationEventsPanels.h"
#include "AnimationEventsUndoCommands.h"
#include "AnimationEventsEditor.h"
#include "AnimationEventsEditorIDs.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"

#include <algorithm>

using namespace Luna;
using namespace Asset;

BEGIN_EVENT_TABLE( AnimationEventsPropertyPanel, wxEvtHandler )
END_EVENT_TABLE()

AnimationEventsPropertyPanel::AnimationEventsPropertyPanel( AnimationEventsManager& manager, wxPanel* panel )
: AnimationPanelBase( manager, panel )
, m_Frozen( false )
{
  m_Manager.AddAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationEventsPropertyPanel::ClipChanged ) );
  m_Manager.AddFrameChangedListener( FrameChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::FrameChanged ) );
  m_Manager.AddEventExistenceListener( EventExistenceSignature::Delegate ( this, &AnimationEventsPropertyPanel::EventExistence ) );
  m_Manager.AddSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::SelectionChanged ) );
  m_Manager.AddEventsChangedListener( EventsChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::EventsChanged ) );
  m_Manager.AddUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::UpdateClassChanged ) );
  m_Manager.AddEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::EventTypesChanged ) );
  m_Manager.AddMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsPropertyPanel::AssetOpened ) );
  m_Manager.AddMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsPropertyPanel::AssetClosed ) );

  m_CanvasWindow = new Inspect::CanvasWindow( m_Panel, wxID_ANY, wxDefaultPosition, m_Panel->GetSize(), wxALWAYS_SHOW_SB | wxCLIP_CHILDREN );
  
  wxBoxSizer* sizer = new wxBoxSizer( wxVERTICAL );
  sizer->Add( m_CanvasWindow, 1, wxEXPAND | wxALL, 0 );

  m_Panel->SetSizer( sizer );
  m_Panel->Layout();
  sizer->Fit( m_Panel );

  m_Canvas.SetControl( m_CanvasWindow );

  DisableAll();
}

AnimationEventsPropertyPanel::~AnimationEventsPropertyPanel()
{
  m_Manager.RemoveAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationEventsPropertyPanel::ClipChanged ) );
  m_Manager.RemoveFrameChangedListener( FrameChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::FrameChanged ) );
  m_Manager.RemoveEventExistenceListener( EventExistenceSignature::Delegate ( this, &AnimationEventsPropertyPanel::EventExistence ) );
  m_Manager.RemoveSelectionChangedListener( EventSelectionChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::SelectionChanged ) );
  m_Manager.RemoveEventsChangedListener( EventsChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::EventsChanged ) );
  m_Manager.RemoveUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::UpdateClassChanged ) );
  m_Manager.RemoveEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::EventTypesChanged ) );
  m_Manager.RemoveMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsPropertyPanel::AssetOpened ) );
  m_Manager.RemoveMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsPropertyPanel::AssetClosed ) );
}

void AnimationEventsPropertyPanel::EnableAll()
{
}

void AnimationEventsPropertyPanel::DisableAll()
{
}

void AnimationEventsPropertyPanel::AssetOpened( const MobyFileChangeArgs& args )
{
  EnableAll();
}

void AnimationEventsPropertyPanel::AssetClosed( const MobyFileChangeArgs& args )
{
  DisableAll();
}

void AnimationEventsPropertyPanel::EventExistence( const EventExistenceArgs& args )
{
}

void AnimationEventsPropertyPanel::ClipChanged( const AnimClipChangeArgs& args )
{
}

void AnimationEventsPropertyPanel::EventsChanged( const EventsChangedArgs& args )
{
  if ( m_Frozen )
    return;

  UpdatePanel();
}

void AnimationEventsPropertyPanel::UpdatePanel()
{
  m_Canvas.Clear();

  OS_AnimationEvent& selection = m_Manager.GetSelection();

  if ( !selection.Empty() )
  {
    std::vector< Reflect::Element* > elems;

    OS_AnimationEvent::Iterator eventIt = selection.Begin();
    OS_AnimationEvent::Iterator eventEnd = selection.End();
    for ( ; eventIt != eventEnd; ++eventIt )
    {
      elems.push_back( *eventIt );
    }

    m_Interpreter = new Inspect::ReflectInterpreter( &m_Canvas );
    m_Interpreter->Interpret( elems );

    m_Interpreter->AddPropertyChangedListener( Inspect::ChangedSignature::Delegate ( this, &AnimationEventsPropertyPanel::PropertyChanged ) );

    m_Canvas.Freeze();
    m_Canvas.Layout();
    m_Canvas.Read();
    m_Canvas.Thaw();
  }
}

void AnimationEventsPropertyPanel::FrameChanged( const FrameChangedArgs& args )
{
}

void AnimationEventsPropertyPanel::SelectionChanged( const EventSelectionChangedArgs& args )
{
  UpdatePanel();
}

void AnimationEventsPropertyPanel::UpdateClassChanged( const UpdateClassChangedArgs& args )
{
}

void AnimationEventsPropertyPanel::EventTypesChanged( const EventTypesChangedArgs& args )
{
}

void AnimationEventsPropertyPanel::PropertyChanged( const Inspect::ChangeArgs& args )
{
  m_Frozen = true;

  AnimationEventsChangedCommand* undoCommand = new AnimationEventsChangedCommand( m_Manager );
  m_Manager.GetUndoQueue().Push( undoCommand );
  m_Manager.EventsChanged( m_Manager.GetSelection() );

  m_Frozen = false;
}
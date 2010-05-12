#include "Precompile.h"

#include "AnimationMainPanel.h"
#include "AnimationEventsPanels.h"
#include "AnimationEventsEditor.h"
#include "AnimationEventsEditorIDs.h"
#include "AnimationEventsEditor.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/UpdateClassAttribute.h"

#include "Console/Console.h"

using namespace Luna;
using namespace Asset;
using namespace Attribute;

BEGIN_EVENT_TABLE( AnimationEventsMainPanel, wxEvtHandler )
EVT_CHOICE( AnimationEventsEditorIDs::AnimClipList, OnClipChanged )

EVT_CHAR( OnChar )
END_EVENT_TABLE()

AnimationEventsMainPanel::AnimationEventsMainPanel( AnimationEventsManager& manager, AnimationPanel* panel )
: AnimationPanelBase( manager, panel )
{
  Clear();

  m_Manager.AddMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsMainPanel::AssetOpened ) );
  m_Manager.AddMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsMainPanel::AssetClosed ) );
  m_Manager.AddAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationEventsMainPanel::ClipChanged ) );
  m_Manager.AddUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( this, &AnimationEventsMainPanel::UpdateClassChanged ) );
  m_Manager.AddEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &AnimationEventsMainPanel::EventTypesChanged ) );


  m_Panel->m_ClipChoice->SetId( AnimationEventsEditorIDs::AnimClipList );

  Connect( m_Panel->m_AddEventButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnAddEvent ) );
  Connect( m_Panel->m_DeleteSelection->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnDeleteSelection ) );
  Connect( m_Panel->m_RebuildMoby->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnRebuildMoby ) );
  Connect( m_Panel->m_ShowOnlyMatchingEvents->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnFilterEvents ) );
  Connect( m_Panel->m_UpdateClassChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AnimationEventsMainPanel::OnUpdateClass ) );
  Connect( m_Panel->m_AuditionButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnAudition ) );
  Connect( m_Panel->m_StopAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnStopAll ) );
  Connect( m_Panel->m_EventTypeChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AnimationEventsMainPanel::OnEventType ) );
  Connect( m_Panel->m_RefreshEvents->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnRefreshEvents ) );
  Connect( m_Panel->m_GameplayEvent->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnGameplayEvent ) );
  Connect( m_Panel->m_OverrideEventsCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnOverrideEvents ) );

  // disable everything when we first start up
  DisableAll();
  
  Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( &FinderSpecs::Asset::ENTITY_DECORATION );
  filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &AnimationEventsMainPanel::OnEntityDropped ) );
  m_Panel->SetDropTarget( filteredDropTarget );
}

AnimationEventsMainPanel::~AnimationEventsMainPanel()
{
  m_Manager.RemoveMobyOpenedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsMainPanel::AssetOpened ) );
  m_Manager.RemoveMobyClosedListener( MobyFileChangeSignature::Delegate ( this, &AnimationEventsMainPanel::AssetClosed ) );
  m_Manager.RemoveAnimClipChangeListener( AnimClipChangeSignature::Delegate ( this, &AnimationEventsMainPanel::ClipChanged ) );
  m_Manager.RemoveUpdateClassChangedListener( UpdateClassChangedSignature::Delegate ( this, &AnimationEventsMainPanel::UpdateClassChanged ) );
  m_Manager.RemoveEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &AnimationEventsMainPanel::EventTypesChanged ) );

  Disconnect( m_Panel->m_AddEventButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnAddEvent ) );
  Disconnect( m_Panel->m_DeleteSelection->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnDeleteSelection ) );
  Disconnect( m_Panel->m_RebuildMoby->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnRebuildMoby ) );
  Disconnect( m_Panel->m_ShowOnlyMatchingEvents->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnFilterEvents ) );
  Disconnect( m_Panel->m_UpdateClassChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AnimationEventsMainPanel::OnUpdateClass ) );
  Disconnect( m_Panel->m_AuditionButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnAudition ) );
  Disconnect( m_Panel->m_StopAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnStopAll ) );
  Disconnect( m_Panel->m_EventTypeChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( AnimationEventsMainPanel::OnEventType ) );
  Disconnect( m_Panel->m_RefreshEvents->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnRefreshEvents ) );
  Disconnect( m_Panel->m_GameplayEvent->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnGameplayEvent ) );
  Disconnect( m_Panel->m_OverrideEventsCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( AnimationEventsMainPanel::OnOverrideEvents ) );
}

void AnimationEventsMainPanel::Clear()
{
  m_Panel->m_UpdateClassChoice->Clear();
  m_Panel->m_ClipChoice->Clear();
  m_Panel->m_EventTypeChoice->Clear();
  m_Panel->m_ValueCombo->Clear();
  m_Panel->m_Untrigger->SetValue( false );
  m_Panel->m_ShowOnlyMatchingEvents->SetValue( false );
}

void AnimationEventsMainPanel::Populate( const AnimationEventsDocumentPtr& file )
{
  Clear();

  m_Panel->Freeze();

  // Update Classes
  AttributeViewer<UpdateClassAttribute> updateClassAttr( file->m_MobyClass );

  if ( updateClassAttr.Valid() )
  {
    const V_string& updateClasses = updateClassAttr->GetClassNames(); 
    
    V_string::const_iterator it  = updateClasses.begin();
    V_string::const_iterator end = updateClasses.end();

    for ( ; it != end; ++it )
    {
      m_Panel->m_UpdateClassChoice->Append( *it );
    }
  }

  if ( m_Panel->m_UpdateClassChoice->GetCount() > 0)
  {
    m_Panel->m_UpdateClassChoice->SetSelection( 0 );
    m_Manager.SetUpdateClass( 0 );
  }

  // Clips
  int i = 0;
  for each ( const AnimationClipDataPtr& clip in file->m_ClipData )
  {
    //add the clip to the clip combobox
    std::stringstream s;
    s << (i++) + 1 << ": " << clip->GetName();
    m_Panel->m_ClipChoice->Append( s.str(), clip.Ptr() );
  }

  m_Panel->m_ShowOnlyMatchingEvents->SetValue( m_Manager.GetEventTypeFilter() );

  // Events
  PopulateEventTypes();

  PopulateAppliedValues();

  m_Panel->Thaw();
}

void AnimationEventsMainPanel::PopulateAppliedValues()
{
  std::string currentValue = m_Panel->m_ValueCombo->GetValue();

  m_Panel->m_ValueCombo->Clear();
  std::string eventType = m_Panel->m_EventTypeChoice->GetStringSelection();

  for each ( const u32 value in m_Manager.GetAppliedValues( eventType ) )
  {
    if ( value != -1 )
    {
      std::stringstream stream;
      stream << value;
      m_Panel->m_ValueCombo->Append( stream.str() );
    }
  }

  m_Panel->m_ValueCombo->SetValue( currentValue );
}

void AnimationEventsMainPanel::PopulateEventTypes()
{
  m_Panel->Freeze();

  m_Panel->m_EventTypeChoice->Clear();

  const V_string& eventTypes = m_Manager.GetEventTypes( m_Panel->m_GameplayEvent->IsChecked() );

  for each ( const std::string& eventType in eventTypes )
  {
    if ( eventType != "None" && eventType != "NONE" )
    {
      m_Panel->m_EventTypeChoice->Append( eventType );
    }
  }

  if ( m_Panel->m_EventTypeChoice->GetCount() > 0 )
  {
    m_Panel->m_EventTypeChoice->SetSelection( 0 );
  }

  m_Panel->Thaw();
}

void AnimationEventsMainPanel::DisableAll()
{
  m_Panel->m_UpdateClassChoice->Disable();
  m_Panel->m_ClipChoice->Disable();
  m_Panel->m_EventTypeChoice->Disable();
  m_Panel->m_ValueCombo->Disable();
  m_Panel->m_Untrigger->Disable();
  m_Panel->m_DominantOnly->Disable();
  m_Panel->m_InfiniteDuration->Disable();
  m_Panel->m_GameplayEvent->Disable();
  m_Panel->m_ShowOnlyMatchingEvents->Disable();
  m_Panel->m_AddEventButton->Disable();
  m_Panel->m_DeleteSelection->Disable();
  m_Panel->m_RebuildMoby->Disable();
  m_Panel->m_RefreshEvents->Disable();
  m_Panel->m_AuditionButton->Disable();
  m_Panel->m_StopAllButton->Disable();
  m_Panel->m_OverrideEventsCheckbox->Disable();
}

void AnimationEventsMainPanel::EnableAll()
{
  m_Panel->m_UpdateClassChoice->Enable();
  m_Panel->m_ClipChoice->Enable();
  m_Panel->m_EventTypeChoice->Enable();
  m_Panel->m_ValueCombo->Enable();
  m_Panel->m_Untrigger->Enable();
  m_Panel->m_DominantOnly->Enable();
  m_Panel->m_InfiniteDuration->Enable();
  m_Panel->m_GameplayEvent->Enable();
  m_Panel->m_ShowOnlyMatchingEvents->Enable();
  m_Panel->m_AddEventButton->Enable();
  m_Panel->m_DeleteSelection->Enable();
  m_Panel->m_RebuildMoby->Enable();
  m_Panel->m_RefreshEvents->Enable();
  m_Panel->m_AuditionButton->Enable();
  m_Panel->m_StopAllButton->Enable();
  m_Panel->m_OverrideEventsCheckbox->Enable();
}

void AnimationEventsMainPanel::AssetOpened( const MobyFileChangeArgs& args )
{
  Populate( args.m_File );
  EnableAll();
}

void AnimationEventsMainPanel::AssetClosed( const MobyFileChangeArgs& args )
{
  DisableAll();
  Clear();

  m_CurrentClip = NULL;
}

void AnimationEventsMainPanel::ClipChanged( const AnimClipChangeArgs& args )
{
  if ( args.m_Clip.Ptr() == m_CurrentClip.Ptr() )
  {
    return;
  }

  m_CurrentClip = args.m_Clip;

  bool found = false;
  for ( u32 i = 0; i < m_Panel->m_ClipChoice->GetCount(); ++i )
  {
    if ( m_Panel->m_ClipChoice->GetClientData( i ) == m_CurrentClip.Ptr() )
    {
      m_Panel->m_ClipChoice->SetSelection( i );

      found = true;
      break;
    }
  }

  if ( !found )
  {
    Console::Error( "Tried to set the clip to a clip that is not in the list!  Clip name: %s\n", m_CurrentClip->GetName() );
    NOC_BREAK();
  }

  m_Panel->m_OverrideEventsCheckbox->SetValue( m_CurrentClip->m_UseOverrideEvents );
}

void AnimationEventsMainPanel::UpdateClassChanged( const UpdateClassChangedArgs& args )
{
  if ( args.m_UpdateClass == m_Panel->m_UpdateClassChoice->GetStringSelection() )
    return;

  PopulateEventTypes();
  PopulateAppliedValues();
}

void AnimationEventsMainPanel::EventTypesChanged( const EventTypesChangedArgs& args )
{
  PopulateEventTypes();
  PopulateAppliedValues();
}

void AnimationEventsMainPanel::OnClipChanged( wxCommandEvent& evt )
{
  m_Manager.SetClip( m_Manager.GetFile()->m_ClipData[ m_Panel->m_ClipChoice->GetSelection() ] );
}

void AnimationEventsMainPanel::OnUpdateClass( wxCommandEvent& evt )
{
  m_Manager.SetUpdateClass( m_Panel->m_UpdateClassChoice->GetSelection() );
}

void AnimationEventsMainPanel::OnAddEvent( wxCommandEvent& evt )
{
  AddEvent();
}

void AnimationEventsMainPanel::AddEvent()
{
  Asset::AnimationEventPtr event = new Asset::AnimationEvent;
  event->m_EventType = m_Panel->m_EventTypeChoice->GetStringSelection().c_str();
  if ( !m_Panel->m_ValueCombo->GetValue().IsEmpty() )
  {
    event->m_Value = atof( m_Panel->m_ValueCombo->GetValue() );
  }
  event->m_Untrigger = m_Panel->m_Untrigger->IsChecked();
  event->m_InfiniteDuration = m_Panel->m_InfiniteDuration->IsChecked();
  event->m_DominantOnly = m_Panel->m_DominantOnly->IsChecked();
  event->m_GameplayEvent = m_Panel->m_GameplayEvent->IsChecked();

  m_Manager.AddEvent( event );
}

void AnimationEventsMainPanel::OnChar( wxKeyEvent& e )
{
  e.Skip();

  m_Manager.GetEditor()->ProcessEvent( e );
}

void AnimationEventsMainPanel::OnDeleteSelection( wxCommandEvent& evt )
{
  m_Manager.DeleteSelected();
}

void AnimationEventsMainPanel::OnRebuildMoby( wxCommandEvent& evt )
{
  tuid assetTuid = m_Manager.GetTuid();
  m_Manager.BuildMoby( assetTuid );
}

void AnimationEventsMainPanel::OnFilterEvents( wxCommandEvent& evt )
{
  m_Manager.SetEventTypeFilter( m_Panel->m_ShowOnlyMatchingEvents->IsChecked() );
}

void AnimationEventsMainPanel::OnAudition( wxCommandEvent& evt )
{
  m_Manager.PlayEvent( m_Panel->m_EventTypeChoice->GetStringSelection().c_str(), atof( m_Panel->m_ValueCombo->GetValue() ), 0, m_Panel->m_Untrigger->IsChecked(), m_Panel->m_InfiniteDuration->IsChecked(), m_Panel->m_DominantOnly->IsChecked(), m_Panel->m_GameplayEvent->IsChecked() );
}

void AnimationEventsMainPanel::OnStopAll( wxCommandEvent& evt )
{
  m_Manager.StopAll();
}

void AnimationEventsMainPanel::OnEventType( wxCommandEvent& evt )
{
  PopulateAppliedValues();
}

void AnimationEventsMainPanel::OnRefreshEvents( wxCommandEvent& evt )
{
  m_Manager.UpdateEventTypes();
}

void AnimationEventsMainPanel::OnGameplayEvent( wxCommandEvent& evt )
{
  m_Manager.UpdateEventTypes();
}

void AnimationEventsMainPanel::OnOverrideEvents( wxCommandEvent& evt )
{
  if ( !m_Manager.SetOverrideEvents( evt.IsChecked() ) )
  {
    m_Panel->m_OverrideEventsCheckbox->SetValue( !evt.IsChecked() );
  }
}

void AnimationEventsMainPanel::OnEntityDropped( const Inspect::FilteredDropTargetArgs& args )
{
  if ( args.m_Paths.size() )
  {
    m_Manager.GetEditor()->PerformOpen( args.m_Paths[ 0 ] );
  }
}

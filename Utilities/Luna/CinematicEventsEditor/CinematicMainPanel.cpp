#include "Precompile.h"

#include "CinematicMainPanel.h"
#include "CinematicEventsPanels.h"
#include "CinematicEventsEditor.h"
#include "CinematicEventsEditorIDs.h"
#include "CinematicEventsEditor.h"

#include "Asset/AssetClass.h"
#include "Asset/AnimationClip.h"
#include "Attribute/AttributeHandle.h"
//#include "Asset/UpdateClassAttribute.h"
#include "Asset/CinematicAttribute.h"

#include "Console/Console.h"

using namespace Luna;
using namespace Asset;
using namespace Attribute;

BEGIN_EVENT_TABLE( CinematicMainPanel, wxEvtHandler )
EVT_CHOICE( CinematicEventsEditorIDs::CinematicList, OnClipChanged )

EVT_CHAR( OnChar )
END_EVENT_TABLE()

CinematicMainPanel::CinematicMainPanel( CinematicEventsManager& manager, CinematicPanel* panel )
: CinematicPanelBase( manager, panel )
{
  Clear();

  m_Manager.AddCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicMainPanel::AssetOpened ) );
  m_Manager.AddCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicMainPanel::AssetClosed ) );
  m_Manager.AddCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicMainPanel::ClipChanged ) );
  m_Manager.AddCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicMainPanel::CinematicLoaded ) );
  m_Manager.AddEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &CinematicMainPanel::EventTypesChanged ) );

  Connect( m_Panel->m_AddEventButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnAddEvent ) );
  Connect( m_Panel->m_DeleteSelection->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnDeleteSelection ) );
  //Connect( m_Panel->m_ShowOnlyMatchingEvents->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnFilterEvents ) );
  //Connect( m_Panel->m_UpdateClassChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnUpdateClass ) );
  Connect( m_Panel->m_RebuildButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnRebuild ) );
  //Connect( m_Panel->m_SyncButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnSync ) );
  Connect( m_Panel->m_StopAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnStopAll ) );
  //Connect( m_Panel->m_EventTypeChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnEventType ) );
  Connect( m_Panel->m_ActorChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnActor ) );
  //Connect( m_Panel->m_SubsystemChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnSubsystem ) );
  //Connect( m_Panel->m_RefreshEvents->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnRefreshEvents ) );
  //Connect( m_Panel->m_GameplayEvent->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnGameplayEvent ) );
  //Connect( m_Panel->m_OverrideEventsCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnOverrideEvents ) );

  // disable everything when we first start up
  DisableAll();
}

CinematicMainPanel::~CinematicMainPanel()
{
  m_Manager.RemoveCinematicOpenedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicMainPanel::AssetOpened ) );
  m_Manager.RemoveCinematicClosedListener( CinematicFileChangeSignature::Delegate ( this, &CinematicMainPanel::AssetClosed ) );
  m_Manager.RemoveCinematicChangeListener( CinematicChangeSignature::Delegate ( this, &CinematicMainPanel::ClipChanged ) );
  m_Manager.RemoveCinematicLoadedListener( CinematicLoadedSignature::Delegate ( this, &CinematicMainPanel::CinematicLoaded ) );
  m_Manager.RemoveEventTypesChangedListener( EventTypesChangedSignature::Delegate ( this, &CinematicMainPanel::EventTypesChanged ) );

  Disconnect( m_Panel->m_AddEventButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnAddEvent ) );
  Disconnect( m_Panel->m_DeleteSelection->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnDeleteSelection ) );
  //Disconnect( m_Panel->m_ShowOnlyMatchingEvents->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnFilterEvents ) );
  //Disconnect( m_Panel->m_UpdateClassChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnUpdateClass ) );
  Disconnect( m_Panel->m_RebuildButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnRebuild ) );
  //Disconnect( m_Panel->m_SyncButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnSync ) );
  Disconnect( m_Panel->m_StopAllButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnStopAll ) );
  //Disconnect( m_Panel->m_EventTypeChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnEventType ) );
  Disconnect( m_Panel->m_ActorChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnActor ) );
  //Disconnect( m_Panel->m_SubsystemChoice->GetId(), wxEVT_COMMAND_CHOICE_SELECTED, wxCommandEventHandler( CinematicMainPanel::OnSubsystem ) );
  //Disconnect( m_Panel->m_RefreshEvents->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnRefreshEvents ) );
  //Disconnect( m_Panel->m_GameplayEvent->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnGameplayEvent ) );
  //Disconnect( m_Panel->m_OverrideEventsCheckbox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( CinematicMainPanel::OnOverrideEvents ) );
}

void CinematicMainPanel::Clear()
{
  //m_Panel->m_UpdateClassChoice->Clear();
  //m_Panel->m_ClipChoice->Clear();
  //m_Panel->m_EventTypeChoice->Clear();
  //m_Panel->m_ValueCombo->Clear();
  m_Panel->m_Untrigger->SetValue( false );
  //m_Panel->m_ShowOnlyMatchingEvents->SetValue( false );
}

void CinematicMainPanel::Populate( const CinematicEventsDocumentPtr& file )
{
  Clear();

  m_Panel->Freeze();

  AttributeViewer<CinematicAttribute> cinematicClass( file->m_CinematicAsset );

  int i = 0;

  //m_Panel->m_ShowOnlyMatchingEvents->SetValue( m_Manager.GetEventTypeFilter() );

  PopulateActorList();
  PopulateJointList();

  // Events
  //PopulateEffectSubsystems();
  //PopulateEffects();
  //PopulateEventTypes();

  //PopulateAppliedValues();

  m_Panel->Thaw();
}

void CinematicMainPanel::PopulateAppliedValues()
{
  //std::string currentValue = m_Panel->m_ValueCombo->GetValue();

  //m_Panel->m_ValueCombo->Clear();
  //std::string eventType = m_Panel->m_EventTypeChoice->GetStringSelection();

  //for each ( const u32 value in m_Manager.GetAppliedValues( eventType ) )
  //{
  //  if ( value != -1 )
  //  {
  //    std::stringstream stream;
  //    stream << value;
      //m_Panel->m_ValueCombo->Append( stream.str() );
  //  }
  //}

  //m_Panel->m_ValueCombo->SetValue( currentValue );
}

void CinematicMainPanel::PopulateActorList()
{
  m_Panel->Freeze();

  m_Panel->m_ActorChoice->Clear();

  m_Panel->m_ActorChoice->Append( "None - Play 2D" );
  for each ( std::string actorName in m_Manager.GetActors() )
  {
    m_Panel->m_ActorChoice->Append( actorName.c_str() );
  }  

  if ( m_Panel->m_ActorChoice->GetCount() > 0 )
  {
    m_Panel->m_ActorChoice->SetSelection( 0 );
  }

  m_Panel->Thaw();
}

void CinematicMainPanel::PopulateJointList()
{
  m_Panel->Freeze();

  m_Panel->m_JointChoice->Clear();

  std::string selectedActor = m_Panel->m_ActorChoice->GetStringSelection().c_str();
  M_ActorToJoint actorJoints = m_Manager.GetActorJoints();
  M_ActorToJoint::iterator iter;
  std::pair< M_ActorToJoint::iterator, M_ActorToJoint::iterator > ret;
  ret = actorJoints.equal_range( selectedActor );

  V_string jointList;
  for ( iter = ret.first; iter != ret.second; ++iter )
  {
    jointList.push_back( (*iter).second );
    //m_Panel->m_JointChoice->Append( (*iter).second );
  }  

  std::sort( jointList.begin(), jointList.end() );
  for ( u32 i = 0 ; i < jointList.size() ; ++i)
  {
    m_Panel->m_JointChoice->Append( jointList[ i ] );
  }

  if ( m_Panel->m_JointChoice->GetCount() > 0 )
  {
    m_Panel->m_JointChoice->SetSelection( 0 );
  }

  m_Panel->Thaw();
}


void CinematicMainPanel::DisableAll()
{
  m_Panel->m_ActorChoice->Disable();
  m_Panel->m_JointChoice->Disable();
  //m_Panel->m_EventTypeChoice->Disable();
  //m_Panel->m_ValueCombo->Disable();
  m_Panel->m_Untrigger->Disable();
  //m_Panel->m_DominantOnly->Disable();
  m_Panel->m_InfiniteDuration->Disable();
  //m_Panel->m_GameplayEvent->Disable();
  //m_Panel->m_ShowOnlyMatchingEvents->Disable();
  m_Panel->m_AddEventButton->Disable();
  m_Panel->m_DeleteSelection->Disable();
  //m_Panel->m_RefreshEvents->Disable();
  m_Panel->m_RebuildButton->Disable();
  //m_Panel->m_SyncButton->Disable();
  m_Panel->m_StopAllButton->Disable();
  //m_Panel->m_OverrideEventsCheckbox->Disable();
}

void CinematicMainPanel::EnableAll()
{
  m_Panel->m_ActorChoice->Enable();
  m_Panel->m_JointChoice->Enable();
  //m_Panel->m_EventTypeChoice->Enable();
  //m_Panel->m_ValueCombo->Enable();
  m_Panel->m_Untrigger->Enable();
  //m_Panel->m_DominantOnly->Enable();
  m_Panel->m_InfiniteDuration->Enable();
  //m_Panel->m_GameplayEvent->Enable();
  //m_Panel->m_ShowOnlyMatchingEvents->Enable();
  m_Panel->m_AddEventButton->Enable();
  m_Panel->m_DeleteSelection->Enable();
  //m_Panel->m_RefreshEvents->Enable();
  m_Panel->m_RebuildButton->Enable();
  //m_Panel->m_SyncButton->Enable();
  m_Panel->m_StopAllButton->Enable();
  //m_Panel->m_OverrideEventsCheckbox->Enable();
}

void CinematicMainPanel::AssetOpened( const CinematicFileChangeArgs& args )
{
  Populate( args.m_File );
  EnableAll();
}

void CinematicMainPanel::AssetClosed( const CinematicFileChangeArgs& args )
{
  DisableAll();
  Clear();

  m_CurrentClip = NULL;
}

void CinematicMainPanel::ClipChanged( const CinematicChangeArgs& args )
{
  if ( args.m_Clip.Ptr() == m_CurrentClip.Ptr() )
  {
    return;
  }

  m_CurrentClip = args.m_Clip;

  bool found = false;
}

void CinematicMainPanel::CinematicLoaded( const CinematicLoadedArgs& args )
{
  PopulateAppliedValues();
}

void CinematicMainPanel::EventTypesChanged( const EventTypesChangedArgs& args )
{
  PopulateAppliedValues();
}

void CinematicMainPanel::OnClipChanged( wxCommandEvent& evt )
{
  //m_Manager.SetClip( m_Manager.GetFile()->m_ClipData[ m_Panel->m_ClipChoice->GetSelection() ] );
}

void CinematicMainPanel::OnCinematic( wxCommandEvent& evt )
{
  //m_Manager.SetUpdateClass( m_Panel->m_UpdateClassChoice->GetSelection() );
}

void CinematicMainPanel::OnAddEvent( wxCommandEvent& evt )
{
  AddEvent();
}

void CinematicMainPanel::AddEvent()
{
  Asset::CinematicEventPtr event = new Asset::CinematicEvent;
  //event->m_EventType = m_Panel->m_EventTypeChoice->GetStringSelection().c_str();
  event->m_ActorName = m_Panel->m_ActorChoice->GetStringSelection().c_str();
  event->m_JointName = m_Panel->m_JointChoice->GetStringSelection().c_str();
  //event->m_Value = atof( m_Panel->m_ValueCombo->GetValue() );
  event->m_Untrigger = m_Panel->m_Untrigger->IsChecked();
  event->m_InfiniteDuration = m_Panel->m_InfiniteDuration->IsChecked();
  //event->m_DominantOnly = m_Panel->m_DominantOnly->IsChecked();
  //event->m_GameplayEvent = m_Panel->m_GameplayEvent->IsChecked();
  //event->m_SubsystemID = m_Panel->m_SubsystemChoice->GetCurrentSelection();
  //event->m_EffectPackageID = m_Manager.GetEffectSpecPackageID( event->m_SubsystemID, m_Panel->m_EffectSpecChoice->GetStringSelection().c_str());
  
  m_Manager.AddEvent( event );
}

void CinematicMainPanel::OnChar( wxKeyEvent& e )
{
  e.Skip();

  m_Manager.GetEditor()->ProcessEvent( e );
}

void CinematicMainPanel::OnDeleteSelection( wxCommandEvent& evt )
{
  m_Manager.DeleteSelected();
}

void CinematicMainPanel::OnFilterEvents( wxCommandEvent& evt )
{
  //m_Manager.SetEventTypeFilter( m_Panel->m_ShowOnlyMatchingEvents->IsChecked() );
}

void CinematicMainPanel::OnRebuild( wxCommandEvent& evt )
{
  m_Manager.Build();
}

void CinematicMainPanel::OnSync( wxCommandEvent& evt )
{
  m_Manager.Sync();
}

void CinematicMainPanel::OnStopAll( wxCommandEvent& evt )
{
  m_Manager.StopAll();
}

void CinematicMainPanel::OnEventType( wxCommandEvent& evt )
{
  PopulateAppliedValues();
}

void CinematicMainPanel::OnActor( wxCommandEvent& evt )
{
  PopulateJointList();
}

void CinematicMainPanel::OnSubsystem( wxCommandEvent& evt )
{
  //PopulateEffects();
}

void CinematicMainPanel::OnRefreshEvents( wxCommandEvent& evt )
{
  m_Manager.UpdateEventTypes();
}

void CinematicMainPanel::OnGameplayEvent( wxCommandEvent& evt )
{
  m_Manager.UpdateEventTypes();
}

void CinematicMainPanel::OnOverrideEvents( wxCommandEvent& evt )
{
  //if ( !m_Manager.SetOverrideEvents( evt.IsChecked() ) )
  //{
    //m_Panel->m_OverrideEventsCheckbox->SetValue( !evt.IsChecked() );
  //}
}
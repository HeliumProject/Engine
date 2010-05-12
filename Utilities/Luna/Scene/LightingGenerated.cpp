///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Mar 19 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "Precompile.h"

#include "LightingJobTree.h"
#include "LightingListLight.h"
#include "LightingListLightingEnvironment.h"
#include "LightingListObject.h"
#include "LightingListProbe.h"
#include "LightingListVolume.h"
#include "LightingListZone.h"

#include "LightingGenerated.h"

///////////////////////////////////////////////////////////////////////////
using namespace Luna;

InnerLightingPanel::InnerLightingPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* mainSizer;
	mainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* mainButtonSizer;
	mainButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonRender = new wxButton( this, ID_DEFAULT, wxT("Render"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	mainButtonSizer->Add( m_ButtonRender, 0, wxALL, 0 );
	
	wxString m_ChoiceRenderTypeChoices[] = { wxT("Standard"), wxT("Preview"), wxT("Compute Casters"), wxT("Cubemap") };
	int m_ChoiceRenderTypeNChoices = sizeof( m_ChoiceRenderTypeChoices ) / sizeof( wxString );
	m_ChoiceRenderType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ChoiceRenderTypeNChoices, m_ChoiceRenderTypeChoices, 0 );
	m_ChoiceRenderType->SetSelection( 0 );
	mainButtonSizer->Add( m_ChoiceRenderType, 0, wxALL, 0 );
	
	wxString m_ChoiceRenderLocationChoices[] = { wxT("Remote"), wxT("Local") };
	int m_ChoiceRenderLocationNChoices = sizeof( m_ChoiceRenderLocationChoices ) / sizeof( wxString );
	m_ChoiceRenderLocation = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ChoiceRenderLocationNChoices, m_ChoiceRenderLocationChoices, 0 );
	m_ChoiceRenderLocation->SetSelection( 0 );
	mainButtonSizer->Add( m_ChoiceRenderLocation, 0, wxALL, 0 );
	
	m_ButtonView = new wxButton( this, ID_DEFAULT, wxT("View"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonView->SetToolTip( wxT("view lighting data for the currently selected objects in the lighting job tree.") );
	
	mainButtonSizer->Add( m_ButtonView, 0, wxALL, 0 );
	
	m_ButtonUpdateCasters = new wxButton( this, wxID_ANY, wxT("Update Shadow Casters"), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonUpdateCasters->SetToolTip( wxT("updates the excluded shadow casters of the lighting jobs selected in the lighting job tree based on rendered 'compute caster' data.") );
	
	mainButtonSizer->Add( m_ButtonUpdateCasters, 0, wxALL, 0 );
	
	m_ButtonUpdateLit = new wxButton( this, wxID_ANY, wxT("Update \"Lit\""), wxDefaultPosition, wxDefaultSize, 0 );
	m_ButtonUpdateLit->SetToolTip( wxT("updates the \"lit\" column for all lightable objects currently loaded.") );
	
	mainButtonSizer->Add( m_ButtonUpdateLit, 0, wxALL, 0 );
	
	m_ButtonCheckSetup = new wxButton( this, ID_DEFAULT, wxT("Check Setup"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonCheckSetup->SetToolTip( wxT("checks objects in the currently loaded scene for lighting setup") );
	
	mainButtonSizer->Add( m_ButtonCheckSetup, 0, wxALL, 0 );
	
	m_ButtonViewUVs = new wxButton( this, ID_DEFAULT, wxT("View UVs"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonViewUVs->SetToolTip( wxT("Opens the UV viewer") );
	
	mainButtonSizer->Add( m_ButtonViewUVs, 0, wxALL, 0 );
	
	
	mainButtonSizer->Add( 0, 4, 0, wxALL, 5 );
	
	m_ButtonQuickSetup = new wxButton( this, ID_DEFAULT, wxT("Quick Setup"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonQuickSetup->SetToolTip( wxT("Renders the currently active zone using the default lighting volume") );
	
	mainButtonSizer->Add( m_ButtonQuickSetup, 0, wxALL, 0 );
	
	m_ButtonQuickRender = new wxButton( this, ID_DEFAULT, wxT("Quick Render"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonQuickRender->SetToolTip( wxT("Renders the currently active zone using the default lighting volume") );
	
	mainButtonSizer->Add( m_ButtonQuickRender, 0, wxALL, 0 );
	
	mainSizer->Add( mainButtonSizer, 0, wxEXPAND|wxLEFT|wxTOP, 1 );
	
	m_VerticalSplitter = new wxSplitterWindow( this, ID_DEFAULT, wxDefaultPosition, wxSize( -1,-1 ), wxSP_LIVE_UPDATE );
	m_VerticalSplitter->SetSashGravity( 0.500000 );
	m_VerticalSplitter->Connect( wxEVT_IDLE, wxIdleEventHandler( InnerLightingPanel::m_VerticalSplitterOnIdle ), NULL, this );
	m_PanelLeft = new wxPanel( m_VerticalSplitter, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL );
	m_PanelLeft->SetMinSize( wxSize( 50,-1 ) );
	
	wxBoxSizer* leftSizer;
	leftSizer = new wxBoxSizer( wxVERTICAL );
	
	m_Tabs = new wxNotebook( m_PanelLeft, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, 0 );
	m_TabObjects = new wxPanel( m_Tabs, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* objectListSizer;
	objectListSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LightableInstanceList = new Luna::LightingListObject( m_TabObjects, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT );
	objectListSizer->Add( m_LightableInstanceList, 1, wxALL|wxEXPAND, 5 );
	
	m_TabObjects->SetSizer( objectListSizer );
	m_TabObjects->Layout();
	objectListSizer->Fit( m_TabObjects );
	m_Tabs->AddPage( m_TabObjects, wxT("Objects"), false );
	m_TabLights = new wxPanel( m_Tabs, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* lightListSizer;
	lightListSizer = new wxBoxSizer( wxVERTICAL );
	
	m_LightList = new Luna::LightingListLight( m_TabLights, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT );
	lightListSizer->Add( m_LightList, 1, wxALL|wxEXPAND, 5 );
	
	m_TabLights->SetSizer( lightListSizer );
	m_TabLights->Layout();
	lightListSizer->Fit( m_TabLights );
	m_Tabs->AddPage( m_TabLights, wxT("Lights"), true );
	m_TabVolumes = new wxPanel( m_Tabs, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* volumeListSizer;
	volumeListSizer = new wxBoxSizer( wxVERTICAL );
	
	m_VolumeList = new Luna::LightingListVolume( m_TabVolumes, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT );
	volumeListSizer->Add( m_VolumeList, 1, wxALL|wxEXPAND, 5 );
	
	m_TabVolumes->SetSizer( volumeListSizer );
	m_TabVolumes->Layout();
	volumeListSizer->Fit( m_TabVolumes );
	m_Tabs->AddPage( m_TabVolumes, wxT("Volumes"), false );
	m_TabProbes = new wxPanel( m_Tabs, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* probeSizer;
	probeSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ProbeList = new Luna::LightingListProbe( m_TabProbes, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT );
	probeSizer->Add( m_ProbeList, 1, wxALL|wxEXPAND, 5 );
	
	m_TabProbes->SetSizer( probeSizer );
	m_TabProbes->Layout();
	probeSizer->Fit( m_TabProbes );
	m_Tabs->AddPage( m_TabProbes, wxT("Probes"), false );
	m_TabLightingEnvironments = new wxPanel( m_Tabs, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* environmentSizer;
	environmentSizer = new wxBoxSizer( wxVERTICAL );
	
	m_EnvironmentList = new Luna::LightingListLightingEnvironment( m_TabLightingEnvironments, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT );
	environmentSizer->Add( m_EnvironmentList, 1, wxALL|wxEXPAND, 5 );
	
	m_TabLightingEnvironments->SetSizer( environmentSizer );
	m_TabLightingEnvironments->Layout();
	environmentSizer->Fit( m_TabLightingEnvironments );
	m_Tabs->AddPage( m_TabLightingEnvironments, wxT("Environments"), false );
	m_TabZones = new wxPanel( m_Tabs, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* zoneSizer;
	zoneSizer = new wxBoxSizer( wxVERTICAL );
	
	m_ZoneList = new Luna::LightingListZone( m_TabZones, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxLC_EDIT_LABELS|wxLC_HRULES|wxLC_REPORT );
	zoneSizer->Add( m_ZoneList, 1, wxALL|wxEXPAND, 5 );
	
	m_TabZones->SetSizer( zoneSizer );
	m_TabZones->Layout();
	zoneSizer->Fit( m_TabZones );
	m_Tabs->AddPage( m_TabZones, wxT("Zones"), false );
	
	leftSizer->Add( m_Tabs, 1, wxEXPAND | wxALL, 0 );
	
	m_PanelLeft->SetSizer( leftSizer );
	m_PanelLeft->Layout();
	leftSizer->Fit( m_PanelLeft );
	m_PanelRight = new wxPanel( m_VerticalSplitter, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxSTATIC_BORDER|wxTAB_TRAVERSAL );
	m_PanelRight->SetMinSize( wxSize( 20,-1 ) );
	
	wxBoxSizer* rightSizer;
	rightSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* jobSizer;
	jobSizer = new wxBoxSizer( wxVERTICAL );
	
	
	jobSizer->Add( 0, 4, 0, wxALL, 0 );
	
	wxBoxSizer* jobButtonSizer;
	jobButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonNewJob = new wxButton( m_PanelRight, ID_DEFAULT, wxT("New"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonNewJob->SetToolTip( wxT("Create a new lighting job.") );
	
	jobButtonSizer->Add( m_ButtonNewJob, 0, wxLEFT, 5 );
	
	m_ButtonDeleteJob = new wxButton( m_PanelRight, ID_DEFAULT, wxT("Delete"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonDeleteJob->SetToolTip( wxT("Delete the selected lighting job(s).") );
	
	jobButtonSizer->Add( m_ButtonDeleteJob, 0, wxALL, 0 );
	
	m_ButtonCopyJob = new wxButton( m_PanelRight, ID_DEFAULT, wxT("Copy"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonCopyJob->SetToolTip( wxT("Make copies of the selected lighting job(s).") );
	
	jobButtonSizer->Add( m_ButtonCopyJob, 0, wxALL, 0 );
	
	
	jobButtonSizer->Add( 0, 0, 0, wxALL, 5 );
	
	m_ButtonAddJobMembers = new wxButton( m_PanelRight, ID_DEFAULT, wxT("Add"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonAddJobMembers->SetToolTip( wxT("Add selected items to the selected lighting job(s).") );
	
	jobButtonSizer->Add( m_ButtonAddJobMembers, 0, wxALL, 0 );
	
	m_ButtonRemoveJobMembers = new wxButton( m_PanelRight, ID_DEFAULT, wxT("Remove"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonRemoveJobMembers->SetToolTip( wxT("Remove the selected items below from the lighting job.") );
	
	jobButtonSizer->Add( m_ButtonRemoveJobMembers, 0, wxALL, 0 );
	
	
	jobButtonSizer->Add( 0, 0, 0, wxALL, 5 );
	
	m_ButtonSort = new wxButton( m_PanelRight, wxID_ANY, wxT("Sort"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonSort->SetToolTip( wxT("Alphabetize the jobs panel.") );
	
	jobButtonSizer->Add( m_ButtonSort, 0, wxALL, 0 );
	
	
	jobButtonSizer->Add( 0, 0, 0, wxALL, 5 );
	
	m_ButtonSync = new wxButton( m_PanelRight, wxID_ANY, wxT("Sync"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	m_ButtonSync->SetToolTip( wxT("Sync lighting data for the selected Jobs") );
	
	jobButtonSizer->Add( m_ButtonSync, 0, wxALL, 0 );
	
	jobSizer->Add( jobButtonSizer, 0, wxEXPAND, 5 );
	
	wxBoxSizer* treeButtonSizer;
	treeButtonSizer = new wxBoxSizer( wxVERTICAL );
	
	m_JobTree = new Luna::LightingJobTree( m_PanelRight, ID_DEFAULT, wxDefaultPosition, wxDefaultSize, wxTR_EDIT_LABELS|wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT|wxTR_MULTIPLE|wxTR_NO_LINES );
	treeButtonSizer->Add( m_JobTree, 1, wxALL|wxEXPAND, 5 );
	
	jobSizer->Add( treeButtonSizer, 1, wxEXPAND, 5 );
	
	rightSizer->Add( jobSizer, 1, wxEXPAND, 5 );
	
	m_PanelRight->SetSizer( rightSizer );
	m_PanelRight->Layout();
	rightSizer->Fit( m_PanelRight );
	m_VerticalSplitter->SplitVertically( m_PanelLeft, m_PanelRight, 0 );
	mainSizer->Add( m_VerticalSplitter, 1, wxALL|wxEXPAND, 1 );
	
	wxBoxSizer* selectButtonSizer;
	selectButtonSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ButtonSelect = new wxButton( this, wxID_ANY, wxT("Select"), wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT );
	selectButtonSizer->Add( m_ButtonSelect, 0, wxALL, 0 );
	
	wxString m_ChoiceSelectTypeChoices[] = { wxT("In Lighting Job"), wxT("In Scene"), wxT("Non Render Targets"), wxT("All Render Targets"), wxT("Unlit Instances"), wxT("Lit Instances") };
	int m_ChoiceSelectTypeNChoices = sizeof( m_ChoiceSelectTypeChoices ) / sizeof( wxString );
	m_ChoiceSelectType = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_ChoiceSelectTypeNChoices, m_ChoiceSelectTypeChoices, 0 );
	m_ChoiceSelectType->SetSelection( 0 );
	selectButtonSizer->Add( m_ChoiceSelectType, 0, wxALL, 0 );
	
	m_ButtonSetTPM = new wxButton( this, wxID_ANY, wxT("Set TPM"), wxDefaultPosition, wxDefaultSize, 0 );
	selectButtonSizer->Add( m_ButtonSetTPM, 0, wxALL, 0 );
	
	m_ValueTPMMax = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	selectButtonSizer->Add( m_ValueTPMMax, 0, wxALL, 0 );
	
	m_ValueTPMStep = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	selectButtonSizer->Add( m_ValueTPMStep, 0, wxALL, 0 );
	
	mainSizer->Add( selectButtonSizer, 0, wxEXPAND|wxLEFT|wxTOP, 1 );
	
	this->SetSizer( mainSizer );
	this->Layout();
}

InnerLightingPanel::~InnerLightingPanel()
{
}

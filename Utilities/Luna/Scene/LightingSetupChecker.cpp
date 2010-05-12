#include "Precompile.h"
#include "LightingSetupChecker.h"
#include "Task/Export.h"
#include "Finder/Finder.h"
#include "Common/Boost/Regex.h" 
#include "LightmapUVViewer.h"
#include "UIToolKit/SortableListView.h"
#include "RemoteConstruct.h"
#include "Entity.h"
#include "EntityAssetSet.h"
using namespace Luna;

enum
{
  BUTTON_Close = wxID_HIGHEST + 1, // declares an id which will be used to call our button
  BUTTON_SetUp_All,
  BUTTON_SetUp_Errors,
  BUTTON_SetUp_Selected,
  BUTTON_ViewUVs_Selected,
  BUTTON_ViewUVs,
  BUTTON_EditInMaya,
  BUTTON_SelectInScene,
  LIST_EntityClass
};

BEGIN_EVENT_TABLE(LightingSetupChecker, wxFrame)
EVT_BUTTON( wxID_CLOSE, LightingSetupChecker::OnClose )
EVT_BUTTON( BUTTON_Close, LightingSetupChecker::OnClose )
EVT_BUTTON( BUTTON_SetUp_Errors, LightingSetupChecker::OnSetupErrors )
EVT_BUTTON( BUTTON_SetUp_Selected, LightingSetupChecker::OnSetupSelected )
EVT_BUTTON( BUTTON_ViewUVs, LightingSetupChecker::OnViewSelectedUVs )
EVT_BUTTON( BUTTON_EditInMaya, LightingSetupChecker::OnEditInMaya )
EVT_BUTTON( BUTTON_SelectInScene, LightingSetupChecker::OnSelectInScene )

EVT_LIST_ITEM_SELECTED( LIST_EntityClass, LightingSetupChecker::OnItemSelected )
EVT_LIST_ITEM_DESELECTED( LIST_EntityClass, LightingSetupChecker::OnItemDeSelected )

EVT_LIST_COL_CLICK( LIST_EntityClass, LightingSetupChecker::OnSortEntityClasses )
END_EVENT_TABLE()


LightingSetupChecker::LightingSetupChecker( wxWindow* parent, SceneEditor* sceneEditor, const std::string& title, int xpos, int ypos, int width, int height )
: wxFrame( parent, -1, title.c_str(), wxPoint( xpos, ypos ), wxSize( width, height ), wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLIP_CHILDREN  )
, m_UVViewer(NULL)
{
  wxBoxSizer* bSizer1;
  bSizer1 = new wxBoxSizer( wxVERTICAL );

  wxBoxSizer* bSizer3;
  bSizer3 = new wxBoxSizer( wxVERTICAL );

  m_TextControl = new wxTextCtrl( this, -1, wxT(""), wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH );
  wxTextAttr attr(*wxWHITE);
  attr.SetFlags(wxTEXT_ATTR_FONT_FACE);
  attr.SetFont(wxFont(8, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  m_TextControl->SetDefaultStyle( attr );
  
  m_ListControl = new wxListCtrl( this, LIST_EntityClass, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
  m_ListControl->InsertColumn( 0, "EntityClasses" );
  m_ListControl->InsertColumn( 1, "Error Message" );
  //m_ListControl->EnableSorting( true );
  
  bSizer3->Add( m_ListControl, 1, wxALL|wxEXPAND, 5 );
  bSizer3->Add( m_TextControl, 1, wxALL|wxEXPAND, 5 );
  bSizer1->Add( bSizer3, 1, wxEXPAND|wxVERTICAL, 5 );

  wxBoxSizer* bSizer4;
  bSizer4 = new wxBoxSizer( wxHORIZONTAL );

  m_CloseButton = new wxButton( this, BUTTON_Close, wxT("Close"), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_CloseButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  m_SetUpErrorButton = new wxButton( this, BUTTON_SetUp_Errors, wxT("Auto Setup Error Classes" ), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_SetUpErrorButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  m_SetUpSelectedButton = new wxButton( this, BUTTON_SetUp_Selected, wxT("Auto Setup Selected" ), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_SetUpSelectedButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  m_ViewUVsButton = new wxButton( this, BUTTON_ViewUVs, wxT("View Selected UVs" ), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_ViewUVsButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  m_EditInMayaButton = new wxButton( this, BUTTON_EditInMaya, wxT("Edit In Maya" ), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_EditInMayaButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  m_SelectInSceneButton = new wxButton( this, BUTTON_SelectInScene, wxT("Select In Scene" ), wxDefaultPosition, wxDefaultSize, 0 );
  bSizer4->Add( m_SelectInSceneButton, 0, wxALIGN_BOTTOM|wxALL, 5 );

  bSizer1->Add( bSizer4, 0, wxALIGN_CENTER_HORIZONTAL, 5 );

  this->SetSizer( bSizer1 );
  this->Layout();

  m_CloseButton->Enable( true );
  m_SetUpErrorButton->Enable( true );
  m_SetUpSelectedButton->Enable( true );
  m_ViewUVsButton->Enable( true );
  m_TextControl->SetBackgroundColour( *wxBLACK );

  m_SceneEditor = sceneEditor;

}

LightingSetupChecker::~LightingSetupChecker()
{

}

void LightingSetupChecker::OnClose( wxCommandEvent& event )
{
  delete this;
}


void LightingSetupChecker::OnSetupErrors( wxCommandEvent& event )
{
  Luna::ExportAssets( m_ErrorEntityClasses, NULL, false, false, true );
}

void LightingSetupChecker::OnSetupSelected( wxCommandEvent& event )
{
  Luna::ExportAssets( m_SelectedClasses, NULL, false, false, true );
}

void LightingSetupChecker::OnViewSelectedUVs( wxCommandEvent& event )
{
  if( !m_UVViewer )
  {
    m_UVViewer = new LightmapUVViewer( this, "Lightmap UV Viewer", 100, 100, 600, 650 );
    m_UVViewer->Enable();
    m_UVViewer->CentreOnScreen();
    m_UVViewer->Show();  

    m_UVViewer->m_ViewerClosedSignature.Add( this, &LightingSetupChecker::OnViewerClosed );
  }

  if( !m_SelectedClasses.empty() )
    m_UVViewer->DoOpen( *m_SelectedClasses.begin() );
}

void LightingSetupChecker::OnItemSelected( wxListEvent& event )
{
  wxListItem item;
  item.SetId( event.m_itemIndex );
  item.SetMask( wxLIST_MASK_DATA );
  m_ListControl->GetItem( item );

  SetupCheckerItem* data = (SetupCheckerItem*)(wxUIntToPtr(item.GetData()));

  m_SelectedClasses.insert(data->m_AssetID);

  if( m_UVViewer )
    m_UVViewer->DoOpen( data->m_AssetID );
}

void LightingSetupChecker::OnItemDeSelected( wxListEvent& event )
{
  wxListItem item;
  item.SetId( event.m_itemIndex );
  item.SetMask( wxLIST_MASK_DATA );
  m_ListControl->GetItem( item );

  SetupCheckerItem* data = (SetupCheckerItem*)(wxUIntToPtr(item.GetData()));

  m_SelectedClasses.erase( data->m_AssetID );
}

void LightingSetupChecker::OnViewerClosed( ViewerClosedArgs& args )
{
  if( m_UVViewer )
  {
    if( m_UVViewer == args.m_ClosedViewer )
      m_UVViewer = NULL;
  }
}

wxListCtrl* LightingSetupChecker::GetListCtrl()
{
  return m_ListControl;
}


void LightingSetupChecker::OnEditInMaya( wxCommandEvent& event )
{
 
  if( !m_SelectedClasses.empty() )
  {
   
    // connect if necessary and open the specified AssetClass's art file in maya
    if( RemoteConstruct::EstablishConnection() )
    {
      RPC::EntityEditParam param;
      {
       
        tuid id =  *m_SelectedClasses.begin();
        memcpy( &param.m_EntityAsset, &id, sizeof(param.m_EntityAsset) );
      }
      RemoteConstruct::g_ConstructionTool->OpenArtFile( &param );
    }
  }
}

void LightingSetupChecker::OnSelectInScene( wxCommandEvent& event )
{
  if( m_SceneEditor )
  {
    OS_SelectableDumbPtr objects;
    Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();

    S_tuid::iterator idItor = m_SelectedClasses.begin();
    S_tuid::iterator idEnd  = m_SelectedClasses.end();

    for( ; idItor != idEnd; ++idItor )
    {   
      tuid& id = *idItor;

      std::vector< Entity* > entities;
      currentScene->GetAll< Entity >( entities );

      std::vector< Entity* >::iterator itor = entities.begin();
      std::vector< Entity* >::iterator end  = entities.end();

      for( ; itor != end; ++itor )
      {
        if( (*itor)->GetClassSet()->GetEntityAssetID() == id )
        {
          objects.Append( *itor );
          break;
        }
      }
    }

    if( !objects.Empty() )
    {
      m_SceneEditor->PostCommand( new SceneSelectCommand( currentScene, objects ) );
    }
  }
}

int wxCALLBACK compareEntityClasses( long item1, long item2, long sortData )
{
  SetupCheckerItem* data1 = static_cast< SetupCheckerItem* >( wxUIntToPtr( item1 ) );
  SetupCheckerItem* data2 = static_cast< SetupCheckerItem* >( wxUIntToPtr( item2 ) );


  if( data1->m_AssetName < data2->m_AssetName )
    return -1;
  else if( data1->m_AssetName > data2->m_AssetName )
    return 1;
  else
    return 0;
}
int wxCALLBACK compareErrorMessages( long item1, long item2, long sortData )
{
  SetupCheckerItem* data1 = static_cast< SetupCheckerItem* >( wxUIntToPtr( item1 ) );
  SetupCheckerItem* data2 = static_cast< SetupCheckerItem* >( wxUIntToPtr( item2 ) );

  u32 count1 = 0;
  u32 count2 = 0;
  for( u32 i = 0; i < SetupCheckerStates::NumSetupCheckerStates; ++i )
  {
    if( data1->m_ErrorFlags[i] )
      count1 += i + 1;
    if( data2->m_ErrorFlags[i] )
      count2 += i + 1;
  }
  
  if( count1 < count2 )
    return 1;
  else if( count1 > count2 )
    return -1;
  else
    return 0;
}

void LightingSetupChecker::OnSortEntityClasses( wxListEvent& event )
{
  if( event.GetColumn() == 0 )
  {
    GetListCtrl()->SortItems( &compareEntityClasses, 0 );
  }
  else if (event.GetColumn() == 1 )
  {
    GetListCtrl()->SortItems( &compareErrorMessages, 0 );
  }
}

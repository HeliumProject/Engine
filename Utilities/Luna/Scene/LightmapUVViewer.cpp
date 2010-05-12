#include "Precompile.h"
#include "LightmapUVViewer.h"

#include "Attribute/AttributeHandle.h"
#include "Asset/AssetClass.h"
#include "Asset/ArtFileAttribute.h"

#include "File/ManagedFileDialog.h"
#include "File/Manager.h"
#include "FileBrowser/FileBrowser.h"
#include "Finder/ContentSpecs.h"
#include "Finder/AssetSpecs.h"
#include "Finder/Finder.h"
#include "FileSystem/FileSystem.h"

#include "Content/Scene.h"
#include <omp.h>

using namespace Luna;

BEGIN_EVENT_TABLE(LightmapUVViewer, wxFrame)
EVT_BUTTON( wxID_CLOSE, LightmapUVViewer::OnClose )
EVT_MENU( wxID_OPEN, LightmapUVViewer::OnOpen )
EVT_PAINT( LightmapUVViewer::OnPaint )
EVT_MOUSEWHEEL( LightmapUVViewer::OnScale )
EVT_MOTION( LightmapUVViewer::OnMiddleDrag )
EVT_MIDDLE_DOWN( LightmapUVViewer::StartMiddleDrag )
EVT_MIDDLE_UP( LightmapUVViewer::EndMiddleDrag )
EVT_MENU( LightmapUVViewerIDs::SearchForFile, LightmapUVViewer::OnFind )
END_EVENT_TABLE()

LightmapUVViewer::LightmapUVViewer( wxWindow* parent, const std::string& title, int xpos, int ypos, int width, int height )
: wxFrame( parent, -1, title.c_str(), wxPoint( xpos, ypos ), wxSize( width, height ), wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLIP_CHILDREN  )
, m_Scale(1.0f)
, m_WidthAndHeight( 500 )
, m_OriginOffset( 50, 550 )
{
  InitializeUI();  
}

LightmapUVViewer::LightmapUVViewer( wxWindow* parent, tuid assetID, const std::string& title, int xpos, int ypos, int width, int height )
: wxFrame( parent, -1, title.c_str(), wxPoint( xpos, ypos ), wxSize( width, height ), wxCAPTION|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLIP_CHILDREN  )
, m_Scale(1.0f)
, m_WidthAndHeight( 500 )
, m_OriginOffset( 50, 550 )
{
  InitializeUI();
  DoOpen( assetID );
}

void LightmapUVViewer::InitializeUI()
{
  m_MenuBar = new wxMenuBar( 0 );
  m_MenuFile = new wxMenu();

  wxMenuItem* menuItem = new wxMenuItem( m_MenuFile, wxID_OPEN, "Open\tCtrl-o" );
  menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_FILE_OPEN ) );
  m_MenuFile->Append( menuItem );

  menuItem = new wxMenuItem( m_MenuFile, LightmapUVViewerIDs::SearchForFile, "Find...\tCtrl-f" );
  menuItem->SetBitmap( wxArtProvider::GetBitmap( wxART_FIND ) );
  m_MenuFile->Append( menuItem );

  m_MenuBar->Append( m_MenuFile, wxT("File") );

  this->SetMenuBar( m_MenuBar );

  wxBoxSizer* sizer;
  sizer = new wxBoxSizer( wxVERTICAL );

  m_CheckOverlapsBox = new wxCheckBox( this, wxID_ANY, wxT("Check Overlap"), wxPoint( -1,-1 ), wxDefaultSize, 0 );

  sizer->Add( m_CheckOverlapsBox, 0, wxALIGN_RIGHT, 0 );

  this->SetSizer( sizer );
  this->Layout();

  m_StatusBar = this->CreateStatusBar( 1, wxST_SIZEGRIP, wxID_ANY );

  m_CheckOverlapsBox->Connect( m_CheckOverlapsBox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LightmapUVViewer::OnOverlapCheckBox ), NULL, this );

}


LightmapUVViewer::~LightmapUVViewer()
{
  m_ViewerClosedSignature.Raise( ViewerClosedArgs( *this ) );
  m_CheckOverlapsBox->Disconnect( m_CheckOverlapsBox->GetId(), wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( LightmapUVViewer::OnOverlapCheckBox ), NULL, this );
}

void LightmapUVViewer::OnOverlapCheckBox( wxCommandEvent& event )
{

  if( m_CheckOverlapsBox->IsChecked() )
  {
    if( !m_UVShells.empty() )
    {
      LoadUVShells();
    }
  }
  else
  {
    m_OverlapingTris.clear();
    Refresh();
  }
}
void LightmapUVViewer::OnMiddleDrag( wxMouseEvent& event )
{

  if( event.Dragging() && event.MiddleIsDown() )
  {
    wxPoint pos = event.GetPosition();

    //pos.y = -pos.y;
    wxPoint dir = pos - m_LastMousePos;
    m_OriginOffset += dir;

    m_LastMousePos = pos;
    Refresh();
  }
}
void LightmapUVViewer::StartMiddleDrag( wxMouseEvent& event )
{
  m_LastMousePos = event.GetPosition();
}

void LightmapUVViewer::EndMiddleDrag( wxMouseEvent& event )
{
  m_LastMousePos.x = 0, m_LastMousePos.y = 0;
}


void LightmapUVViewer::OnClose( wxCommandEvent& event )
{
  delete this;
}


void LightmapUVViewer::OnScale( wxMouseEvent& event )
{
  int wheelDelta =  event.GetWheelRotation();

  int width, height;
  this->GetClientSize( &width, &height );
  f32 x = m_OriginOffset.x - ( (f32)width * 0.5f );
  f32 y = m_OriginOffset.y - ( (f32)height * 0.5f );

  if( wheelDelta > 0 )
  {
    m_Scale *= 2.f;   

    f32 newX = x * 2.f;
    f32 newY = y * 2.f;

    m_OriginOffset.x += newX - x;
    m_OriginOffset.y += newY - y;
  }
  else
  {
    m_Scale /= 2.f;   

    f32 newX = x / 2.f;
    f32 newY = y / 2.f;

    m_OriginOffset.x += newX - x;
    m_OriginOffset.y += newY - y;
  }

  Refresh();
}

void LightmapUVViewer::Draw()
{
  wxPaintDC dc(this);
  dc.SetAxisOrientation( true, true );

  f32 scaledWidthAndHeight = m_WidthAndHeight * m_Scale;

  // draw center point
  int width, height;
  this->GetClientSize( &width, &height );

  dc.SetPen( *wxCYAN_PEN );
  dc.DrawPoint(wxPoint( (f32)width * 0.5f, (f32)-height * 0.5f ) );

  dc.SetBrush( *wxTRANSPARENT_BRUSH );

  dc.SetPen( *wxBLACK_PEN );
  dc.DrawRectangle( m_OriginOffset.x, -m_OriginOffset.y, scaledWidthAndHeight, scaledWidthAndHeight );

  dc.SetPen( *wxRED_PEN );
  std::map<u32, std::set<u32> >::iterator findItor;

  wxPoint points[3];

  std::vector< V_u32 >::iterator itor = m_TriangleUVIndices.begin();
  std::vector< V_u32 >::iterator end  = m_TriangleUVIndices.end();

  std::vector< Math::V_Vector2 >::iterator uvShellItor = m_UVShells.begin();
  for( ; itor != end; ++itor, ++uvShellItor )
  {
    findItor = m_OverlapingTris.find( uvShellItor - m_UVShells.begin() );

    u32 size = (u32)itor->size();
    for( u32 i = 0; i < size; i+=3 )
    {
      points[0].x = (*uvShellItor)[(*itor)[i]].x * scaledWidthAndHeight; 
      points[0].y = (*uvShellItor)[(*itor)[i]].y  * scaledWidthAndHeight;
      points[1].x = (*uvShellItor)[(*itor)[i+1]].x * scaledWidthAndHeight;
      points[1].y = (*uvShellItor)[(*itor)[i+1]].y * scaledWidthAndHeight;
      points[2].x = (*uvShellItor)[(*itor)[i+2]].x * scaledWidthAndHeight;
      points[2].y = (*uvShellItor)[(*itor)[i+2]].y * scaledWidthAndHeight;

      dc.DrawPolygon( 3, points, m_OriginOffset.x, -m_OriginOffset.y );
    }
  }

  if( !m_OverlapingTris.empty() )
  {
    itor = m_TriangleUVIndices.begin();
    uvShellItor = m_UVShells.begin();
    dc.SetPen( *wxGREEN_PEN );
    for( ; itor != end; ++itor, ++uvShellItor )
    {
      findItor = m_OverlapingTris.find( uvShellItor - m_UVShells.begin() );

      u32 size = (u32)itor->size();
      for( u32 i = 0; i < size; i+=3 )
      {
        if( findItor != m_OverlapingTris.end() )
        {
          if( findItor->second.find( i/3 ) != findItor->second.end() )
          {
            points[0].x = (*uvShellItor)[(*itor)[i]].x   * scaledWidthAndHeight; 
            points[0].y = (*uvShellItor)[(*itor)[i]].y   * scaledWidthAndHeight;
            points[1].x = (*uvShellItor)[(*itor)[i+1]].x * scaledWidthAndHeight;
            points[1].y = (*uvShellItor)[(*itor)[i+1]].y * scaledWidthAndHeight;
            points[2].x = (*uvShellItor)[(*itor)[i+2]].x * scaledWidthAndHeight;
            points[2].y = (*uvShellItor)[(*itor)[i+2]].y * scaledWidthAndHeight;

            dc.DrawPolygon( 3, points, m_OriginOffset.x, -m_OriginOffset.y );
          }      
        }
      } 
    }
  }



}
void LightmapUVViewer::OnPaint( wxPaintEvent& event )
{
  Draw();
}

bool LightmapUVViewer::DoOpen( tuid id )
{
  m_AssetID = id;
  return LoadUVShells();
}

bool LightmapUVViewer::LoadUVShells()
{
  m_OverlapingTris.clear();
  m_TriangleUVIndices.clear();
  m_UVShells.clear();

  Asset::AssetClass* asset = Asset::AssetClass::FindAssetClass( m_AssetID );

  if( asset )
  {
    Attribute::AttributeViewer< Asset::ArtFileAttribute > artFile( asset );
    if( artFile.Valid() )
    {
      std::string artFilePath = artFile->GetFilePath();

      std::string staticFile = FinderSpecs::Content::STATIC_DECORATION.GetExportFile( artFilePath, artFile->m_FragmentNode );
      Content::Scene staticScene( staticFile );

#pragma omp sections
      {
#pragma omp section
        {
          staticScene.GetUVShells( Content::UVSetTypes::Lightmap, m_UVShells, m_TriangleUVIndices );
        }
#pragma omp section
        {
          if( m_CheckOverlapsBox->IsChecked() )
            staticScene.LightmapUVsOverlap( m_OverlapingTris );
        }
      }
      FileSystem::GetLeaf( artFilePath,  artFilePath );
      
      std::stringstream stream;
      stream << artFilePath << "  % UV Space Used: " << staticScene.UVSurfaceArea( Content::UVSetTypes::Lightmap ) * 100;  

      m_StatusBar->SetStatusText( stream.str().c_str() );

      Refresh();

      return true;
    }
  }
  return false;
}

void LightmapUVViewer::OnOpen( wxCommandEvent& event )
{
  static std::string defaultPath = Finder::ProjectAssets();
  File::ManagedFileDialog browserDlg( this, "Open", defaultPath.c_str(), "", "", UIToolKit::FileDialogStyles::DefaultOpen );

  browserDlg.AddFilter( FinderSpecs::Asset::ASSET_EDITOR_FILTER.GetDialogFilter() );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    std::string path = browserDlg.GetPath();
    tuid id = File::GlobalManager().GetID( path );
    if ( DoOpen( id ) && id != TUID::Null )
    {
      defaultPath = browserDlg.GetPath();
      FileSystem::StripLeaf( defaultPath );
    }
  }
}
void LightmapUVViewer::OnFind( wxCommandEvent& event )
{
  File::FileBrowser browserDlg( this, -1, "Asset Finder: Open" );
  //browserDlg.EnableMultipleSelection();

  browserDlg.SetFilter( FinderSpecs::Asset::ASSET_EDITOR_FILTER );

  if ( browserDlg.ShowModal() == wxID_OK )
  {
    tuid id = File::GlobalManager().GetID( browserDlg.GetPath() );

    if( id != TUID::Null )
    {
      DoOpen( id );
    }
  }
}


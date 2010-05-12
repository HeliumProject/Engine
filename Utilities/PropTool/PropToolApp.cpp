#include "PropToolApp.h"
#include "PropToolFrame.h"
#include "resource.h"
#include "wx/msw/private.h"

DECLARE_APP( PropToolApp )
IMPLEMENT_APP( PropToolApp )

PropToolApp::PropToolApp()
{
}

PropToolApp::~PropToolApp()
{
}

#ifdef _DEBUG
long& g_BreakOnAlloc (_crtBreakAlloc);
#endif //_DEBUG

int PropToolApp::OnRun()
{
  HANDLE handle;
  wxIcon icon;
  wxIconBundle bundle;
  
  handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 );
  icon.SetHICON( handle );
  icon.SetWidth( 16 );
  icon.SetHeight( 16 );
  bundle.AddIcon( icon );

  handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 );
  icon.SetHICON( handle );
  icon.SetWidth( 32 );
  icon.SetHeight( 32 );
  bundle.AddIcon( icon );

  handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 64, 64, 0 );
  icon.SetHICON( handle );
  icon.SetWidth( 64 );
  icon.SetHeight( 64 );
  bundle.AddIcon( icon );

  handle = ::LoadImage( wxGetInstance(), MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 128, 128, 0 );
  icon.SetHICON( handle );
  icon.SetWidth( 128 );
  icon.SetHeight( 128 );
  bundle.AddIcon( icon );

  PropToolFrame* frame = new PropToolFrame( (wxWindow*)NULL );
  frame->SetIcons(bundle);
  frame->Show();
  SetTopWindow( frame );

  return __super::OnRun();
}

#ifdef _DEBUG
void main()
{
  wxEntry( ::GetModuleHandle(NULL), NULL, ::GetCommandLine(), SW_SHOWNORMAL );
}
#endif
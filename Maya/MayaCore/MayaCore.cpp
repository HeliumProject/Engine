#include "MayaCore.h"
#include "Application.h"

#include "AppUtils/AppUtils.h"

#include <maya/MGlobal.h>

u32 MayaCore::g_InitCount = 0;

void MayaCore::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    if ( MGlobal::executeCommand("loadPlugin MayaCore") != MS::kSuccess )
    {
      NOC_BREAK();
    }

    wxInitialize();
    wxImage::AddHandler( new wxPNGHandler() );

    try
    {
      AppUtils::Startup();
    }
    catch ( const Nocturnal::Exception& ex )
    {
      std::cerr << "Unable to initialize: " << ex.what() << std::endl;
    }
  }
}

void MayaCore::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    try
    {
      AppUtils::Shutdown();
    }
    catch ( const Nocturnal::Exception& ex )
    {
      std::cerr << "Unable to shutdown: " << ex.what() << std::endl;
    }

    wxUninitialize();

    MGlobal::executeCommand("evalDeferred \"unloadPlugin MayaCore\"");
  }
}
#include "EditorPch.h"
#include "Perforce.h"
#include "P4Provider.h"

#define MAX_ERR_LENGTH 200

using namespace Helium;
using namespace Helium::Perforce;

static uint32_t g_InitCount = 0;
static Provider g_Provider;

void Perforce::Initialize()
{
  if ( ++g_InitCount == 1 )
  {
    g_Provider.Initialize();
    RCS::SetProvider( &g_Provider );
  }
}

void Perforce::Cleanup()
{
  if ( --g_InitCount == 0 )
  {
    RCS::SetProvider( NULL );
    g_Provider.Cleanup();
  }
}

WaitInterface::~WaitInterface()
{

}

#include "Precompile.h"
#include "MayaUtils.h"

#include <maya/MLibrary.h>
#include <maya/MString.h>
#include <maya/MStatus.h>

#include "Foundation/Log.h"

#include "Export.h"

extern bool g_MayaFileOpen;
extern std::string g_MayaFile;

namespace Maya
{
  int g_InitCount = 0;
  bool IsInitialized()
  {
    return g_InitCount > 0;
  }

  bool Init(const std::string& progname)
  {
    if ( ++g_InitCount > 1 )
    {
      return true;
    }

    g_MayaFileOpen = false;
    g_MayaFile = "";

    // initialize library
    MStatus stat;

    stat = MLibrary::initialize (true, (char *)progname.c_str(), true);

    if (!stat)
    {
      throw Nocturnal::Exception("(Maya::Init) could not initialize Maya libraries for '%s'", progname.c_str());
    }

    return true;
  }

  void Cleanup(int errorcode)
  {
    if ( --g_InitCount == 0 )
    {
      g_MayaFileOpen = false;
      g_MayaFile = "";

      g_foundNodes.clear();

      MLibrary::cleanup(errorcode);
    }
  }

  bool MEL(const std::string& command, std::string &result)
  {
    MString res;

    bool ret = MGlobal::executeCommand(MString (command.c_str()), res) == MS::kSuccess;

    result = res.asChar();

    if (result.size() == 0)
      return true;
    else
      return ret;
  }
}



//
// START ULTRA HACKY SOLUTION TO MAYA BUG
//

// Hi Geoff,
//
// We were able to track down the source of the bug and we even have a workaround for you.
// Your scene file contains IK and there's a problem with the dynamic loading of required dlls on Windows.
// As a result, the dll containing the IK system was not being initialized correctly. This caused the file load failure.
//
// Here's the workaround:
// 1. Add OpenMayaAnim.lib to the link libraries in your project settings.
// 2. Add a reference to an object defined in OpenMayaAnim.lib such as "MFnAnimCurve bogusFn".
//    This will force OpenMayaAnim.lib to be linked into the standalone. Keep in mind that you need to create this bogus
//    object or else your library mode application won't run correctly.
//

#include <maya/MFnAnimCurve.h>

// DO NOT REMOVE THIS UNDER ANY CIRCUMSTANCES
static MFnAnimCurve bogusFn;

//
// END ULTRA HACKY SOLUTION TO MAYA BUG
//



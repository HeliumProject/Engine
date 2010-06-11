#pragma once

#include <string>

#include "Platform/Assert.h"

namespace AppUtils
{
  //
  // Version numbers have four parts:
  //
  //  1.2.3.4
  //
  //   1 - Project Version: Encapsulates the game title for which these tools were built for.
  //     You cannot use a different titles' tools.
  //
  //   2 - Compatible Version: Encapsulates the generation or broad compatibility of the tools.
  //     Different generations are not compatible, and machines with outdated compatible versions are required
  //     to update as they may harm other users.
  //
  //   3 - Feature version: Encapsulates a feature change but interoperatbility with tools of the same generation.
  //     Features are released on regular intervals, and these new features are released with tools that have
  //     this version number incremented.  The user is warned, but not update is required (but heavily recommended).
  //
  //   4 - Patch version: Encapsulates the patch release number of the current feature version
  //     Patches are released on as-needed bases.  The user is informed, but not update is required (but recommended).
  //

  namespace VersionResults
  {
    enum VersionResult
    {
      Match,
      Patch,
      Feature,
      Compatible,
      Project,
      Fail,
      Count,
    };

    static const char* Strings[] =
    {
      "Match",
      "Patch",
      "Feature",
      "Compatible",
      "Project",
      "Fail"
    };

    NOC_COMPILE_ASSERT( sizeof(Strings) / sizeof(const char*) == Count );
  }

  typedef VersionResults::VersionResult VersionResult;

  void CheckVersion();
}
#include "Precompile.h"
#include "RCSTypes.h"

#include <sstream>

using namespace Helium;

std::string RCS::GetChangesetIdAsString( const uint64_t changesetId )
{
  std::stringstream str;
  if ( changesetId == DefaultChangesetId )
  {
    str << "default";
  }
  else
  {
    str << changesetId;
  }

  return str.str();
}

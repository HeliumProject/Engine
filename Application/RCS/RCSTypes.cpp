#include "RCSTypes.h"

#include <sstream>

using namespace RCS;

std::string RCS::GetChangesetIdAsString( const u64 changesetId )
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

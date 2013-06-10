#include "ApplicationPch.h"
#include "RCSTypes.h"

#include <sstream>

using namespace Helium;

std::string RCS::GetChangesetIdAsString( const uint64_t changesetId )
{
  std::stringstream str;
  if ( changesetId == DefaultChangesetId )
  {
    str << TXT( "default" );
  }
  else
  {
    str << changesetId;
  }

  return str.str();
}

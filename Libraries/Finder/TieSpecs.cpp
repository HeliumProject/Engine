#include "TieSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Tie::BUILT_FILE               ( "Tie::BUILT_FILE",                "built",                Extension::DATA,           "99 - dreagan fix vertmapping" );
  const FileSpec Tie::HELPER_FILE              ( "Tie::HELPER_FILE",               "helper",               Extension::DATA,           "32 - dreagan fix vertmapping" );
  const FileSpec Tie::HELPER_ENUM_MANIFEST_FILE( "Tie::HELPER_ENUM_MANIFEST_FILE", "helper_enum_manifest", Extension::TXT,            "3 - cedwards fix ProcessMip" );
  const FileSpec Tie::MENTALRAY_FILE           ( "Tie::MENTALRAY_FILE",            "object",               Extension::MENTALRAY,      "42 - drr" );
}

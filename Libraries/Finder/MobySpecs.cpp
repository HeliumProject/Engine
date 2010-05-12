#include "MobySpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Moby::BUILT_FILE                 ( "Moby::BUILT_FILE",               "built",                Extension::DATA,  "146 - cedwards DT Core 2421");
  const FileSpec Moby::BUILT_ENUM_MANIFEST_FILE   ( "Moby::BUILT_ENUM_MANIFEST_FILE", "built_enum_manifest",  Extension::TXT,   "2 - cedwards fix ProcessMip");
  
  // This is a bit of a misnomer: there won't be a single file with the 
  // specified name.  Instead, each unique animation clip that is assigned
  // to a moby, will generate a file named anim_bspheres_<CLIPTUID>.dat, 
  // which will be used to skip the animation bsphere calculation if possible 
  // as a way of speeding up the moby builder.
  const FileSpec Moby::INTERMEDIATE_BSPHERE_FILE  ( "Moby::INTERMEDIATE_BSPHERE_FILE", "anim_bspheres",       Extension::DATA, "2 - cedwards");
  
  const FileSpec Moby::SHADER_LIST_FILE           ( "Moby::SHADER_LIST_FILE",         "shader_list",          Extension::DATA,  "2 - jfaust");
  const FileSpec Moby::SKELETON_DBG_FILE          ( "Moby::SKELETON_DBG_FILE",        "skeletondbg",          Extension::DATA,  "5 - rsmith_ik" );
}

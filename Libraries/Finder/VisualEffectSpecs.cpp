#include "VisualEffectSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec VisualEffect::TYPE_CONFIG_FILE            ( "VisualEffect::TYPE_CONFIG_FILE",               "FXTTypes",                       Extension::XML );
  const FolderSpec VisualEffect::CONFIG_FOLDER             ( "VisualEffect::CONFIG_FOLDER",                  "config/fxt/",                    FolderRoots::ProjectRoot );

  const FileSpec VisualEffect::BUILT_EFFECT_FILE           ( "VisualEffect::BUILT_EFFECT_FILE",              "effect",                         Extension::DATA,              "7 - tlouden_FXT_CUSTOM_COMPOSITE_SHADERS" );
  const FileSpec VisualEffect::BUILT_MANIFEST_FILE         ( "VisualEffect::BUILT_MANIFEST_FILE",            "manifest",                       Extension::REFLECT_BINARY,    "7 - tlouden_FXT_CUSTOM_COMPOSITE_SHADERS" );

  const FileSpec VisualEffect::SYSTEM_TEXTURE_PACK         ( "VisualEffect::SYSTEM_TEXTURE_PACK",            "vfx_system_texel",               Extension::DATA, "3 - cglave_dxtopt");
  const FileSpec VisualEffect::SYSTEM_TEXTURE_PACK_HEADER  ( "VisualEffect::SYSTEM_TEXTURE_PACK_HEADER",     "vfx_system_header",              Extension::DATA, "3 - cglave_dxtopt");

  const FileSpec VisualEffect::EFFECT_TEXTURE_PACK         ( "VisualEffect::EFFECT_TEXTURE_PACK",            "effect" );
}
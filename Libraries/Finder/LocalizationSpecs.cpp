#include "LocalizationSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Localization::CHARACTER_SET_FILE   ( "Localization::CHARACTER_SET_FILE",     "character_set",  Extension::DATA );
  const FileSpec Localization::TQD_FILE             ( "Localization::TQD_FILE",               "tqd",            Extension::DATA );
  const FileSpec Localization::PACKAGE_FILE         ( "Localization::PACKAGE_FILE",           "" );
  const FileSpec Localization::PACKAGE_LIST_FILE    ( "Localization::PACKAGE_LIST_FILE",      "package_list.sym.rb" );
  const FileSpec Localization::DIALOGUE_PACKAGE_FILE( "Localization::DIALOGUE_PACKAGE_FILE",  "dialogue",       "9_tlouden - intermediate_dialogue_ccs patch" );

  const FileSpec Localization::DATABASE             ( "Localization::DATABASE",               "" );

  const FileSpec Localization::CHARACTER_KEYS_FILE  ( "Localization::CHARACTER_KEY_FILE",     "character_keys", Extension::XML );
}

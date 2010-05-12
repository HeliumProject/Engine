#include "SymbolSpecs.h" 

#include "Finder.h"
#include "ExtensionSpecs.h"
#include "CodeSpecs.h" 

#include "Common/Environment.h"

namespace FinderSpecs
{
  const FileSpec Symbol::SHARED_SYMBOL_FILE                 ("Symbol::SHARED_SYMBOL_FILE",            "shared.sym.irb");
  const FileSpec Symbol::SYMBOL_LIST_FILE                   ("Symbol::SYMBOL_LIST_FILE",              "files_to_process",  Extension::TXT, ""); 
  const FileSpec Symbol::VT_GEN_IMPL_FILE                   ("Symbol::VT_GEN_IMPL_FILE",              "vt_gen_table",      Extension::IMPL,   ""); 
  const FileSpec Symbol::VT_GEN_ENUMS_FILE                  ("Symbol::VT_GEN_ENUMS_FILE",             "vt_gen_enums",      Code::HEADER_FILE_EXTENSION, ""); 
  const FileSpec Symbol::VT_GEN_TABLE_FILE                  ("Symbol::VT_GEN_TABLE_FILE",             "vt_gen_table",      Extension::REFLECT_BINARY, ""); 
  const FileSpec Symbol::VT_GEN_TABLE_DATA_FILE             ("Symbol::VT_GEN_TABLE_DATA_FILE",        "vt_gen_table_data", Extension::REFLECT_BINARY, ""); 

  const FileSpec Symbol::HEADERPARSER_ALIGN_FILE            ("Symbol::HEADERPARSER_ALIGN_FILE",       "headerparser_align",           Extension::XML, ""); 
  const FileSpec Symbol::HEADERPARSER_DEPENDENCY_DATA_FILE  ("Symbol::HEADERPARSER_DEPENDENCY_DATA",  "headerparser_dependency_data", Extension::CFG, ""); 

  const FolderSpec Symbol::HEADERPARSER_OUTPUT_PS3          ("Symbol::HEADERPARSER_OUTPUT_PS3",       "output/headerparser",  FolderRoots::ProjectCode); 
  const FolderSpec Symbol::VTGEN_OUTPUT_PS3                 ("Symbol::VTGEN_OUTPUT_PS3",              "output/vtgen",         FolderRoots::ProjectCode); 
  const FolderSpec Symbol::SYMBOLS_PS3                      ("Symbol::SYMBOLS_PS3",                   "assets/symbols",       FolderRoots::ProjectCode); 

  const FileSpec Symbol::WADPACK_SYMBOL_TABLE_FILE          ("Symbol::WADPACK_SYMBOL_TABLE_FILE",     "wadstruct.sym.irb"); 
  const FileSpec Symbol::AUTOFIXUP_OUTPUT_FILE              ("Symbol::AUTOFIXUP_OUTPUT_FILE",         "", "1 - jfaust"); 

  void Symbol::Init()
  {
    // awesome
    std::string value;
    if (Nocturnal::GetEnvVar( NOCTURNAL_STUDIO_PREFIX"GAME", value ))
    {
      {
        // awesome++
        FolderSpec& folder = (FolderSpec&)Symbol::HEADERPARSER_OUTPUT_PS3;
        folder.SetValue( std::string (folder.GetValue()) + '/' + value + "/ps3" );
      }

      {
        // awesome++
        FolderSpec& folder = (FolderSpec&)Symbol::VTGEN_OUTPUT_PS3;
        folder.SetValue( std::string (folder.GetValue()) + '/' + value + "/ps3" );
      }

      {
        // awesome++
        FolderSpec& folder = (FolderSpec&)Symbol::SYMBOLS_PS3;
        folder.SetValue( value + '/' + folder.GetValue() + "/ps3" );
      }
    }
    else
    {
      throw Nocturnal::Exception( NOCTURNAL_STUDIO_PREFIX"GAME is not defined in the environment" );
    }
  }
}

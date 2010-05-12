#pragma once

#include "API.h" 
#include "FinderSpec.h" 

namespace FinderSpecs
{
  class FINDER_API Symbol
  {
  public:
    const static FileSpec SHARED_SYMBOL_FILE; 
    const static FileSpec SYMBOL_LIST_FILE; 

    const static FileSpec VT_GEN_IMPL_FILE; 
    const static FileSpec VT_GEN_ENUMS_FILE; 
    const static FileSpec VT_GEN_TABLE_FILE; 
    const static FileSpec VT_GEN_TABLE_DATA_FILE; 

    const static FileSpec HEADERPARSER_ALIGN_FILE; 
    const static FileSpec HEADERPARSER_DEPENDENCY_DATA_FILE; 

    const static FolderSpec HEADERPARSER_OUTPUT_PS3; 
    const static FolderSpec VTGEN_OUTPUT_PS3;
    const static FolderSpec SYMBOLS_PS3; 

    const static FileSpec WADPACK_SYMBOL_TABLE_FILE; 
    const static FileSpec AUTOFIXUP_OUTPUT_FILE;

    static void Init();
  };
}

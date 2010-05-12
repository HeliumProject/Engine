#pragma once

#include "API.h" 
#include "FinderSpec.h" 

namespace FinderSpecs
{
  class FINDER_API Lipsync
  {
  public: 
    const static FolderSpec DATA_FOLDER; 
    const static FolderSpec DATA_P2V_FOLDER; 
    const static FolderSpec BUILT_FOLDER; 

    const static FileSpec   CHARACTERS_XML_FILE; 

  }; 


}

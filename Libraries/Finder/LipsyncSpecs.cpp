#include "Finder.h" 
#include "LipsyncSpecs.h" 
#include "ExtensionSpecs.h" 


namespace FinderSpecs
{
  const FolderSpec Lipsync::DATA_FOLDER    ("Lipsync::DATA_FOLDER",     "lipsync",        FolderRoots::ProjectAssets); 
  const FolderSpec Lipsync::DATA_P2V_FOLDER("Lipsync::DATA_P2V_FOLDER", "lipsync/p2v",    FolderRoots::ProjectAssets); 
  const FolderSpec Lipsync::BUILT_FOLDER   ("Lipsync::BUILT_FOLDER",    "built/lipsync",  FolderRoots::ProjectAssets); 

  const FileSpec   Lipsync::CHARACTERS_XML_FILE("Lipsync::CHARACTERS_XML_FILE", "characters", Extension::XML); 
}

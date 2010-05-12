#include "OcclusionSpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FileSpec Occlusion::PROCESSED_MAIN_FILE ( "Occlusion::MAIN_FILE",       "occltopc",           Extension::DATA );
  const FileSpec Occlusion::PROCESSED_GRID_FILE ( "Occlusion::GRID_FILE",       "occlgrid_" );
  const FileSpec Occlusion::DATABASE_FILE       ( "Occlusion::DATABASE_FILE",   "occlusiondb",        Extension::DATA );
  const FileSpec Occlusion::CLUSTERS_FILE       ( "Occlusion::CLUSTERS_FILE",   "occlusionclusters",  Extension::DATA );

  const FolderSpec Occlusion::GRIDS_FOLDER      ( "Occlusion::GRIDS_FOLDER",    "occlusion/grids",    FolderRoots::ProjectAssets );
  const FolderSpec Occlusion::BUILT_FOLDER      ( "Occlusion::BUILT_FOLDER",    "occlusion/built",    FolderRoots::ProjectAssets );
}
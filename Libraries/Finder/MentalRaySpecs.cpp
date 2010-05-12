#include "MentalRaySpecs.h"

#include "Finder.h"
#include "ExtensionSpecs.h"

namespace FinderSpecs
{
  const FolderSpec MentalRay::CONFIG_FOLDER ( "MentalRay::CONFIG_FOLDER",    "config/mentalray",   FolderRoots::ProjectRoot );

  const FileSpec MentalRay::HEADER_FILE         ( "MentalRay::HEADER_FILE",         "header",             Extension::MENTALRAY );
  const FileSpec MentalRay::DISK_SAMPLES_FILE   ( "MentalRay::DISK_SAMPLES_FILE",   "disksamples",        Extension::DATA );
  const FileSpec MentalRay::VOLUME_FILE         ( "MentalRay::VOLUME_FILE",         "volume",             Extension::MENTALRAY );
  const FileSpec MentalRay::MENTALRAY_DATA      ( "MentalRay::MENTALRAY_DATA",      "mentalraydata",      Extension::DATA,   "1" );
  const FileSpec MentalRay::MENTALRAY_MANIFEST  ( "MentalRay::MENTALRAY_MANIFEST",  "manifest",           Extension::REFLECT_BINARY,   "4 - jvalenzu" );
}

#pragma once

#include "API.h"
#include "FinderSpec.h"

namespace FinderSpecs
{
  class FINDER_API MentalRay
  {
  public:
    const static FolderSpec CONFIG_FOLDER;
    const static FileSpec HEADER_FILE;
    const static FileSpec DISK_SAMPLES_FILE;
    const static FileSpec VOLUME_FILE;
    const static FileSpec MENTALRAY_DATA;
    const static FileSpec MENTALRAY_MANIFEST;

  };
}

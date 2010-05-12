#include "FileBackedAttribute.h"

#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "File/Manager.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( FileBackedAttribute );

FileBackedAttribute::FileBackedAttribute()
{
}


Attribute::AttributeCategoryType FileBackedAttribute::GetCategoryType() const
{
  return Attribute::AttributeCategoryTypes::File;
}

std::string FileBackedAttribute::GetFilePath() const
{
  std::string filePath = "";

  tuid fileID = GetFileID();

  if ( fileID != TUID::Null )
  {
    // you may think there needs to be a try/catch here, but consumer APIs should handle exceptional cases
    File::GlobalManager().GetPath( fileID, filePath );
  }

  return filePath;
}
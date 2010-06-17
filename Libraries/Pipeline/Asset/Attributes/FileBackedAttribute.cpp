#include "FileBackedAttribute.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( FileBackedAttribute );

Attribute::AttributeCategoryType FileBackedAttribute::GetCategoryType() const
{
  return Attribute::AttributeCategoryTypes::File;
}

const Nocturnal::Path& FileBackedAttribute::GetPath() const
{
    return m_Path;
}

void FileBackedAttribute::SetPath( const Nocturnal::Path& path )
{
    m_Path = path;
}
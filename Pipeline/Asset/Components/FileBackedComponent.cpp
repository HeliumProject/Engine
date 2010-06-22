#include "FileBackedComponent.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( FileBackedComponent );

Component::ComponentCategoryType FileBackedComponent::GetCategoryType() const
{
  return Component::ComponentCategoryTypes::File;
}

const Nocturnal::Path& FileBackedComponent::GetPath() const
{
    return m_Path;
}

void FileBackedComponent::SetPath( const Nocturnal::Path& path )
{
    m_Path = path;
}
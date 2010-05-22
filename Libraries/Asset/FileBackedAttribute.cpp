#include "FileBackedAttribute.h"

#include "Console/Console.h"
#include "FileSystem/FileSystem.h"

using namespace Asset;

REFLECT_DEFINE_ABSTRACT( FileBackedAttribute );

Attribute::AttributeCategoryType FileBackedAttribute::GetCategoryType() const
{
  return Attribute::AttributeCategoryTypes::File;
}

File::Reference& FileBackedAttribute::GetFileReference() const
{
    return *m_FileReference;
}

void FileBackedAttribute::SetFileReference( const File::Reference& fileReference )
{
    if ( m_FileReference )
    {
        delete m_FileReference;
    }
    
    m_FileReference = new File::Reference( fileReference );
}
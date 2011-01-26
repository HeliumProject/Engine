#include "Foundation/Reflect/Data/TypeIDData.h"

#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( TypeIDData );

TypeIDData::TypeIDData()
{

}

TypeIDData::~TypeIDData()
{

}

void TypeIDData::ConnectData(void* data)
{
    m_Data.Connect( data );
}

bool TypeIDData::Set(Data* data, uint32_t flags)
{
    const TypeIDData* rhs = SafeCast<TypeIDData>(data);
    if (!rhs)
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

bool TypeIDData::Equals(Object* object)
{
    const TypeIDData* rhs = SafeCast<TypeIDData>(object);
    if (!rhs)
    {
        return false;
    }

    return *m_Data == *rhs->m_Data;
}

void TypeIDData::Serialize(Archive& archive) const
{
    const Type* type = *m_Data;

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            if ( type )
            {
                xml.GetStream() << "<![CDATA[" << type->m_Name << "]]>";
            }

            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            uint32_t crc = type ? Crc32( type->m_Name ) : BeginCrc32();
            binary.GetStream().Write(&crc); 
            break;
        }
    }
}

void TypeIDData::Deserialize(Archive& archive)
{
    const Type* type = NULL;

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            std::streamsize size = xml.GetStream().ObjectsAvailable(); 
            tstring str;
            str.resize( (size_t)size );
            xml.GetStream().ReadBuffer(const_cast<tchar_t*>(str.c_str()), size);
            type = Registry::GetInstance()->GetType( str.c_str() );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            uint32_t crc;
            binary.GetStream().Read(&crc);
            type = Registry::GetInstance()->GetType( crc );
            break;
        }
    }

    if ( type )
    {
        *m_Data = type;
    }
}

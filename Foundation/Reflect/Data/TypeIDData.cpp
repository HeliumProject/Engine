#include "FoundationPch.h"
#include "Foundation/Reflect/Data/TypeIDData.h"

#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::TypeIDData );

using namespace Helium;
using namespace Helium::Reflect;

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

void TypeIDData::Serialize(ArchiveBinary& archive)
{
    const Type* type = *m_Data;
    uint32_t crc = type ? Crc32( type->m_Name ) : BeginCrc32();
    archive.GetStream().Write(&crc); 
}

void TypeIDData::Deserialize(ArchiveBinary& archive)
{
    uint32_t crc;
    archive.GetStream().Read(&crc);

    const Type* type = Registry::GetInstance()->GetType( crc );
    if ( type )
    {
        *m_Data = type;
    }
}

void TypeIDData::Serialize(ArchiveXML& archive)
{
    const Type* type = *m_Data;
    if ( type )
    {
        //archive.GetStream() << "<![CDATA[" << type->m_Name << "]]>";
        archive.WriteString(type->m_Name);
    }
}

void TypeIDData::Deserialize(ArchiveXML& archive)
{
//     std::streamsize size = archive.GetStream().ElementsAvailable(); 
//     tstring str;
//     str.resize( (size_t)size );
//     archive.GetStream().ReadBuffer(const_cast<tchar_t*>(str.c_str()), size);

    tstring str;
    archive.ReadString(str);

    const Type* type = Registry::GetInstance()->GetType( str.c_str() );
    if ( type )
    {
        *m_Data = type;
    }
}

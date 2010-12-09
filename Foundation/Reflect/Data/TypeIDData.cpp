#include "Foundation/Reflect/Data/TypeIDData.h"

#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS( TypeIDData );

TypeIDData::TypeIDData()
{

}

TypeIDData::~TypeIDData()
{

}

void TypeIDData::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

bool TypeIDData::Set(const Reflect::Data* s, uint32_t flags)
{
    const TypeIDData* rhs = Reflect::ConstObjectCast<TypeIDData>(s);
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

bool TypeIDData::Equals(const Reflect::Data* s) const
{
    const TypeIDData* rhs = Reflect::ConstObjectCast<TypeIDData>(s);
    if (!rhs)
    {
        return false;
    }

    return m_Data.Get() == rhs->m_Data.Get();
}

void TypeIDData::Serialize(Archive& archive) const
{
    const Reflect::Type* type = m_Data.Get();

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

            int32_t index = binary.GetStrings().Insert( type ? *type->m_Name : TXT("") );
            binary.GetStream().Write(&index); 
            break;
        }
    }
}

void TypeIDData::Deserialize(Archive& archive)
{
    tstring str;

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            std::streamsize size = xml.GetStream().ElementsAvailable(); 
            str.resize( (size_t)size );
            xml.GetStream().ReadBuffer(const_cast<tchar_t*>(str.c_str()), size);
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            int32_t index;
            binary.GetStream().Read(&index); 
            str = binary.GetStrings().Get(index);
            break;
        }
    }

    const Reflect::Type* type = Reflect::Registry::GetInstance()->GetType( Name( str.c_str() ) );

    if ( type )
    {
        m_Data.Set( type );
    }
}

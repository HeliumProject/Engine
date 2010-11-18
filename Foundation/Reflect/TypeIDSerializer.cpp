#include "Foundation/Reflect/TypeIDSerializer.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS( TypeIDSerializer );

TypeIDSerializer::TypeIDSerializer ()
{

}

TypeIDSerializer::~TypeIDSerializer()
{

}

void TypeIDSerializer::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

bool TypeIDSerializer::Set(const Reflect::Serializer* s, uint32_t flags)
{
    const TypeIDSerializer* rhs = Reflect::ConstObjectCast<TypeIDSerializer>(s);
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

bool TypeIDSerializer::Equals(const Reflect::Serializer* s) const
{
    const TypeIDSerializer* rhs = Reflect::ConstObjectCast<TypeIDSerializer>(s);
    if (!rhs)
    {
        return false;
    }

    return m_Data.Get() == rhs->m_Data.Get();
}

void TypeIDSerializer::Serialize(Archive& archive) const
{
    const Reflect::Type* type = Reflect::Registry::GetInstance()->GetType( m_Data.Get() );

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

            int32_t index = binary.GetStrings().Insert( type ? type->m_Name : TXT("") );
            binary.GetStream().Write(&index); 
            break;
        }
    }
}

void TypeIDSerializer::Deserialize(Archive& archive)
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

    const Reflect::Type* type = Reflect::Registry::GetInstance()->GetType( str );

    if ( type )
    {
        m_Data.Set( type->m_TypeID );
    }
}

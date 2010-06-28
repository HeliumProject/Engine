#include "TypeIDSerializer.h"
#include "ArchiveBinary.h"
#include "ArchiveXML.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS( TypeIDSerializer );

TypeIDSerializer::TypeIDSerializer ()
{

}

TypeIDSerializer::~TypeIDSerializer()
{

}

bool TypeIDSerializer::IsCompact() const
{ 
    return true; 
}

void TypeIDSerializer::ConnectData(Nocturnal::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Nocturnal::HybridPtr<DataType> (data.Address(), data.State()) );
}

bool TypeIDSerializer::Set(const Reflect::Serializer* s, u32 flags)
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
                xml.GetStream() << "<![CDATA[" << type->m_ShortName << "]]>";
            }

            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            i32 index = binary.GetStrings().Insert( type ? type->m_ShortName : TXT("") );
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
            xml.GetStream().ReadBuffer(const_cast<tchar*>(str.c_str()), size);
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            i32 index;
            binary.GetStream().Read(&index); 
            str = binary.GetStrings().GetString(index);
            break;
        }
    }

    const Reflect::Type* type = Reflect::Registry::GetInstance()->GetType( str );

    if ( type )
    {
        m_Data.Set( type->m_TypeID );
    }
}

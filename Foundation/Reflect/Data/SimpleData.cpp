#include "FoundationPch.h"
#include "Foundation/Reflect/Data/SimpleData.h"

#include "Foundation/Memory/Endian.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

template <class T>
SimpleData<T>::SimpleData()
{

}

template <class T>
SimpleData<T>::~SimpleData()
{

}

template <class T>
void SimpleData<T>::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template <class T>
bool SimpleData<T>::Set(Data* src, uint32_t flags)
{
    const SimpleDataT* rhs = SafeCast<SimpleDataT>(src);
    if (!rhs)
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template <class T>
bool SimpleData<T>::Equals(Object* object)
{
    const SimpleDataT* rhs = SafeCast<SimpleDataT>(object);
    if (!rhs)
    {
        return false;
    }

    return *rhs->m_Data == *m_Data;
}

template <class T>
void SimpleData<T>::Serialize(ArchiveBinary& archive)
{
    const T* data = m_Data;
    archive.GetStream().Write( data ); 
}

template <class T>
void SimpleData<T>::Deserialize(ArchiveBinary& archive)
{
    T* data = m_Data;
    archive.GetStream().Read( data ); 
}

template <class T>
void SimpleData<T>::Serialize(ArchiveXML& archive)
{
    archive.GetStream() << *m_Data;
}

template <class T>
void SimpleData<T>::Deserialize(ArchiveXML& archive)
{
    archive.GetStream() >> *m_Data;
}

template <class T>
tostream& SimpleData<T>::operator>>(tostream& stream) const
{
    stream << *m_Data;

    return stream;
}

template <class T>
tistream& SimpleData<T>::operator<<(tistream& stream)
{
    stream >> *m_Data;

    if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
    {
        Object* object = static_cast< Object* >( m_Instance );
        object->RaiseChanged( m_Field );
    }

    return stream;
}

//
// Specializations
//

template <>
void StlStringData::Serialize(ArchiveBinary& archive)
{
    archive.GetStream().WriteString( *m_Data ); 
}

template <>
void StlStringData::Deserialize(ArchiveBinary& archive)
{
    archive.GetStream().ReadString( *m_Data );
}

template <>
void StlStringData::Serialize(ArchiveXML& archive)
{
    archive.GetStream() << TXT( "<![CDATA[" ) << *m_Data << TXT( "]]>" );
}

template <>
void StlStringData::Deserialize(ArchiveXML& archive)
{
    std::streamsize size = archive.GetStream().ElementsAvailable(); 
    m_Data->resize( (size_t)size );
    archive.GetStream().ReadBuffer( const_cast<tchar_t*>( (*m_Data).c_str() ), size );
}

template<>
tostream& StlStringData::operator>>(tostream& stream) const
{
    stream << *m_Data;

    return stream;
}

template<>
tistream& StlStringData::operator<<(tistream& stream)
{
    std::streamsize size = stream.rdbuf()->in_avail();
    m_Data->resize( (size_t) size);
    stream.read( const_cast<tchar_t*>( (*m_Data).c_str()), size );

    return stream;
}

template <>
void UInt8Data::Serialize(ArchiveBinary& archive)
{
    const uint8_t* data = m_Data;
    archive.GetStream().Write( data ); 
}

template <>
void UInt8Data::Deserialize(ArchiveBinary& archive)
{
    const uint8_t* data = m_Data;
    archive.GetStream().Read( data ); 
}

template <>
void UInt8Data::Serialize(ArchiveXML& archive)
{
    uint16_t tmp = *m_Data;
    archive.GetStream() << tmp;
}

template <>
void UInt8Data::Deserialize(ArchiveXML& archive)
{
    uint16_t tmp;
    archive.GetStream() >> tmp;
    *m_Data = (uint8_t)tmp;
}

template<>
tostream& UInt8Data::operator>>(tostream& stream) const
{
    uint16_t val = *m_Data;
    stream << val;
    return stream;
}

template<>
tistream& UInt8Data::operator<<(tistream& stream)
{
    uint16_t val;
    stream >> val;
    *m_Data = (uint8_t)val;
    return stream;
}

template <>
void Int8Data::Serialize(ArchiveBinary& archive)
{
    const int8_t* data = m_Data;
    archive.GetStream().Write( data ); 
}

template <>
void Int8Data::Deserialize(ArchiveBinary& archive)
{
    const int8_t* data = m_Data;
    archive.GetStream().Read( data ); 
}

template <>
void Int8Data::Serialize(ArchiveXML& archive)
{
    int16_t tmp = *m_Data;
    archive.GetStream() << tmp;
}

template <>
void Int8Data::Deserialize(ArchiveXML& archive)
{
    int16_t tmp;
    archive.GetStream() >> tmp;
    *m_Data = (char)tmp;
}

template<>
tostream& Int8Data::operator>>(tostream& stream) const
{
    int16_t val = *m_Data;
    stream << val;

    return stream;
}

template<>
tistream& Int8Data::operator<<(tistream& stream)
{
    int16_t val;
    stream >> val;
    *m_Data = (uint8_t)val;

    return stream;
}

template SimpleData<tstring>;
template SimpleData<bool>;
template SimpleData<uint8_t>;
template SimpleData<int8_t>;
template SimpleData<uint16_t>;
template SimpleData<int16_t>;
template SimpleData<uint32_t>;
template SimpleData<int32_t>;
template SimpleData<uint64_t>;
template SimpleData<int64_t>;
template SimpleData<float32_t>;
template SimpleData<float64_t>;
template SimpleData<Helium::GUID>;
template SimpleData<Helium::TUID>;

template SimpleData<Vector2>;
template SimpleData<Vector3>;
template SimpleData<Vector4>;

template SimpleData<Matrix3>;
template SimpleData<Matrix4>;

template SimpleData<Color3>;
template SimpleData<HDRColor3>;
template SimpleData<Color4>;
template SimpleData<HDRColor4>;

REFLECT_DEFINE_OBJECT( Helium::Reflect::StlStringData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::BoolData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt8Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int8Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt16Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int16Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int64Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Float32Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Float64Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::GUIDData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::TUIDData );

REFLECT_DEFINE_OBJECT( Helium::Reflect::Vector2Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Vector3Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Vector4Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Matrix3Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Matrix4Data );

REFLECT_DEFINE_OBJECT( Helium::Reflect::Color3Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Color4Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::HDRColor3Data );
REFLECT_DEFINE_OBJECT( Helium::Reflect::HDRColor4Data );
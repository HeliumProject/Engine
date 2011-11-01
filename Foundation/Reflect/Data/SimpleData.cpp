
#include "FoundationPch.h"

#include "SimpleData.h"

#define API_DEFINE HELIUM_FOUNDATION_API
#define TEMPLATE_NAME SimpleData
#include "Foundation/Reflect/Data/SimpleDataTemplate.cpp.inl"
#undef API_DEFINE
#undef TEMPLATE_NAME

//
// Specializations
//

template <>
void StlStringData::Serialize(ArchiveBinary& archive)
{
    archive.WriteString( *m_Data );
}

template <>
void StlStringData::Deserialize(ArchiveBinary& archive)
{
    archive.ReadString( *m_Data );
}

template <>
void StlStringData::Serialize(ArchiveXML& archive)
{
    archive.WriteString( *m_Data );
}

template <>
void StlStringData::Deserialize(ArchiveXML& archive)
{
    archive.ReadString( *m_Data );
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

REFLECT_DEFINE_OBJECT(StlStringData);
REFLECT_DEFINE_OBJECT(BoolData);
REFLECT_DEFINE_OBJECT(UInt8Data);
REFLECT_DEFINE_OBJECT(Int8Data);
REFLECT_DEFINE_OBJECT(UInt16Data);
REFLECT_DEFINE_OBJECT(Int16Data);
REFLECT_DEFINE_OBJECT(UInt32Data);
REFLECT_DEFINE_OBJECT(Int32Data);
REFLECT_DEFINE_OBJECT(UInt64Data);
REFLECT_DEFINE_OBJECT(Int64Data);
REFLECT_DEFINE_OBJECT(Float32Data);
REFLECT_DEFINE_OBJECT(Float64Data);
REFLECT_DEFINE_OBJECT(GUIDData);
REFLECT_DEFINE_OBJECT(TUIDData);

REFLECT_DEFINE_OBJECT(Vector2Data);
REFLECT_DEFINE_OBJECT(Vector3Data);
REFLECT_DEFINE_OBJECT(Vector4Data);
REFLECT_DEFINE_OBJECT(Matrix3Data);
REFLECT_DEFINE_OBJECT(Matrix4Data);

REFLECT_DEFINE_OBJECT(Color3Data);
REFLECT_DEFINE_OBJECT(Color4Data);
REFLECT_DEFINE_OBJECT(HDRColor3Data);
REFLECT_DEFINE_OBJECT(HDRColor4Data);

REFLECT_DEFINE_OBJECT(NameData);
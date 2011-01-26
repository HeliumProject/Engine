#include "Foundation/Reflect/Data/SimpleData.h"

#include "Foundation/Memory/Endian.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

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
void SimpleData<T>::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetStream() << *m_Data;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            const T* data = m_Data;
            binary.GetStream().Write( data ); 
            break;
        }
    }
}

template <class T>
void SimpleData<T>::Serialize(const Helium::BasicBufferPtr& buffer, const tchar_t* debugStr) const
{
    T val = *m_Data;

    Helium::Swizzle( val, buffer->GetByteOrder() != Helium::ByteOrders::LittleEndian );

    buffer->AddBuffer( (const uint8_t*)&val, sizeof(T), debugStr );
}

template <class T>
tostream& SimpleData<T>::operator>> (tostream& stream) const
{
    stream << *m_Data;

    return stream;
}

template <class T>
void SimpleData<T>::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));
            
            xml.GetStream() >> *m_Data;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            T* data = m_Data;
            binary.GetStream().Read( data ); 
            break;
        }
    }
}

template <class T>
tistream& SimpleData<T>::operator<< (tistream& stream)
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

// must escape strings to account for special "evil" characters... like ", &, `, etc...
template <>
void StlStringData::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetStream() << TXT( "<![CDATA[" ) << *m_Data << TXT( "]]>" );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().WriteString( *m_Data ); 
            break;
        }
    }
}

// keep reading the string until we run out of buffer
template <>
void StlStringData::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            std::streamsize size = xml.GetStream().ObjectsAvailable(); 
            m_Data->resize( (size_t)size );
            xml.GetStream().ReadBuffer( const_cast<tchar_t*>( (*m_Data).c_str() ), size );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().ReadString( *m_Data );
            break;
        }
    }
}

template<>
tostream& StlStringData::operator>> (tostream& stream) const
{
    stream << *m_Data;

    return stream;
}

template<>
tistream& StlStringData::operator<< (tistream& stream)
{
    std::streamsize size = stream.rdbuf()->in_avail();
    m_Data->resize( (size_t) size);
    stream.read( const_cast<tchar_t*>( (*m_Data).c_str()), size );

    return stream;
}

// this is a char, we must treat it as a number
template <>
void UInt8Data::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            uint16_t tmp = *m_Data;
            xml.GetStream() << tmp;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            const uint8_t* data = m_Data;
            binary.GetStream().Write( data ); 
            break;
        }
    }
}

template <>
void UInt8Data::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            uint16_t tmp;
            xml.GetStream() >> tmp;
            *m_Data = (unsigned char)tmp;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            const uint8_t* data = m_Data;
            binary.GetStream().Read( data ); 
            break;
        }
    }
}

template<>
tostream& UInt8Data::operator>> (tostream& stream) const
{
    uint16_t val = *m_Data;
    stream << val;

    return stream;
}

template<>
tistream& UInt8Data::operator<< (tistream& stream)
{
    uint16_t val;
    stream >> val;
    *m_Data = (uint8_t)val;

    return stream;
}

// this is a char, we must treat it as a number
template <>
void Int8Data::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            int16_t tmp = *m_Data;
            xml.GetStream() << tmp;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            const int8_t* data = m_Data;
            binary.GetStream().Write( data ); 
            break;
        }
    }
}

template <>
void Int8Data::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            int16_t tmp;
            xml.GetStream() >> tmp;
            *m_Data = (char)tmp;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            const int8_t* data = m_Data;
            binary.GetStream().Read( data ); 
            break;
        }
    }
}

template<>
tostream& Int8Data::operator>> (tostream& stream) const
{
    int16_t val = *m_Data;
    stream << val;

    return stream;
}

template<>
tistream& Int8Data::operator<< (tistream& stream)
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
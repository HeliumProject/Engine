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
void SimpleData<T>::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

template <class T>
bool SimpleData<T>::Set(const Data* src, uint32_t flags)
{
    const SimpleDataT* rhs = ConstObjectCast<SimpleDataT>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

template <class T>
bool SimpleData<T>::Equals(const Data* s) const
{
    const SimpleDataT* rhs = ConstObjectCast<SimpleDataT>(s);
    if (!rhs)
    {
        return false;
    }

    return rhs->m_Data.Get() == m_Data.Get();
}

template <class T>
void SimpleData<T>::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetStream() << m_Data.Get();
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().Write(m_Data.Ptr()); 
            break;
        }
    }
}

template <class T>
void SimpleData<T>::Serialize(const Helium::BasicBufferPtr& buffer, const tchar_t* debugStr) const
{
    T val = m_Data.Get();

    Helium::Swizzle( val, buffer->GetByteOrder() != Helium::ByteOrders::LittleEndian );

    buffer->AddBuffer( (const uint8_t*)&val, sizeof(T), debugStr );
}

template <class T>
tostream& SimpleData<T>::operator>> (tostream& stream) const
{
    stream << m_Data.Get();

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
            
            xml.GetStream() >> m_Data.Ref();
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().Read(m_Data.Ptr()); 
            break;
        }
    }
}

template <class T>
tistream& SimpleData<T>::operator<< (tistream& stream)
{
    stream >> m_Data.Ref();

    if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
    {
        Element* element = (Element*)m_Instance;
        element->RaiseChanged( m_Field );
    }

    return stream;
}

//
// Specializations
//

// must escape strings to account for special "evil" characters... like ", &, `, etc...
template <>
void StringData::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetStream() << TXT( "<![CDATA[" ) << m_Data.Get() << TXT( "]]>" );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().WriteString( m_Data.Get() ); 
            break;
        }
    }
}

// keep reading the string until we run out of buffer
template <>
void StringData::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            std::streamsize size = xml.GetStream().ElementsAvailable(); 
            m_Data->resize( (size_t)size );
            xml.GetStream().ReadBuffer(const_cast<tchar_t*>(m_Data->c_str()), size);
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().ReadString( m_Data.Ref() );
            break;
        }
    }
}

template<>
tostream& StringData::operator>> (tostream& stream) const
{
    stream << m_Data.Get();

    return stream;
}

template<>
tistream& StringData::operator<< (tistream& stream)
{
    std::streamsize size = stream.rdbuf()->in_avail();
    m_Data->resize( (size_t) size);
    stream.read(const_cast<tchar_t*>(m_Data.Get().c_str()), size);

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

            uint16_t tmp = m_Data.Get();
            xml.GetStream() << tmp;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().Write(m_Data.Ptr()); 
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
            m_Data.Set( (unsigned char)tmp );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().Read(m_Data.Ptr()); 
            break;
        }
    }
}

template<>
tostream& UInt8Data::operator>> (tostream& stream) const
{
    uint16_t val = m_Data.Get();
    stream << val;

    return stream;
}

template<>
tistream& UInt8Data::operator<< (tistream& stream)
{
    uint16_t val;
    stream >> val;
    m_Data.Set( (uint8_t)val );

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

            int16_t tmp = m_Data.Get();
            xml.GetStream() << tmp;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().Write(m_Data.Ptr()); 
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
            m_Data.Set( (char)tmp );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            binary.GetStream().Read(m_Data.Ptr()); 
            break;
        }
    }
}

template<>
tostream& Int8Data::operator>> (tostream& stream) const
{
    int16_t val = m_Data.Get();
    stream << val;

    return stream;
}

template<>
tistream& Int8Data::operator<< (tistream& stream)
{
    int16_t val;
    stream >> val;
    m_Data.Set( (uint8_t)val );

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

REFLECT_DEFINE_CLASS(StringData);
REFLECT_DEFINE_CLASS(BoolData);
REFLECT_DEFINE_CLASS(UInt8Data);
REFLECT_DEFINE_CLASS(Int8Data);
REFLECT_DEFINE_CLASS(UInt16Data);
REFLECT_DEFINE_CLASS(Int16Data);
REFLECT_DEFINE_CLASS(UInt32Data);
REFLECT_DEFINE_CLASS(Int32Data);
REFLECT_DEFINE_CLASS(UInt64Data);
REFLECT_DEFINE_CLASS(Int64Data);
REFLECT_DEFINE_CLASS(Float32Data);
REFLECT_DEFINE_CLASS(Float64Data);
REFLECT_DEFINE_CLASS(GUIDData);
REFLECT_DEFINE_CLASS(TUIDData);

REFLECT_DEFINE_CLASS(Vector2Data);
REFLECT_DEFINE_CLASS(Vector3Data);
REFLECT_DEFINE_CLASS(Vector4Data);
REFLECT_DEFINE_CLASS(Matrix3Data);
REFLECT_DEFINE_CLASS(Matrix4Data);

REFLECT_DEFINE_CLASS(Color3Data);
REFLECT_DEFINE_CLASS(Color4Data);
REFLECT_DEFINE_CLASS(HDRColor3Data);
REFLECT_DEFINE_CLASS(HDRColor4Data);
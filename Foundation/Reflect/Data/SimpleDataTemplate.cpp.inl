//
// THIS FILE IS UNUSUAL
//
// Include it only within a .cpp where you are defining a new type of simple data!



#include "Foundation/Memory/Endian.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;



template <class T>
TEMPLATE_NAME<T>::TEMPLATE_NAME()
{

}

template <class T>
TEMPLATE_NAME<T>::~TEMPLATE_NAME()
{

}

template <class T>
void TEMPLATE_NAME<T>::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template <class T>
bool TEMPLATE_NAME<T>::Set(Data* src, uint32_t flags)
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
bool TEMPLATE_NAME<T>::Equals(Object* object)
{
    const SimpleDataT* rhs = SafeCast<SimpleDataT>(object);
    if (!rhs)
    {
        return false;
    }

    return *rhs->m_Data == *m_Data;
}

template <class T>
void TEMPLATE_NAME<T>::Serialize(const Helium::BasicBufferPtr& buffer, const tchar_t* debugStr) const
{
    T val = *m_Data;

    Helium::Swizzle( val, buffer->GetByteOrder() != Helium::ByteOrders::LittleEndian );

    buffer->AddBuffer( (const uint8_t*)&val, sizeof(T), debugStr );
}

template <class T>
void TEMPLATE_NAME<T>::Serialize(class ArchiveBinary& archive)
{
    const T* data = m_Data;
    archive.GetStream().Write( data ); 
}

template <class T>
void TEMPLATE_NAME<T>::Deserialize(class ArchiveBinary& archive)
{
    T* data = m_Data;
    archive.GetStream().Read( data ); 
}

template <class T>
void TEMPLATE_NAME<T>::Serialize(class ArchiveXML& archive)
{
    archive.GetStream() << *m_Data;
}

template <class T>
void TEMPLATE_NAME<T>::Deserialize(class ArchiveXML& archive)
{
    archive.GetStream() >> *m_Data;
}

template <class T>
tostream& TEMPLATE_NAME<T>::operator>>(tostream& stream) const
{
    stream << *m_Data;

    return stream;
}

template <class T>
tistream& TEMPLATE_NAME<T>::operator<<(tistream& stream)
{
    stream >> *m_Data;

    if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
    {
        Object* object = static_cast< Object* >( m_Instance );
        object->RaiseChanged( m_Field );
    }

    return stream;
}

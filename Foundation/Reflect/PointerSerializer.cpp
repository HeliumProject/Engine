#include "PointerSerializer.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS( PointerSerializer );

PointerSerializer::PointerSerializer ()
: m_TypeID( ReservedTypes::Invalid )
{

}

PointerSerializer::~PointerSerializer()
{

}

void PointerSerializer::ConnectData(Nocturnal::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Nocturnal::HybridPtr<DataType> (data.Address(), data.State()) );
}

bool PointerSerializer::Set(const Reflect::Serializer* s, u32 flags)
{
    const PointerSerializer* rhs = Reflect::ConstObjectCast<PointerSerializer>(s);
    if (!rhs)
    {
        return false;
    }

    if (flags & Reflect::SerializerFlags::Shallow)
    {
        m_Data.Set( rhs->m_Data.Get() );
    }
    else
    {
        m_Data.Set( rhs->m_Data.Get().ReferencesObject() ? rhs->m_Data.Get()->Clone() : NULL );
    }

    return true;
}

bool PointerSerializer::Equals(const Reflect::Serializer* s) const
{
    const PointerSerializer* rhs = Reflect::ConstObjectCast<PointerSerializer>(s);
    if (!rhs)
    {
        return false;
    }

    // if the pointers are equal we are done
    if ( m_Data.Get() == rhs->m_Data.Get() )
    {
        return true;
    }
    // if they are not equal but one is null we are done
    else if (m_Data.Get() == NULL || rhs->m_Data.Get() == NULL)
    {
        return false;
    }

    // pointers aren't equal so we have to do deep equality test
    return m_Data.Get()->Equals( rhs->m_Data.Get() );
}

void PointerSerializer::Host(Reflect::Visitor& visitor)
{
    if (!visitor.VisitPointer(*(Reflect::ElementPtr*)(m_Data.Ptr())))
    {
        return;
    }

    if ( m_Data.Get() )
    {
        m_Data.Get()->Host( visitor );
    }
}

void PointerSerializer::Serialize(Archive& archive) const
{
    archive.Serialize(m_Data.Get());
}

void PointerSerializer::Deserialize(Archive& archive)
{
    m_Data.Set( NULL );

    archive.Deserialize(m_Data.Ref());
}
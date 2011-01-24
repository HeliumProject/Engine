#include "Foundation/Reflect/Data/PointerData.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( PointerData );

PointerData::PointerData()
{

}

PointerData::~PointerData()
{

}

void PointerData::ConnectData(Helium::HybridPtr<void> data)
{
    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

bool PointerData::Set(const Data* s, uint32_t flags)
{
    const PointerData* rhs = SafeCast<PointerData>(s);
    if (!rhs)
    {
        return false;
    }

    if (flags & DataFlags::Shallow)
    {
        *m_Data = *rhs->m_Data;
    }
    else
    {
        *m_Data = (*rhs->m_Data).ReferencesObject() ? (*rhs->m_Data)->Clone() : NULL;
    }

    return true;
}

bool PointerData::Equals(const Object* object) const
{
    const PointerData* rhs = SafeCast<PointerData>(object);
    
    if (!rhs)
    {
        return false;
    }

    // if the pointers are equal we are done
    if ( *m_Data == *rhs->m_Data )
    {
        return true;
    }
    // if they are not equal but one is null we are done
    else if ( (*m_Data).ReferencesObject() || !(*rhs->m_Data).ReferencesObject() )
    {
        return false;
    }

    // pointers aren't equal so we have to do deep equality test
    return (*m_Data)->Equals( *rhs->m_Data );
}

void PointerData::Accept(Visitor& visitor)
{
    if ( !visitor.VisitPointer( *m_Data ) )
    {
        return;
    }

    if ( *m_Data )
    {
        (*m_Data)->Accept( visitor );
    }
}

void PointerData::Serialize(Archive& archive) const
{
    archive.Serialize( *m_Data );
}

void PointerData::Deserialize(Archive& archive)
{
    *m_Data = NULL;

    archive.Deserialize( *m_Data );
}
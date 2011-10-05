#include "FoundationPch.h"
#include "Foundation/Reflect/Data/Data.h"

#include "Platform/Debug.h"
#include "Foundation/TUID.h"
#include "Foundation/Numeric.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( Data )

Data::Data()
: m_Field( NULL )
{

}

Data::~Data()
{

}

template<class srcT, class destT>
bool Cast(Data* src, Data* dest)
{
    SimpleData<srcT>* source = static_cast<SimpleData<srcT>*>(src);
    SimpleData<destT>* destination = static_cast<SimpleData<destT>*>(dest);

    return Helium::RangeCast( *source->m_Data, *destination->m_Data );
}

template<>
bool Cast<uint64_t, Helium::TUID>(Data* src, Data* dest)
{
    SimpleData<uint64_t>* source = static_cast<SimpleData<uint64_t>*>(src);
    SimpleData<Helium::TUID>* destination = static_cast<SimpleData<Helium::TUID>*>(dest);

    *destination->m_Data = *source->m_Data;

    return true;
}

template<>
bool Cast<Helium::TUID, uint64_t>(Data* src, Data* dest)
{
    SimpleData<Helium::TUID>* source = static_cast<SimpleData<Helium::TUID>*>(src);
    SimpleData<uint64_t>* destination = static_cast<SimpleData<uint64_t>*>(dest);

    *destination->m_Data = *source->m_Data;

    return true;
}

template<>
bool Cast<Helium::TUID, Helium::GUID>(Data* src, Data* dest)
{
    SimpleData<Helium::TUID>* source = static_cast<SimpleData<Helium::TUID>*>(src);
    SimpleData<Helium::GUID>* destination = static_cast<SimpleData<Helium::GUID>*>(dest);

    (*destination->m_Data).FromTUID( *source->m_Data );

    return true;
}

template<>
bool Cast<Helium::GUID, Helium::TUID>(Data* src, Data* dest)
{
    SimpleData<Helium::GUID>* source = static_cast<SimpleData<Helium::GUID>*>(src);
    SimpleData<Helium::TUID>* destination = static_cast<SimpleData<Helium::TUID>*>(dest);

    (*destination->m_Data).FromGUID( *source->m_Data );

    return true;
}

typedef std::pair<const Class*, const Class*> ClassPair;
typedef std::map<ClassPair, bool (*)(Data*, Data*)> CastingFunctionMap;

CastingFunctionMap g_CastingFunctions;

template<class S, class D>
void MapCast()
{
    ClassPair key (Reflect::GetDataClass<S>(), Reflect::GetDataClass<D>());
    CastingFunctionMap::value_type val (key , &Cast<S, D>);
    bool ins = g_CastingFunctions.insert( val ).second;
    HELIUM_ASSERT(ins);
}

template<class A, class B>
void MapCasts()
{
    MapCast<A, B>();
    MapCast<B, A>();
}

void Data::Initialize()
{
    if (!g_CastingFunctions.empty())
    {
        return;
    }

    // signed to unsigned, same size
    MapCasts<int8_t, uint8_t>();
    MapCasts<int16_t, uint16_t>();
    MapCasts<int32_t, uint32_t>();
    MapCasts<int64_t, uint64_t>();

    // signed to signed, different size / signed to unsigned, different size
    MapCasts<int8_t, int16_t>(); MapCasts<int8_t, uint16_t>();
    MapCasts<int8_t, int32_t>(); MapCasts<int8_t, uint32_t>();
    MapCasts<int8_t, int64_t>(); MapCasts<int8_t, uint64_t>();

    // unsigned to signed, different size / unsigned to unsigned, different size
    MapCasts<uint8_t, int16_t>(); MapCasts<uint8_t, uint16_t>();
    MapCasts<uint8_t, int32_t>(); MapCasts<uint8_t, uint32_t>();
    MapCasts<uint8_t, int64_t>(); MapCasts<uint8_t, uint64_t>();

    // signed to signed, different size / signed to unsigned, different size
    MapCasts<int16_t, int32_t>(); MapCasts<int16_t, uint32_t>();
    MapCasts<int16_t, int64_t>(); MapCasts<int16_t, uint64_t>();

    // unsigned to signed, different size / unsigned to unsigned, different size
    MapCasts<uint16_t, int32_t>(); MapCasts<uint16_t, uint32_t>();
    MapCasts<uint16_t, int64_t>(); MapCasts<uint16_t, uint64_t>();

    // signed to signed, different size / signed to unsigned, different size
    MapCasts<int32_t, int64_t>(); MapCasts<int32_t, uint64_t>();

    // unsigned to signed, different size / unsigned to unsigned, different size
    MapCasts<uint32_t, int64_t>(); MapCasts<uint32_t, uint64_t>();

    // 8^2 - 8 (don't cant to ourself)
    HELIUM_ASSERT(g_CastingFunctions.size() == 56);

    // float to double
    MapCasts<float32_t, float64_t>();

    // float to signed / float to unsigned
    MapCasts<float32_t, int8_t>(); MapCasts<float32_t, uint8_t>();
    MapCasts<float32_t, int16_t>(); MapCasts<float32_t, uint16_t>();
    MapCasts<float32_t, int32_t>(); MapCasts<float32_t, uint32_t>();
    MapCasts<float32_t, int64_t>(); MapCasts<float32_t, uint64_t>();

    // double to signed / double to unsigned
    MapCasts<float64_t, int8_t>(); MapCasts<float64_t, uint8_t>();
    MapCasts<float64_t, int16_t>(); MapCasts<float64_t, uint16_t>();
    MapCasts<float64_t, int32_t>(); MapCasts<float64_t, uint32_t>();
    MapCasts<float64_t, int64_t>(); MapCasts<float64_t, uint64_t>();

    // 8 * 4 (to and from float32_t and float64_t across 8 integer type)
    HELIUM_ASSERT(g_CastingFunctions.size() == 90);

    // uint64_t to TUID and back
    MapCasts<uint64_t, Helium::TUID>();

    // GUID to TUID and back
    MapCasts<Helium::GUID, Helium::TUID>();
}

void Data::Cleanup()
{
    if (g_CastingFunctions.size()>0)
    {
        g_CastingFunctions.clear();
    }
}

void Data::ConnectField( void* instance, const Field* field, uintptr_t offsetInField )
{
    m_Instance = instance;
    m_Field = field;
    ConnectData( static_cast< char* >( m_Instance ) + m_Field->m_Offset + offsetInField ); 
}

void Data::Disconnect()
{
    m_Instance = (Object*)NULL;
    m_Field = NULL;
    ConnectData( NULL );
}

bool Data::CastSupported(const Class* srcType, const Class* destType)
{
    if (srcType == destType)
    {
        return true;
    }

    CastingFunctionMap::iterator found = g_CastingFunctions.find( ClassPair (srcType, destType) );
    if (found != g_CastingFunctions.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Data::CastValue(Data* src, Data* dest, uint32_t flags)
{
    // if the types are a match, just do set value
    if (dest->IsClass(src->GetClass()) || src->IsClass(dest->GetClass()))
    {
        return dest->Set(src);
    }

    // next look for a natural casting function (non-container cast)
    CastingFunctionMap::iterator found = g_CastingFunctions.find( ClassPair (src->GetClass(), dest->GetClass()) );
    if (found != g_CastingFunctions.end())
    {
        return found->second(src, dest);
    }

    // check to see if we can do a container cast, casting the data within the container
    if (src->IsClass( Reflect::GetClass<ContainerData>()) && dest->IsClass( Reflect::GetClass<ContainerData>() ))
    {
        StlVectorData* srcArray = AssertCast<StlVectorData>( src );
        StlVectorData* destArray = AssertCast<StlVectorData>( dest );
        if ( srcArray && destArray )
        {
            if (CastSupported( srcArray->GetItemClass(), destArray->GetItemClass() ))
            {
                destArray->SetSize( srcArray->GetSize() );

                for (size_t i=0; i<srcArray->GetSize(); i++)
                {
                    Data::CastValue( srcArray->GetItem(i), destArray->GetItem(i) );
                }

                return true;
            }
        }
        else if (src->IsClass( Reflect::GetClass<StlSetData>() ) && dest->IsClass( Reflect::GetClass<StlSetData>() ))
        {
            StlSetData* srcSet = AssertCast<StlSetData>( src );
            StlSetData* destSet = AssertCast<StlSetData>( dest );
            if (CastSupported( srcSet->GetItemClass(), destSet->GetItemClass() ))
            {
                std::vector< DataPtr > data;
                srcSet->GetItems( data );

                destSet->Clear();

                std::vector< DataPtr >::const_iterator itr = data.begin();
                std::vector< DataPtr >::const_iterator end = data.end();
                for ( ; itr != end; ++itr )
                {
                    DataPtr value = AssertCast<Data>( Registry::GetInstance()->CreateInstance( destSet->GetItemClass() ) );
                    if (Data::CastValue( *itr, value ))
                    {
                        destSet->AddItem( value );
                    }
                }

                return true;
            }
        }
        else if (src->IsClass( Reflect::GetClass<StlMapData>() ) && dest->IsClass( Reflect::GetClass<StlMapData>() ))
        {
            StlMapData* srcMap = AssertCast<StlMapData>( src );
            StlMapData* destMap = AssertCast<StlMapData>( dest );
            if ( CastSupported( srcMap->GetKeyClass(), destMap->GetKeyClass() ) && CastSupported( srcMap->GetValueClass(), destMap->GetValueClass() ) )
            {
                StlMapData::V_ValueType data;
                srcMap->GetItems( data );

                destMap->Clear();

                StlMapData::V_ValueType::const_iterator itr = data.begin();
                StlMapData::V_ValueType::const_iterator end = data.end();
                for ( ; itr != end; ++itr )
                {
                    DataPtr key = AssertCast<Data>( Registry::GetInstance()->CreateInstance( destMap->GetKeyClass() ) );
                    DataPtr value = AssertCast<Data>( Registry::GetInstance()->CreateInstance( destMap->GetValueClass() ) );
                    if (Data::CastValue( itr->first, key ) && Data::CastValue( itr->second, value ))
                    {
                        destMap->SetItem( key, value );
                    }
                }

                return true;
            }
        }
        else if (src->IsClass( Reflect::GetClass<ObjectStlMapData>() ) && dest->IsClass( Reflect::GetClass<ObjectStlMapData>() ))
        {
            ObjectStlMapData* srcObjectMap = AssertCast<ObjectStlMapData>( src );
            ObjectStlMapData* destObjectMap = AssertCast<ObjectStlMapData>( dest );
            if (CastSupported( srcObjectMap->GetKeyClass(), destObjectMap->GetKeyClass() ))
            {
                ObjectStlMapData::V_ValueType data;
                srcObjectMap->GetItems( data );

                destObjectMap->Clear();

                ObjectStlMapData::V_ValueType::const_iterator itr = data.begin();
                ObjectStlMapData::V_ValueType::const_iterator end = data.end();
                for ( ; itr != end; ++itr )
                {
                    DataPtr key = AssertCast<Data>( Registry::GetInstance()->CreateInstance( destObjectMap->GetKeyClass() ) );
                    if (Data::CastValue( itr->first, key ))
                    {
                        destObjectMap->SetItem( key, flags & DataFlags::Shallow ? itr->second->Ptr() : (*itr->second)->Clone() );
                    }
                }

                return true;
            }      
        }
    }

    return false;
}

bool Data::ShouldSerialize()
{
    return true;
}

tostream& Data::operator>>(tostream& stream) const
{ 
    HELIUM_BREAK(); 
    return stream; 
}

tistream& Data::operator<<(tistream& stream)
{ 
    HELIUM_BREAK(); 
    return stream; 
}

void Data::Accept(Visitor& visitor)
{
    // by default, don't do anything as it will all have to be special cased in derived classes
}

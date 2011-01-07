#include "Foundation/Reflect/Data/Data.h"

#include "Platform/Debug.h"
#include "Foundation/TUID.h"
#include "Foundation/Numeric.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( Data )

Data::Data()
: m_Field (NULL)
{

}

Data::~Data()
{

}

template<class srcT, class destT>
bool Cast(const Data* src, Data* dest)
{
    const SimpleData<srcT>* source = static_cast<const SimpleData<srcT>*>(src);
    SimpleData<destT>* destination = static_cast<SimpleData<destT>*>(dest);

    return Helium::RangeCast( source->m_Data.Get(), destination->m_Data.Ref() );
}

template<>
bool Cast<uint64_t, Helium::TUID>(const Data* src, Data* dest)
{
    const SimpleData<uint64_t>* source = static_cast<const SimpleData<uint64_t>*>(src);
    SimpleData<Helium::TUID>* destination = static_cast<SimpleData<Helium::TUID>*>(dest);

    destination->m_Data.Set( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Helium::TUID, uint64_t>(const Data* src, Data* dest)
{
    const SimpleData<Helium::TUID>* source = static_cast<const SimpleData<Helium::TUID>*>(src);
    SimpleData<uint64_t>* destination = static_cast<SimpleData<uint64_t>*>(dest);

    destination->m_Data.Set( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Helium::TUID, Helium::GUID>(const Data* src, Data* dest)
{
    const SimpleData<Helium::TUID>* source = static_cast<const SimpleData<Helium::TUID>*>(src);
    SimpleData<Helium::GUID>* destination = static_cast<SimpleData<Helium::GUID>*>(dest);

    destination->m_Data.Ref().FromTUID( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Helium::GUID, Helium::TUID>(const Data* src, Data* dest)
{
    const SimpleData<Helium::GUID>* source = static_cast<const SimpleData<Helium::GUID>*>(src);
    SimpleData<Helium::TUID>* destination = static_cast<SimpleData<Helium::TUID>*>(dest);

    destination->m_Data.Ref().FromGUID( source->m_Data.Get() );

    return true;
}

typedef std::pair<const Class*, const Class*> ClassPair;
typedef std::map<ClassPair, bool (*)(const Data*, Data*)> CastingFunctionMap;

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

bool Data::CastValue(const Data* src, Data* dest, uint32_t flags)
{
    // if the types are a match, just do set value
    if (dest->HasType(src->GetType()) || src->HasType(dest->GetType()))
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
    if (src->HasType( Reflect::GetType<ContainerData>()) && dest->HasType( Reflect::GetType<ContainerData>() ))
    {
        if (src->HasType( Reflect::GetType<StlVectorData>() ) && dest->HasType( Reflect::GetType<StlVectorData>() ))
        {
            const StlVectorData* srcArray = DangerousCast<StlVectorData>( src );
            StlVectorData* destArray = DangerousCast<StlVectorData>( dest );

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
        else if (src->HasType( Reflect::GetType<StlSetData>() ) && dest->HasType( Reflect::GetType<StlSetData>() ))
        {
            const StlSetData* srcSet = DangerousCast<StlSetData>( src );
            StlSetData* destSet = DangerousCast<StlSetData>( dest );

            if (CastSupported( srcSet->GetItemClass(), destSet->GetItemClass() ))
            {
                std::vector< ConstDataPtr > data;
                srcSet->GetItems( data );

                destSet->Clear();

                std::vector< ConstDataPtr >::const_iterator itr = data.begin();
                std::vector< ConstDataPtr >::const_iterator end = data.end();
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
        else if (src->HasType( Reflect::GetType<StlMapData>() ) && dest->HasType( Reflect::GetType<StlMapData>() ))
        {
            const StlMapData* srcMap = DangerousCast<StlMapData>( src );
            StlMapData* destMap = DangerousCast<StlMapData>( dest );

            if ( CastSupported( srcMap->GetKeyClass(), destMap->GetKeyClass() ) && CastSupported( srcMap->GetValueClass(), destMap->GetValueClass() ) )
            {
                StlMapData::V_ConstValueType data;
                srcMap->GetItems( data );

                destMap->Clear();

                StlMapData::V_ConstValueType::const_iterator itr = data.begin();
                StlMapData::V_ConstValueType::const_iterator end = data.end();
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
        else if (src->HasType( Reflect::GetType<ObjectStlMapData>() ) && dest->HasType( Reflect::GetType<ObjectStlMapData>() ))
        {
            const ObjectStlMapData* srcObjectMap = DangerousCast<ObjectStlMapData>( src );
            ObjectStlMapData* destObjectMap = DangerousCast<ObjectStlMapData>( dest );

            if (CastSupported( srcObjectMap->GetKeyClass(), destObjectMap->GetKeyClass() ))
            {
                ObjectStlMapData::V_ConstValueType data;
                srcObjectMap->GetItems( data );

                destObjectMap->Clear();

                ObjectStlMapData::V_ConstValueType::const_iterator itr = data.begin();
                ObjectStlMapData::V_ConstValueType::const_iterator end = data.end();
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

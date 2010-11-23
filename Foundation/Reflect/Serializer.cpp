#include "Serializer.h"
#include "SerializerDeduction.h"

#include "Platform/Debug.h"
#include "Foundation/TUID.h"
#include "Foundation/Numeric.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( Serializer )

Serializer::Serializer()
: m_Field (NULL)
{

}

Serializer::~Serializer()
{

}

template<class srcT, class destT>
bool Cast(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<srcT>* source = static_cast<const SimpleSerializer<srcT>*>(src);
    SimpleSerializer<destT>* destination = static_cast<SimpleSerializer<destT>*>(dest);

    return Helium::RangeCast( source->m_Data.Get(), destination->m_Data.Ref() );
}

template<>
bool Cast<uint64_t, Helium::TUID>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<uint64_t>* source = static_cast<const SimpleSerializer<uint64_t>*>(src);
    SimpleSerializer<Helium::TUID>* destination = static_cast<SimpleSerializer<Helium::TUID>*>(dest);

    destination->m_Data.Set( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Helium::TUID, uint64_t>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<Helium::TUID>* source = static_cast<const SimpleSerializer<Helium::TUID>*>(src);
    SimpleSerializer<uint64_t>* destination = static_cast<SimpleSerializer<uint64_t>*>(dest);

    destination->m_Data.Set( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Helium::TUID, Helium::GUID>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<Helium::TUID>* source = static_cast<const SimpleSerializer<Helium::TUID>*>(src);
    SimpleSerializer<Helium::GUID>* destination = static_cast<SimpleSerializer<Helium::GUID>*>(dest);

    destination->m_Data.Ref().FromTUID( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Helium::GUID, Helium::TUID>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<Helium::GUID>* source = static_cast<const SimpleSerializer<Helium::GUID>*>(src);
    SimpleSerializer<Helium::TUID>* destination = static_cast<SimpleSerializer<Helium::TUID>*>(dest);

    destination->m_Data.Ref().FromGUID( source->m_Data.Get() );

    return true;
}

typedef std::pair<int32_t, int32_t> P_i32;
typedef std::map<P_i32, bool (*)(const Serializer*, Serializer*)> M_CastingFuncs;

M_CastingFuncs g_CastingFuncs;

template<class S, class D>
void MapCast()
{
    P_i32 key (Reflect::GetSerializer<S>(), Reflect::GetSerializer<D>());
    M_CastingFuncs::value_type val (key , &Cast<S, D>);
    bool ins = g_CastingFuncs.insert( val ).second;
    HELIUM_ASSERT(ins);
}

template<class A, class B>
void MapCasts()
{
    MapCast<A, B>();
    MapCast<B, A>();
}

void Serializer::Initialize()
{
    if (!g_CastingFuncs.empty())
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
    HELIUM_ASSERT(g_CastingFuncs.size() == 56);

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
    HELIUM_ASSERT(g_CastingFuncs.size() == 90);

    // uint64_t to TUID and back
    MapCasts<uint64_t, Helium::TUID>();

    // GUID to TUID and back
    MapCasts<Helium::GUID, Helium::TUID>();
}

void Serializer::Cleanup()
{
    if (g_CastingFuncs.size()>0)
    {
        g_CastingFuncs.clear();
    }
}

bool Serializer::CastSupported(int32_t srcType, int32_t destType)
{
    if (srcType == destType)
    {
        return true;
    }

    M_CastingFuncs::iterator found = g_CastingFuncs.find( P_i32 (srcType, destType) );
    if (found != g_CastingFuncs.end())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Serializer::CastValue(const Serializer* src, Serializer* dest, uint32_t flags)
{
    // if the types are a match, just do set value
    if (dest->HasType(src->GetType()) || src->HasType(dest->GetType()))
    {
        return dest->Set(src);
    }

    // next look for a natural casting function (non-container cast)
    M_CastingFuncs::iterator found = g_CastingFuncs.find( P_i32 (src->GetType(), dest->GetType()) );
    if (found != g_CastingFuncs.end())
    {
        return found->second(src, dest);
    }

    // check to see if we can do a container cast, casting the data within the container
    if (src->HasType( Reflect::GetType<ContainerSerializer>()) && dest->HasType( Reflect::GetType<ContainerSerializer>() ))
    {
        if (src->HasType( Reflect::GetType<ArraySerializer>() ) && dest->HasType( Reflect::GetType<ArraySerializer>() ))
        {
            const ArraySerializer* srcArray = ConstDangerousCast<ArraySerializer>( src );
            ArraySerializer* destArray = DangerousCast<ArraySerializer>( dest );

            if (CastSupported( srcArray->GetItemType(), destArray->GetItemType() ))
            {
                destArray->SetSize( srcArray->GetSize() );

                for (size_t i=0; i<srcArray->GetSize(); i++)
                {
                    Serializer::CastValue( srcArray->GetItem(i), destArray->GetItem(i) );
                }

                return true;
            }
        }
        else if (src->HasType( Reflect::GetType<SetSerializer>() ) && dest->HasType( Reflect::GetType<SetSerializer>() ))
        {
            const SetSerializer* srcSet = ConstDangerousCast<SetSerializer>( src );
            SetSerializer* destSet = DangerousCast<SetSerializer>( dest );

            if (CastSupported( srcSet->GetItemType(), destSet->GetItemType() ))
            {
                std::vector< ConstSerializerPtr > data;
                srcSet->GetItems( data );

                destSet->Clear();

                std::vector< ConstSerializerPtr >::const_iterator itr = data.begin();
                std::vector< ConstSerializerPtr >::const_iterator end = data.end();
                for ( ; itr != end; ++itr )
                {
                    SerializerPtr value = AssertCast<Serializer>( Registry::GetInstance()->CreateInstance( destSet->GetItemType() ) );
                    if (Serializer::CastValue( *itr, value ))
                    {
                        destSet->AddItem( value );
                    }
                }

                return true;
            }
        }
        else if (src->HasType( Reflect::GetType<MapSerializer>() ) && dest->HasType( Reflect::GetType<MapSerializer>() ))
        {
            const MapSerializer* srcMap = ConstDangerousCast<MapSerializer>( src );
            MapSerializer* destMap = DangerousCast<MapSerializer>( dest );

            if ( CastSupported( srcMap->GetKeyType(), destMap->GetKeyType() ) && CastSupported( srcMap->GetValueType(), destMap->GetValueType() ) )
            {
                MapSerializer::V_ConstValueType data;
                srcMap->GetItems( data );

                destMap->Clear();

                MapSerializer::V_ConstValueType::const_iterator itr = data.begin();
                MapSerializer::V_ConstValueType::const_iterator end = data.end();
                for ( ; itr != end; ++itr )
                {
                    SerializerPtr key = AssertCast<Serializer>( Registry::GetInstance()->CreateInstance( destMap->GetKeyType() ) );
                    SerializerPtr value = AssertCast<Serializer>( Registry::GetInstance()->CreateInstance( destMap->GetValueType() ) );
                    if (Serializer::CastValue( itr->first, key ) && Serializer::CastValue( itr->second, value ))
                    {
                        destMap->SetItem( key, value );
                    }
                }

                return true;
            }
        }
        else if (src->HasType( Reflect::GetType<ElementMapSerializer>() ) && dest->HasType( Reflect::GetType<ElementMapSerializer>() ))
        {
            const ElementMapSerializer* srcElementMap = ConstDangerousCast<ElementMapSerializer>( src );
            ElementMapSerializer* destElementMap = DangerousCast<ElementMapSerializer>( dest );

            if (CastSupported( srcElementMap->GetKeyType(), destElementMap->GetKeyType() ))
            {
                ElementMapSerializer::V_ConstValueType data;
                srcElementMap->GetItems( data );

                destElementMap->Clear();

                ElementMapSerializer::V_ConstValueType::const_iterator itr = data.begin();
                ElementMapSerializer::V_ConstValueType::const_iterator end = data.end();
                for ( ; itr != end; ++itr )
                {
                    SerializerPtr key = AssertCast<Serializer>( Registry::GetInstance()->CreateInstance( destElementMap->GetKeyType() ) );
                    if (Serializer::CastValue( itr->first, key ))
                    {
                        destElementMap->SetItem( key, flags & SerializerFlags::Shallow ? itr->second->Ptr() : (*itr->second)->Clone() );
                    }
                }

                return true;
            }      
        }
    }

    return false;
}

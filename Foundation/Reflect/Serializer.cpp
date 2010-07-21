#include "Serializer.h"
#include "Serializers.h"

#include "Platform/Debug.h"
#include "Foundation/TUID.h"
#include "Foundation/Numeric.h"

using namespace Reflect;

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

    return Nocturnal::RangeCast( source->m_Data.Get(), destination->m_Data.Ref() );
}

template<>
bool Cast<u64, Nocturnal::TUID>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<u64>* source = static_cast<const SimpleSerializer<u64>*>(src);
    SimpleSerializer<Nocturnal::TUID>* destination = static_cast<SimpleSerializer<Nocturnal::TUID>*>(dest);

    destination->m_Data.Set( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Nocturnal::TUID, u64>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<Nocturnal::TUID>* source = static_cast<const SimpleSerializer<Nocturnal::TUID>*>(src);
    SimpleSerializer<u64>* destination = static_cast<SimpleSerializer<u64>*>(dest);

    destination->m_Data.Set( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Nocturnal::TUID, Nocturnal::GUID>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<Nocturnal::TUID>* source = static_cast<const SimpleSerializer<Nocturnal::TUID>*>(src);
    SimpleSerializer<Nocturnal::GUID>* destination = static_cast<SimpleSerializer<Nocturnal::GUID>*>(dest);

    destination->m_Data.Ref().FromTUID( source->m_Data.Get() );

    return true;
}

template<>
bool Cast<Nocturnal::GUID, Nocturnal::TUID>(const Serializer* src, Serializer* dest)
{
    const SimpleSerializer<Nocturnal::GUID>* source = static_cast<const SimpleSerializer<Nocturnal::GUID>*>(src);
    SimpleSerializer<Nocturnal::TUID>* destination = static_cast<SimpleSerializer<Nocturnal::TUID>*>(dest);

    destination->m_Data.Ref().FromGUID( source->m_Data.Get() );

    return true;
}

typedef std::pair<i32, i32> P_i32;
typedef std::map<P_i32, bool (*)(const Serializer*, Serializer*)> M_CastingFuncs;

M_CastingFuncs g_CastingFuncs;

template<class S, class D>
void MapCast()
{
    P_i32 key (Reflect::Serializer::DeduceType<S>(), Reflect::Serializer::DeduceType<D>());
    M_CastingFuncs::value_type val (key , &Cast<S, D>);
    bool ins = g_CastingFuncs.insert( val ).second;
    NOC_ASSERT(ins);
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
    MapCasts<i8, u8>();
    MapCasts<i16, u16>();
    MapCasts<i32, u32>();
    MapCasts<i64, u64>();

    // signed to signed, different size / signed to unsigned, different size
    MapCasts<i8, i16>(); MapCasts<i8, u16>();
    MapCasts<i8, i32>(); MapCasts<i8, u32>();
    MapCasts<i8, i64>(); MapCasts<i8, u64>();

    // unsigned to signed, different size / unsigned to unsigned, different size
    MapCasts<u8, i16>(); MapCasts<u8, u16>();
    MapCasts<u8, i32>(); MapCasts<u8, u32>();
    MapCasts<u8, i64>(); MapCasts<u8, u64>();

    // signed to signed, different size / signed to unsigned, different size
    MapCasts<i16, i32>(); MapCasts<i16, u32>();
    MapCasts<i16, i64>(); MapCasts<i16, u64>();

    // unsigned to signed, different size / unsigned to unsigned, different size
    MapCasts<u16, i32>(); MapCasts<u16, u32>();
    MapCasts<u16, i64>(); MapCasts<u16, u64>();

    // signed to signed, different size / signed to unsigned, different size
    MapCasts<i32, i64>(); MapCasts<i32, u64>();

    // unsigned to signed, different size / unsigned to unsigned, different size
    MapCasts<u32, i64>(); MapCasts<u32, u64>();

    // 8^2 - 8 (don't cant to ourself)
    NOC_ASSERT(g_CastingFuncs.size() == 56);

    // float to double
    MapCasts<f32, f64>();

    // float to signed / float to unsigned
    MapCasts<f32, i8>(); MapCasts<f32, u8>();
    MapCasts<f32, i16>(); MapCasts<f32, u16>();
    MapCasts<f32, i32>(); MapCasts<f32, u32>();
    MapCasts<f32, i64>(); MapCasts<f32, u64>();

    // double to signed / double to unsigned
    MapCasts<f64, i8>(); MapCasts<f64, u8>();
    MapCasts<f64, i16>(); MapCasts<f64, u16>();
    MapCasts<f64, i32>(); MapCasts<f64, u32>();
    MapCasts<f64, i64>(); MapCasts<f64, u64>();

    // 8 * 4 (to and from f32 and f64 across 8 integer type)
    NOC_ASSERT(g_CastingFuncs.size() == 90);

    // u64 to TUID and back
    MapCasts<u64, Nocturnal::TUID>();

    // GUID to TUID and back
    MapCasts<Nocturnal::GUID, Nocturnal::TUID>();
}

void Serializer::Cleanup()
{
    if (g_CastingFuncs.size()>0)
    {
        g_CastingFuncs.clear();
    }
}

bool Serializer::CastSupported(i32 srcType, i32 destType)
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

bool Serializer::CastValue(const Serializer* src, Serializer* dest, u32 flags)
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
                V_ConstSerializer data;
                srcSet->GetItems( data );

                destSet->Clear();

                V_ConstSerializer::const_iterator itr = data.begin();
                V_ConstSerializer::const_iterator end = data.end();
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

bool Serializer::TranslateOutput( tostream& stream ) const
{
    if (m_TranslateOutput.Valid())
    {
        TranslateOutputEventArgs args ( const_cast<Serializer*>(this), stream );

        m_TranslateOutput.Invoke( args );

        return true;
    }
    else
    {
        return false;
    }
}

bool Serializer::TranslateInput( tistream& stream )
{
    if (m_TranslateInput.Valid())
    {
        TranslateInputEventArgs args ( this, stream );

        m_TranslateInput.Invoke( args );

        if ( m_Instance && m_Field && m_Field->m_Type->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = m_Instance;
            element->RaiseChanged( m_Field );
        }

        return true;
    }
    else
    {
        return false;
    }
}

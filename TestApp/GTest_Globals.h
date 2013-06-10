
#pragma once

#if GTEST

#include "Reflect/Registry.h"
#include "WindowProc.h"

#include <cfloat>
#include <ctime>

class NonTrivialClass
{
public:
    NonTrivialClass( float32_t value = 0.0f )
        : m_value( value )
    {
        HELIUM_TRACE( Helium::TraceLevels::Debug, TXT( "NonTrivialClass::NonTrivialClass()\n" ) );
    }

    NonTrivialClass( const NonTrivialClass& rSource )
        : m_value( rSource.m_value )
    {
        HELIUM_TRACE( Helium::TraceLevels::Debug, TXT( "NonTrivialClass::NonTrivialClass( const NonTrivialClass& )\n" ) );
    }

    ~NonTrivialClass()
    {
        HELIUM_TRACE( Helium::TraceLevels::Debug, TXT( "NonTrivialClass::~NonTrivialClass()\n" ) );
    }

    NonTrivialClass& operator=( const NonTrivialClass& rSource )
    {
        HELIUM_TRACE( Helium::TraceLevels::Debug, TXT( "NonTrivialClass::operator=( const NonTrivialClass& )\n" ) );
        m_value = rSource.m_value;
        return *this;
    }

    float32_t GetValue() const
    {
        return m_value;
    }

private:
    float32_t m_value;
};

class TestRunnable : public Helium::Runnable
{
public:
    TestRunnable( const Helium::String& rString )
        : m_string( rString )
    {
    }

    virtual void Run()
    {
        HELIUM_TRACE( Helium::TraceLevels::Debug, TXT( "%s\n" ), *m_string );
    }

private:
    Helium::String m_string;
};

template< size_t BufferSize >
static void GetFormattedValue( char ( &buffer )[ BufferSize ], int value )
{
    StringPrint( buffer, BufferSize, TXT( "%u" ), value );
    buffer[ BufferSize - 1 ] = TXT( '\0' );
}

template< size_t BufferSize >
static void GetFormattedValue( char ( &buffer )[ BufferSize ], const NonTrivialClass& rValue )
{
    StringPrint( buffer, BufferSize, TXT( "%f" ), rValue.GetValue() );
    buffer[ BufferSize - 1 ] = TXT( '\0' );
}

template< typename T, typename Allocator >
static void PrintArrayInfo( const char varName[], const Helium::DynamicArray< T, Allocator >& rArray )
{
    HELIUM_UNREF( varName );

    HELIUM_TRACE( TraceLevels::Debug, TXT( "%s.GetSize() = %Iu\n" ), varName, rArray.GetSize() );
    HELIUM_TRACE( TraceLevels::Debug, TXT( "%s.GetCapacity() = %Iu\n" ), varName, rArray.GetCapacity() );

    char valueBuffer[ 64 ];

    size_t arraySize = rArray.GetSize();
    for( size_t arrayIndex = 0; arrayIndex < arraySize; ++arrayIndex )
    {
        GetFormattedValue( valueBuffer, rArray[ arrayIndex ] );
        HELIUM_TRACE( TraceLevels::Debug, TXT( "%s " ), valueBuffer );
    }

    HELIUM_TRACE( TraceLevels::Debug, TXT( "\n" ) );
}

static int FloatCompare( const void* pElement0, const void* pElement1 )
{
    float32_t element0 = *static_cast< const float32_t* >( pElement0 );
    float32_t element1 = *static_cast< const float32_t* >( pElement1 );

    return ( element0 < element1 ? -1 : ( element0 > element1 ? 1 : 0 ) );
}

class StringCompareFunction
{
public:
    bool operator()( const Helium::String& rString0, const Helium::String& rString1 ) const
    {
        return ( Helium::CompareString( *rString0, *rString1 ) < 0 );
    }
};

// Globals to share between main and test cases
extern Helium::AssetLoader* gAssetLoader;

#endif
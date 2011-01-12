//----------------------------------------------------------------------------------------------------------------------
// Serializer.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef LUNAR_ENGINE_SERIALIZER_H
#define LUNAR_ENGINE_SERIALIZER_H

#include "Engine/Engine.h"

#include "Foundation/Name.h"
#include "Foundation/Memory/ReferenceCounting.h"

#include "boost/preprocessor/seq.hpp"

// Helper macro for defining serializable enumerated values.
#define L_ENUM_VALUE_OP( R, PREFIX, VALUE ) BOOST_PP_CAT( PREFIX, VALUE )
// Helper macro for defining and stringizing serializable enumerated values.
#define L_ENUM_VALUE_STRING_OP( R, PREFIX, VALUE ) HELIUM_TSTRINGIZE( BOOST_PP_CAT( PREFIX, VALUE ) )

/// @defgroup serializationmacros Serialization Support Macros
//@{

/// Tag and serialize a variable.
///
/// @param[in] X  Variable to serialize.
#define L_TAGGED( X ) Lunar::Serializer::Tag( HELIUM_TSTRINGIZE( X ) ) << ( X )

/// Tag and serialize a struct.
///
/// @param[in] X  Struct to serialize.
#define L_TAGGED_STRUCT( X ) Lunar::Serializer::Tag( HELIUM_TSTRINGIZE( X ) ) << Lunar::Serializer::WrapStruct( X )

/// Tag and serialize an array.
///
/// @param[in] X  Array to serialize.
#define L_TAGGED_ARRAY( X ) Lunar::Serializer::Tag( HELIUM_TSTRINGIZE( X ) ) << Lunar::Serializer::WrapArray( X )

/// Tag and serialize a dynamic array.
///
/// @param[in] X  Dynamic array to serialize.
#define L_TAGGED_DYNARRAY( X ) Lunar::Serializer::Tag( HELIUM_TSTRINGIZE( X ) ) << Lunar::Serializer::WrapDynArray( X )

/// Tag and serialize an array of structs.
///
/// @param[in] X  Array of structs to serialize.
#define L_TAGGED_STRUCT_ARRAY( X ) \
    Lunar::Serializer::Tag( HELIUM_TSTRINGIZE( X ) ) << Lunar::Serializer::WrapStructArray( X )

/// Tag and serialize a dynamic array of structs.
///
/// @param[in] X  Dynamic array of structs to serialize.
#define L_TAGGED_STRUCT_DYNARRAY( X ) \
    Lunar::Serializer::Tag( HELIUM_TSTRINGIZE( X ) ) << Lunar::Serializer::WrapStructDynArray( X )

/// Serialize the parent class properties within a group named after the class.
///
/// @param[in] S  Serializer instance.
#define L_SERIALIZE_SUPER( S ) \
    { \
        const GameObjectType* pSuperType = Base::GetStaticType(); \
        HELIUM_ASSERT( pSuperType ); \
        ( S ).BeginPropertyGroup( *pSuperType->GetName() ); \
        Base::Serialize( S ); \
        ( S ).EndPropertyGroup(); \
    }

/// Define a serializable enumerated type.
///
/// @param[in] NAME    Enumeration type name.
/// @param[in] PREFIX  Prefix for enumerated values (should not include a trailing underscore).
/// @param[in] VALUES  Sequence of value identifiers (prefix should be excluded).
#define L_ENUM( NAME, PREFIX, VALUES ) \
    enum NAME##_Value \
    { \
        PREFIX##_FIRST   =  0, \
        PREFIX##_INVALID = -1, \
        BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM( L_ENUM_VALUE_OP, PREFIX##_, VALUES ) ), \
        PREFIX##_MAX, \
        PREFIX##_LAST = PREFIX##_MAX - 1 \
    }; \
    \
    class NAME : public Lunar::Serializer::Enum< NAME, NAME##_Value > \
    { \
    public: \
        NAME() \
        { \
        } \
        \
        NAME( NAME##_Value value ) \
            : Lunar::Serializer::Enum< NAME, NAME##_Value >( value ) \
        { \
        } \
        \
        static const tchar_t* const* GetNames() \
        { \
            static const tchar_t* ppValueNames[] = \
            { \
                BOOST_PP_SEQ_ENUM( BOOST_PP_SEQ_TRANSFORM( L_ENUM_VALUE_STRING_OP, PREFIX##_, VALUES ) ) \
            }; \
            \
            return ppValueNames; \
        } \
        \
        static uint32_t GetCount() \
        { \
            return static_cast< uint32_t >( PREFIX##_MAX ); \
        } \
    };

//@}

namespace Helium
{
    namespace Simd
    {
        class Vector2;
        class Vector3;
        class Vector4;
        class Quat;
        class Matrix44;
        class AaBox;
    }

    union Float16;
    union Float32;

    class CharString;
    class WideString;
}

namespace Lunar
{
    class GameObjectType;

    HELIUM_DECLARE_PTR( GameObject );

    /// Struct serialization wrapper.
    template< typename T >
    class StructSerializeProxy
    {
    public:
        /// Structure being serialized.
        T& m_rStructure;

        /// @name Construction/Destruction
        //@{
        inline explicit StructSerializeProxy( T& rStructure );
        //@}

    private:
        /// @name Overloaded Operators
        //@{
        StructSerializeProxy& operator=( const StructSerializeProxy& );  // Not implemented.
        //@}
    };

    /// Array serialization wrapper.
    template< typename T, size_t Size >
    class ArraySerializeProxy
    {
    public:
        /// Array being serialized.
        T ( &m_rArray )[ Size ];

        /// @name Construction/Destruction
        //@{
        inline explicit ArraySerializeProxy( T ( &rArray )[ Size ] );
        //@}

    private:
        /// @name Overloaded Operators
        //@{
        ArraySerializeProxy& operator=( const ArraySerializeProxy& );  // Not implemented.
        //@}
    };

    /// Dynamic array serialization wrapper.
    template< typename T >
    class DynArraySerializeProxy
    {
    public:
        /// Array being serialized.
        DynArray< T >& m_rArray;

        /// @name Construction/Destruction
        //@{
        inline explicit DynArraySerializeProxy( DynArray< T >& rArray );
        //@}

    private:
        /// @name Overloaded Operators
        //@{
        DynArraySerializeProxy& operator=( const DynArraySerializeProxy& );  // Not implemented.
        //@}
    };

    /// Array of structures serialization wrapper.
    template< typename T, size_t Size >
    class StructArraySerializeProxy
    {
    public:
        /// Array being serialized.
        T ( &m_rArray )[ Size ];

        /// @name Construction/Destruction
        //@{
        inline explicit StructArraySerializeProxy( T ( &rArray )[ Size ] );
        //@}

    private:
        /// @name Overloaded Operators
        //@{
        StructArraySerializeProxy& operator=( const StructArraySerializeProxy& );  // Not implemented.
        //@}
    };

    /// Dynamic array of structures serialization wrapper.
    template< typename T >
    class StructDynArraySerializeProxy
    {
    public:
        /// Array being serialized.
        DynArray< T >& m_rArray;

        /// @name Construction/Destruction
        //@{
        inline explicit StructDynArraySerializeProxy( DynArray< T >& rArray );
        //@}

    private:
        /// @name Overloaded Operators
        //@{
        StructDynArraySerializeProxy& operator=( const StructDynArraySerializeProxy& );  // Not implemented.
        //@}
    };

    /// Base interface for GameObject serialization.
    class LUNAR_ENGINE_API Serializer : NonCopyable
    {
    public:
        /// Serialization modes.
        enum EMode
        {
            MODE_FIRST   =  0,
            MODE_INVALID = -1,

            /// Saving/writing to a buffer or resource.
            MODE_SAVE,
            /// Loading/reading from a buffer or resource.
            MODE_LOAD,
            /// Resolving object references.
            MODE_LINK,

            MODE_MAX,
            MODE_LAST = MODE_MAX - 1
        };

        /// Aggregate type identifiers.
        enum EStructTag
        {
            STRUCT_TAG_FIRST   =  0,
            STRUCT_TAG_INVALID = -1,

            /// 2-component vector.
            STRUCT_TAG_VECTOR2,
            /// 3-component vector.
            STRUCT_TAG_VECTOR3,
            /// 4-component vector.
            STRUCT_TAG_VECTOR4,
            /// Quaternion.
            STRUCT_TAG_QUAT,
            /// 4x4 matrix.
            STRUCT_TAG_MATRIX44,
            /// Axis-aligned bounding box.
            STRUCT_TAG_AA_BOX,
            /// Color (32-bit RGBA).
            STRUCT_TAG_COLOR,

            STRUCT_TAG_MAX,
            STRUCT_TAG_LAST = STRUCT_TAG_MAX - 1
        };

        /// Property flags.
        enum EFlag
        {
            FLAG_FIRST   =  0,
            FLAG_INVALID = -1,

            /// Properties should only be serialized for editor usage.
            FLAG_EDITOR_ONLY,

            FLAG_MAX,
            FLAG_LAST = FLAG_MAX - 1
        };

        /// Property tag.
        class LUNAR_ENGINE_API Tag
        {
        public:
            /// @name Construction/Destruction
            //@{
            inline explicit Tag( const tchar_t* pName );
            //@}

            /// @name Data Access
            //@{
            inline const tchar_t* Get() const;
            //@}

        private:
            /// Property name.
            const tchar_t* m_pName;
        };

        /// Enumeration interface.
        template< typename EnumInfo, typename ValueType >
        class Enum
        {
        public:
            /// Enum value type.
            typedef ValueType ValueType;
            /// Enum value type constant reference.
            typedef const ValueType& ConstValueReferenceType;

            /// @name Construction/Destruction
            //@{
            Enum();
            Enum( ValueType value );
            //@}

            /// @name Name Lookup
            //@{
            const tchar_t* const* GetNames() const;
            uint32_t GetCount() const;

            const tchar_t* GetName( ValueType value ) const;
            ValueType FindName( const tchar_t* pName ) const;
            //@}

            /// @name Overloaded Operators
            //@{
            operator ConstValueReferenceType() const;
            //@}

        private:
            /// Enum value.
            ValueType m_value;
        };

        /// @name Construction/Destruction
        //@{
        virtual ~Serializer() = 0;
        //@}

        /// @name Serialization Interface
        //@{
        virtual bool Serialize( GameObject* pObject ) = 0;
        virtual EMode GetMode() const = 0;

        virtual void SerializeTag( const Tag& rTag ) = 0;
        virtual bool CanResolveTags() const = 0;

        virtual void GetPropertyTagNames( DynArray< String >& rTagNames ) const;

        virtual void SerializeBool( bool& rValue ) = 0;
        virtual void SerializeInt8( int8_t& rValue ) = 0;
        virtual void SerializeUint8( uint8_t& rValue ) = 0;
        virtual void SerializeInt16( int16_t& rValue ) = 0;
        virtual void SerializeUint16( uint16_t& rValue ) = 0;
        virtual void SerializeInt32( int32_t& rValue ) = 0;
        virtual void SerializeUint32( uint32_t& rValue ) = 0;
        virtual void SerializeInt64( int64_t& rValue ) = 0;
        virtual void SerializeUint64( uint64_t& rValue ) = 0;
        virtual void SerializeFloat32( float32_t& rValue ) = 0;
        virtual void SerializeFloat64( float64_t& rValue ) = 0;
        virtual void SerializeBuffer( void* pBuffer, size_t elementSize, size_t count ) = 0;
        virtual void SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames ) = 0;
        virtual void SerializeCharName( CharName& rValue ) = 0;
        virtual void SerializeWideName( WideName& rValue ) = 0;
        virtual void SerializeCharString( CharString& rValue ) = 0;
        virtual void SerializeWideString( WideString& rValue ) = 0;
        virtual void SerializeObjectReference( const GameObjectType* pType, GameObjectPtr& rspObject ) = 0;

        virtual void BeginStruct( EStructTag tag = STRUCT_TAG_INVALID );
        virtual void EndStruct();

        virtual void BeginArray( uint32_t size );
        virtual void EndArray();

        virtual void BeginDynArray();
        virtual void EndDynArray();

        virtual void BeginPropertyGroup( const tchar_t* pName );
        virtual void EndPropertyGroup();

        virtual void PushPropertyFlags( uint32_t flags );
        virtual void PopPropertyFlags();
        //@}

        /// @name Aggregate Type Wrapping
        //@{
        template< typename T > static StructSerializeProxy< T > WrapStruct( T& rStruct );
        template< typename T, size_t Size > static ArraySerializeProxy< T, Size > WrapArray( T ( &rArray )[ Size ] );
        template< typename T > static DynArraySerializeProxy< T > WrapDynArray( DynArray< T >& rArray );
        template< typename T, size_t Size > static StructArraySerializeProxy< T, Size > WrapStructArray(
            T ( &rArray )[ Size ] );
        template< typename T > static StructDynArraySerializeProxy< T > WrapStructDynArray( DynArray< T >& rArray );
        //@}

        /// @name Overloaded Operators
        //@{
        inline Serializer& operator<<( const Tag& rTag );

        inline Serializer& operator<<( bool& rValue );
        inline Serializer& operator<<( int8_t& rValue );
        inline Serializer& operator<<( uint8_t& rValue );
        inline Serializer& operator<<( int16_t& rValue );
        inline Serializer& operator<<( uint16_t& rValue );
        inline Serializer& operator<<( int32_t& rValue );
        inline Serializer& operator<<( uint32_t& rValue );
        inline Serializer& operator<<( int64_t& rValue );
        inline Serializer& operator<<( uint64_t& rValue );
        inline Serializer& operator<<( float32_t& rValue );
        inline Serializer& operator<<( float64_t& rValue );
        template< typename EnumInfo, typename ValueType > Serializer& operator<<( Enum< EnumInfo, ValueType >& rValue );
        inline Serializer& operator<<( CharName& rValue );
        inline Serializer& operator<<( WideName& rValue );
        inline Serializer& operator<<( CharString& rValue );
        inline Serializer& operator<<( WideString& rValue );
        template< typename T > Serializer& operator<<( StrongPtr< T >& rspObject );

        template< typename T > Serializer& operator<<( const StructSerializeProxy< T >& rValue );
        template< typename T, size_t Size > Serializer& operator<<( const ArraySerializeProxy< T, Size >& rValue );
        template< typename T > Serializer& operator<<( const DynArraySerializeProxy< T >& rValue );
        template< typename T, size_t Size > Serializer& operator<<(
            const StructArraySerializeProxy< T, Size >& rValue );
        template< typename T > Serializer& operator<<( const StructDynArraySerializeProxy< T >& rValue );

        Serializer& operator<<( Simd::Vector2& rValue );
        Serializer& operator<<( Simd::Vector3& rValue );
        Serializer& operator<<( Simd::Vector4& rValue );
        Serializer& operator<<( Simd::Quat& rValue );
        Serializer& operator<<( Simd::Matrix44& rValue );
        Serializer& operator<<( Simd::AaBox& rValue );
        Serializer& operator<<( Float16& rValue );
        Serializer& operator<<( Float32& rValue );
        //@}
    };
}

#include "Engine/Serializer.inl"

#endif  // LUNAR_ENGINE_SERIALIZER_H

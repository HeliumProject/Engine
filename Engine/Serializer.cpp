//----------------------------------------------------------------------------------------------------------------------
// Serializer.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Serializer.h"

#include "Math/SimdVector2.h"
#include "Math/SimdVector3.h"
#include "Math/SimdVector4.h"
#include "Math/SimdQuat.h"
#include "Math/SimdMatrix44.h"
#include "Math/SimdAaBox.h"
#include "Math/Float16.h"

using namespace Helium;

/// Destructor.
Serializer::~Serializer()
{
}

/// @fn bool Serializer::Serialize( GameObject* pObject )
/// Serialize an object.
///
/// @param[in] pObject  GameObject to serialize.
///
/// @return  True if serialization was successful, false if an error occurred (will be logged).

/// @fn Serializer::EMode Serializer::GetMode() const
/// Get the serialization mode of this serializer.
///
/// @return  Serialization mode.

/// @fn void Serializer::SerializeTag( const Tag& rTag )
/// Apply a tag for an upcoming property.
///
/// @param[in] rTag  Tag to serialize.

/// @fn bool Serializer::CanResolveTags() const
/// Get whether this serializer can resolve property tags to the proper values when loading or write property tag
/// information when saving.
///
/// @return  True if this serializer makes use of property tags, false if not.
///
/// @see Tag

/// Get the list of property tag names.
///
/// Property tag names are formatted as follows:
/// - Top-level properties will contain just a name string (i.e. "m_width").
/// - Properties of structures are identified with the structure name followed by a dot, then followed by the member
///   name (i.e. "m_parameters.type").
/// - Array elements are identified with the array name followed by the element index in brackets (i.e.
///   "m_children[3]").
/// - Arrays can be nested within structs, and structs can be nested within other structs as well as arrays.  When
///   this occurs, a dot is used to separate the nested elements (i.e. "m_children[3].parameters.type", where
///   "m_children" is an array of structs, "parameters" is a struct member within the array element, and "type" is
///   a member of the "parameters" struct).
///
/// Note that this is only supported for loading serializers (where GetMode() returns MODE_LOAD) that support tag
/// resolution (where CanResolveTags() returns true).  This can also be fairly slow, so it should not be used in
/// production runtime code.
///
/// @param[out] rTagNames  List of property tag names.  The existing contents of this array will be erased.
void Serializer::GetPropertyTagNames( DynArray< String >& rTagNames ) const
{
    HELIUM_TRACE( TraceLevels::Warning, TXT( "Serializer::GetPropertyTagNames(): Called on an unsupported serializer.\n" ) );

    rTagNames.Clear();
}

/// @fn void Serializer::SerializeBool( bool& rValue )
/// Serialize a boolean value.
///
/// @param[in,out] rValue  Boolean value.

/// @fn void Serializer::SerializeInt8( int8_t& rValue )
/// Serialize a signed 8-bit integer.
///
/// @param[in,out] rValue  Signed 8-bit integer.

/// @fn void Serializer::SerializeUint8( uint8_t& rValue )
/// Serialize an unsigned 8-bit integer.
///
/// @param[in,out] rValue  Unsigned 8-bit integer.

/// @fn void Serializer::SerializeInt16( int16_t& rValue )
/// Serialize a signed 16-bit integer.
///
/// @param[in,out] rValue  Signed 16-bit integer.

/// @fn void Serializer::SerializeUint16( uint16_t& rValue )
/// Serialize an unsigned 16-bit integer.
///
/// @param[in,out] rValue  Unsigned 16-bit integer.

/// @fn void Serializer::SerializeInt32( int32_t& rValue )
/// Serialize a signed 32-bit integer.
///
/// @param[in,out] rValue  Signed 32-bit integer.

/// @fn void Serializer::SerializeUint32( uint32_t& rValue )
/// Serialize an unsigned 32-bit integer.
///
/// @param[in,out] rValue  Unsigned 32-bit integer.

/// @fn void Serializer::SerializeInt64( int64_t& rValue )
/// Serialize a signed 64-bit integer.
///
/// @param[in,out] rValue  Signed 64-bit integer.

/// @fn void Serializer::SerializeUint64( uint64_t& rValue )
/// Serialize an unsigned 64-bit integer.
///
/// @param[in,out] rValue  Unsigned 64-bit integer.

/// @fn void Serializer::SerializeFloat32( float32_t& rValue )
/// Serialize a single-precision floating-point value.
///
/// @param[in,out] rValue  Single-precision floating-point value.

/// @fn void Serializer::SerializeFloat64( float64_t& rValue )
/// Serialize a double-precision floating-point value.
///
/// @param[in,out] rValue  Double-precision floating-point value.

/// @fn void Serializer::SerializeBuffer( void* pBuffer, size_t elementSize, size_t count )
/// Serialize a buffer of data.
///
/// Note that this may not be supported by all serialization interfaces.
///
/// @param[in,out] pBuffer      Buffer to serialize.  This must point to a region of memory at least
///                             <tt>elementSize * count</tt> bytes in length.
/// @param[in]     elementSize  Size of each buffer element (this may influence byte swapping if performed).
/// @param[in]     count        Number of elements to serialize.

/// @fn void Serializer::SerializeEnum( int32_t& rValue, uint32_t nameCount, const tchar_t* const* ppNames )
/// Serialize an enumeration value.
///
/// @param[in,out] rValue     Enumeration value.
/// @param[in]     nameCount  Number of valid enumeration values, and the size of the given name array.
/// @param[in]     ppNames    Array of name strings for each valid enumeration value.

/// @fn void Serializer::SerializeEnum( int32_t& rValue, const Helium::Reflect::Enumeration* pEnumeration )
/// Serialize an enumeration value.
///
/// @param[in,out] rValue       Enumeration value.
/// @param[in]     pEnumeration Reflection information for the enum type.

/// @fn void Serializer::SerializeName( Name& rValue )
/// Serialize an 8-bit character name.
///
/// @param[in,out] rValue  Name.

/// @fn void Serializer::SerializeString( String& rValue )
/// Serialize an 8-bit character string.
///
/// @param[in,out] rValue  String.

/// @fn void Serializer::SerializeObjectReference( GameObjectType* pType, GameObjectPtr& rspObject )
/// Serialize an object reference.
///
/// @param[in]     pType      Actual object type used by the object smart pointer.
/// @param[in,out] rspObject  GameObject smart pointer.

/// Begin serializing a structure.
///
/// This is called automatically when serializing a structure using a StructSerializeProxy wrapper.  If calling this
/// function manually, EndStruct() must always be called when struct serialization is completed.
///
/// @param[in] tag  Tag specifying any special connotation for the struct about to be serialized.  If the struct has
///                 no special connotations (the default behavior), this will be STRUCT_TAG_INVALID.
///
/// @see EndStruct()
void Serializer::BeginStruct( EStructTag /*tag*/ )
{
    // Behavior defined by subclasses.
}

/// Finish serializing a structure.
///
/// This is called automatically when serializing a structure using a StructSerializeProxy wrapper.  If
/// BeginStruct() was called manually, this must always be called when struct serialization is completed.
///
/// @see BeginStruct()
void Serializer::EndStruct()
{
    // Behavior defined by subclasess.
}

/// Begin serializing a static array.
///
/// This is called automatically when serializing a static array using an ArraySerializeProxy wrapper.  If calling
/// this function manually, EndArray() must always be called when array serialization is completed.
///
/// @param[in] size  Number of elements in the array.
///
/// @see EndArray()
void Serializer::BeginArray( uint32_t /*size*/ )
{
    // Behavior defined by subclasses.
}

/// Finish serializing a static array.
///
/// This is called automatically when serializing a static array using an ArraySerializeProxy wrapper.  If
/// BeginArray() was called manually, this must always be called when array serialization is completed.
///
/// @see BeginArray()
void Serializer::EndArray()
{
    // Behavior defined by subclasess.
}

/// Begin serializing a dynamic array.
///
/// This is called automatically when serializing a dynamic array using a DynArraySerializeProxy wrapper.  If
/// calling this function manually, EndDynArray() must always be called when array serialization is completed.
///
/// @see EndDynArray()
void Serializer::BeginDynArray()
{
    // Behavior defined by subclasses.
}

/// Finish serializing a dynamic array.
///
/// This is called automatically when serializing a dynamic array using a DynArraySerializeProxy wrapper.  If
/// BeginDynArray() was called manually, this must always be called when array serialization is completed.
///
/// @see BeginDynArray()
void Serializer::EndDynArray()
{
    // Behavior defined by subclasess.
}

/// Begin a property group.
///
/// This is provided for editor usage for grouping properties together in a property window.  Outside the editor,
/// this typically has no effect.
///
/// To end a property group, call EndPropertyGroup().
///
/// Note that property groups can be nested.  If BeginPropertyGroup() is called while within other sets of calls to
/// BeginPropertyGroup() and EndPropertyGroup(), the new property group becomes active.  When EndPropertyGroup() is
/// called, the previously set group becomes active.
///
/// @param[in] pName  Group name.
///
/// @see EndPropertyGroup()
void Serializer::BeginPropertyGroup( const tchar_t* /*pName*/ )
{
    // Behavior defined by subclasess.
}

/// End a property group previously begun using BeginPropertyGroup().
///
/// @see BeginPropertyGroup()
void Serializer::EndPropertyGroup()
{
    // Behavior defined by subclasess.
}

/// Push a set of flags to apply to properties being serialized.
///
/// Property flags can be stacked.  The flags specified will be bitwise-OR'd with the existing flags.  Popping a set
/// of flags using PopPropertyFlags() will return the flag state to that prior to the corresponding flag push call.
///
/// @param[in] flags  Flags to push.
///
/// @see PopPropertyFlags()
void Serializer::PushPropertyFlags( uint32_t /*flags*/ )
{
    // Behavior defined by subclasses.
}

/// Pop a set of flags applied to properties being serialized from a previous call to PushPropertyFlags().
///
/// This will return the property flag state to that prior to the corresponding flag push call.
///
/// @see PushPropertyFlags()
void Serializer::PopPropertyFlags()
{
    // Behavior defined by subclasses.
}

/// Serialize a Vector2.
///
/// @param[in,out] rValue  Vector2 to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Simd::Vector2& rValue )
{
    BeginStruct( STRUCT_TAG_VECTOR2 );
    *this << Tag( TXT( "x" ) ) << rValue.GetX();
    *this << Tag( TXT( "y" ) ) << rValue.GetY();
    EndStruct();

    return *this;
}

/// Serialize a Vector3.
///
/// @param[in,out] rValue  Vector3 to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Simd::Vector3& rValue )
{
    BeginStruct( STRUCT_TAG_VECTOR3 );
    *this << Tag( TXT( "x" ) ) << rValue.GetElement( 0 );
    *this << Tag( TXT( "y" ) ) << rValue.GetElement( 1 );
    *this << Tag( TXT( "z" ) ) << rValue.GetElement( 2 );
    EndStruct();

    return *this;
}

/// Serialize a Vector4.
///
/// @param[in,out] rValue  Vector4 to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Simd::Vector4& rValue )
{
    BeginStruct( STRUCT_TAG_VECTOR4 );
    *this << Tag( TXT( "x" ) ) << rValue.GetElement( 0 );
    *this << Tag( TXT( "y" ) ) << rValue.GetElement( 1 );
    *this << Tag( TXT( "z" ) ) << rValue.GetElement( 2 );
    *this << Tag( TXT( "w" ) ) << rValue.GetElement( 3 );
    EndStruct();

    return *this;
}

/// Serialize a Quat.
///
/// @param[in,out] rValue  Quat to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Simd::Quat& rValue )
{
    BeginStruct( STRUCT_TAG_QUAT );
    *this << Tag( TXT( "x" ) ) << rValue.GetElement( 0 );
    *this << Tag( TXT( "y" ) ) << rValue.GetElement( 1 );
    *this << Tag( TXT( "z" ) ) << rValue.GetElement( 2 );
    *this << Tag( TXT( "w" ) ) << rValue.GetElement( 3 );
    EndStruct();

    return *this;
}

/// Serialize a Matrix44.
///
/// @param[in,out] rValue  Matrix44 to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Simd::Matrix44& rValue )
{
    BeginStruct( STRUCT_TAG_MATRIX44 );
    *this << Tag( TXT( "m00" ) ) << rValue.GetElement( 0 );
    *this << Tag( TXT( "m01" ) ) << rValue.GetElement( 1 );
    *this << Tag( TXT( "m02" ) ) << rValue.GetElement( 2 );
    *this << Tag( TXT( "m03" ) ) << rValue.GetElement( 3 );
    *this << Tag( TXT( "m10" ) ) << rValue.GetElement( 4 );
    *this << Tag( TXT( "m11" ) ) << rValue.GetElement( 5 );
    *this << Tag( TXT( "m12" ) ) << rValue.GetElement( 6 );
    *this << Tag( TXT( "m13" ) ) << rValue.GetElement( 7 );
    *this << Tag( TXT( "m20" ) ) << rValue.GetElement( 8 );
    *this << Tag( TXT( "m21" ) ) << rValue.GetElement( 9 );
    *this << Tag( TXT( "m22" ) ) << rValue.GetElement( 10 );
    *this << Tag( TXT( "m23" ) ) << rValue.GetElement( 11 );
    *this << Tag( TXT( "m30" ) ) << rValue.GetElement( 12 );
    *this << Tag( TXT( "m31" ) ) << rValue.GetElement( 13 );
    *this << Tag( TXT( "m32" ) ) << rValue.GetElement( 14 );
    *this << Tag( TXT( "m33" ) ) << rValue.GetElement( 15 );
    EndStruct();

    return *this;
}

/// Serialize an AaBox.
///
/// @param[in,out] rValue  AaBox to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Simd::AaBox& rValue )
{
    Simd::Vector3 minimum = rValue.GetMinimum();
    Simd::Vector3 maximum = rValue.GetMaximum();

    BeginStruct( STRUCT_TAG_AA_BOX );
    *this << Tag( TXT( "min" ) ) << minimum;
    *this << Tag( TXT( "max" ) ) << maximum;
    EndStruct();

    if( GetMode() == MODE_LOAD )
    {
        maximum.SetElement( 0, Max( minimum.GetElement( 0 ), maximum.GetElement( 0 ) ) );
        maximum.SetElement( 1, Max( minimum.GetElement( 1 ), maximum.GetElement( 1 ) ) );
        maximum.SetElement( 2, Max( minimum.GetElement( 2 ), maximum.GetElement( 2 ) ) );

        rValue.Set( minimum, maximum );
    }

    return *this;
}

/// Serialize a Float16.
///
/// @param[in,out] rValue  Float16 to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Float16& rValue )
{
    *this << rValue.packed;

    return *this;
}

/// Serialize a Float32.
///
/// @param[in,out] rValue  Float32 to serialize.
///
/// @return  Reference to this object.
Serializer& Serializer::operator<<( Float32& rValue )
{
    *this << rValue.packed;

    return *this;
}

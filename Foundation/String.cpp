#include "FoundationPch.h"
#include "Foundation/String.h"

using namespace Helium;

template< class T >
static int CaseSensitiveCompare( T a, T b )
{
	return a - b;
}

template< class T >
static int CaseInsensitiveCompare( T a, T b )
{
	HELIUM_COMPILE_ASSERT( 'a' > 'A' );
	const static int offset = 'a' - 'A';
	if ( a >= 'A' && a <= 'Z' )
	{
		a += offset;
	}
	if ( b >= 'A' && b <= 'Z' )
	{
		b += offset;
	}
	return a - b;
}

template< class T, int (*C)( T a, T b ) >
static int CompareStringHelper( const T* a, const T* b, size_t count )
{
	size_t lenA = StringLength( a );
	size_t lenB = StringLength( b );
	size_t min = lenA < lenB ? lenA : lenB;
	
	int result;
	const T *pA = a, *pB = b;
	for ( int i=0; i<=min && i<count; i++ ) // note: compare against the null terminator
	{
		result = C( *pA, *pB );
		if ( result != 0 )
		{
			return result;
		}
	}

	return 0;
}

int Helium::CompareString( const char* a, const char* b, size_t count )
{
	return CompareStringHelper<char, CaseSensitiveCompare>( a, b, count );
}

int Helium::CompareString( const wchar_t* a, const wchar_t* b, size_t count )
{
	return CompareStringHelper<wchar_t, CaseSensitiveCompare>( a, b, count );
}

int Helium::CaseInsensitiveCompareString( const char* a, const char* b, size_t count )
{
	return CompareStringHelper<char, CaseInsensitiveCompare>( a, b, count );
}

int Helium::CaseInsensitiveCompareString( const wchar_t* a, const wchar_t* b, size_t count )
{
	return CompareStringHelper<wchar_t, CaseInsensitiveCompare>( a, b, count );
}


/// Constructor.
///
/// This creates an empty string without allocating any memory.
CharString::CharString()
{
}

/// Constructor.
///
/// This creates a copy of a null-terminated C-style string.  The length is determined automatically based on the
/// location of the first null terminating character.
///
/// @param[in] pString  C-style string from which to copy.  This can be null.
CharString::CharString( const char* pString )
    : StringBase( pString )
{
}

/// Constructor.
///
/// This creates a copy of a C-style string with an explicit length specified.
///
/// @param[in] pString  C-style string from which to copy.  This can be null as long as the size specified is zero.
/// @param[in] size     Number of character type elements in the string, not including the null terminator.
CharString::CharString( const char* pString, size_t size )
    : StringBase( pString, size )
{
}

/// Copy constructor.
///
/// When copying, only the memory needed to hold onto the used contents of the source string will be allocated (i.e.
/// if the source string has 10 elements but a capacity of 20, only memory for the 10 used elements will be
/// allocated for this copy).
///
/// @param[in] rSource  String from which to copy.
CharString::CharString( const CharString& rSource )
    : StringBase( rSource.GetData(), rSource.GetSize() )
{
}

/// Append a character to the end of this string.
///
/// @param[in] character  Character to append.
/// @param[in] count      Number of copies of the specified character to append.
void CharString::Add( char character, size_t count )
{
    StringBase::Add( character, count );
}

/// Append the contents of a null-terminated C-style string to the end of this string.
///
/// Note that it is not safe to append a string to itself using this function.
///
/// @param[in] pString  String to append.
void CharString::Add( const char* pString, size_t length )
{
    StringBase::Add( pString, length );
}

/// Append the contents of a string to the end of this string.
///
/// @param[in] rString  String to append.
void CharString::Add( const CharString& rString )
{
    StringBase::Add( rString );
}

/// Insert copies of a character at the specified index in this string.
///
/// @param[in] index      Index at which to insert copies of the character.
/// @param[in] character  Character to insert.
/// @param[in] count      Number of copies of the character to insert.
void CharString::Insert( size_t index, char character, size_t count )
{
    StringBase::Insert( index, character, count );
}

/// Insert a copy of a null-terminated C-style string at the specified index in this string.
///
/// Note that it is not safe to insert a string to itself using this function.
///
/// @param[in] index    Index at which to insert a copy of the string.
/// @param[in] pString  String to insert.
void CharString::Insert( size_t index, const char* pString )
{
    StringBase::Insert( index, pString );
}

/// Insert a copy of a string at the specified index in this string.
///
/// Note that it is not safe to insert a string to itself using this function.
///
/// @param[in] index    Index at which to insert a copy of the string.
/// @param[in] rString  String to insert.
void CharString::Insert( size_t index, const CharString& rString )
{
    StringBase::Insert( index, rString );
}

/// Set this string to a single character.
///
/// This will always destroy the current string contents and allocate a fresh buffer whose capacity matches that
/// needed to hold onto a string of only the given character.
///
/// @param[in] character  Character to set.
///
/// @return  Reference to this string.
CharString& CharString::operator=( char character )
{
    StringBase::operator=( character );
    return *this;
}

/// Set this string to a copy of the given C-style string.
///
/// The length is determined automatically based on the location of the first null terminating character.  If the
/// given string is not the same as this string, this will always destroy the current string contents and allocate a
/// fresh buffer whose capacity matches the size of the given string.
///
/// @param[in] pString  String from which to copy.  This can be null.
///
/// @return  Reference to this string.
CharString& CharString::operator=( const char* pString )
{
    StringBase::operator=( pString );
    return *this;
}

/// Set this string to the contents of the given string.
///
/// If the given string is not the same as this string, this will always destroy the current string contents and
/// allocate a fresh buffer whose capacity matches the size of the given string.
///
/// @param[in] rSource  String from which to copy.
///
/// @return  Reference to this string.
CharString& CharString::operator=( const CharString& rSource )
{
    StringBase::operator=( rSource );
    return *this;
}

/// Append a character to the end of this string.
///
/// @param[in] character  Character to append.
///
/// @return  Reference to this string.
CharString& CharString::operator+=( char character )
{
    Add( character );
    return *this;
}

/// Append the contents of a null-terminated C-style string to the end of this string.
///
/// Note that it is not safe to append a string to itself using this function.
///
/// @param[in] pString  String to append.
CharString& CharString::operator+=( const char* pString )
{
    Add( pString );
    return *this;
}

/// Append the contents of a string to the end of this string.
///
/// @param[in] rString  String to append.
///
/// @return  Reference to this string.
CharString& CharString::operator+=( const CharString& rString )
{
    Add( rString );
    return *this;
}

/// Check whether the contents of this string match the contents of a given null-terminated C-style string.
///
/// @param[in] pString  String with which to compare.  This can be null.
///
/// @return  True if the strings match, false if not.
bool CharString::operator==( const char* pString ) const
{
    return StringBase::operator==( pString );
}

/// Check whether the contents of this string match the contents of a given string.
///
/// @param[in] rString  String with which to compare.
///
/// @return  True if the strings match, false if not.
bool CharString::operator==( const CharString& rString ) const
{
    return StringBase::operator==( rString );
}

/// Check whether the contents of this string do not match the contents of a given null-terminated C-style string.
///
/// @param[in] pString  String with which to compare.  This can be null.
///
/// @return  True if the strings do not match, false if they do.
bool CharString::operator!=( const char* pString ) const
{
    return StringBase::operator!=( pString );
}

/// Check whether the contents of this string do not match the contents of a given string.
///
/// @param[in] rString  String with which to compare.
///
/// @return  True if the strings do not match, false if they do.
bool CharString::operator!=( const CharString& rString ) const
{
    return StringBase::operator!=( rString );
}


/// Constructor.
///
/// This creates an empty string without allocating any memory.
WideString::WideString()
{
}

/// Constructor.
///
/// This creates a copy of a null-terminated C-style string.  The length is determined automatically based on the
/// location of the first null terminating character.
///
/// @param[in] pString  C-style string from which to copy.  This can be null.
WideString::WideString( const wchar_t* pString )
    : StringBase( pString )
{
}

/// Constructor.
///
/// This creates a copy of a C-style string with an explicit length specified.
///
/// @param[in] pString  C-style string from which to copy.  This can be null as long as the size specified is zero.
/// @param[in] size     Number of character type elements in the string, not including the null terminator.
WideString::WideString( const wchar_t* pString, size_t size )
    : StringBase( pString, size )
{
}

/// Copy constructor.
///
/// When copying, only the memory needed to hold onto the used contents of the source string will be allocated (i.e.
/// if the source string has 10 elements but a capacity of 20, only memory for the 10 used elements will be
/// allocated for this copy).
///
/// @param[in] rSource  String from which to copy.
WideString::WideString( const WideString& rSource )
    : StringBase( rSource.GetData(), rSource.GetSize() )
{
}

/// Append a character to the end of this string.
///
/// @param[in] character  Character to append.
/// @param[in] count      Number of copies of the specified character to append.
void WideString::Add( wchar_t character, size_t count )
{
    StringBase::Add( character, count );
}

/// Append the contents of a null-terminated C-style string to the end of this string.
///
/// Note that it is not safe to append a string to itself using this function.
///
/// @param[in] pString  String to append.
void WideString::Add( const wchar_t* pString, size_t length )
{
    StringBase::Add( pString, length );
}

/// Append the contents of a string to the end of this string.
///
/// @param[in] rString  String to append.
void WideString::Add( const WideString& rString )
{
    StringBase::Add( rString );
}

/// Insert copies of a character at the specified index in this string.
///
/// @param[in] index      Index at which to insert copies of the character.
/// @param[in] character  Character to insert.
/// @param[in] count      Number of copies of the character to insert.
void WideString::Insert( size_t index, wchar_t character, size_t count )
{
    StringBase::Insert( index, character, count );
}

/// Insert a copy of a null-terminated C-style string at the specified index in this string.
///
/// Note that it is not safe to insert a string to itself using this function.
///
/// @param[in] index    Index at which to insert a copy of the string.
/// @param[in] pString  String to insert.
void WideString::Insert( size_t index, const wchar_t* pString )
{
    StringBase::Insert( index, pString );
}

/// Insert a copy of a string at the specified index in this string.
///
/// Note that it is not safe to insert a string to itself using this function.
///
/// @param[in] index    Index at which to insert a copy of the string.
/// @param[in] rString  String to insert.
void WideString::Insert( size_t index, const WideString& rString )
{
    StringBase::Insert( index, rString );
}

/// Set this string to a single character.
///
/// This will always destroy the current string contents and allocate a fresh buffer whose capacity matches that
/// needed to hold onto a string of only the given character.
///
/// @param[in] character  Character to set.
///
/// @return  Reference to this string.
WideString& WideString::operator=( wchar_t character )
{
    StringBase::operator=( character );
    return *this;
}

/// Set this string to a copy of the given C-style string.
///
/// The length is determined automatically based on the location of the first null terminating character.  If the
/// given string is not the same as this string, this will always destroy the current string contents and allocate a
/// fresh buffer whose capacity matches the size of the given string.
///
/// @param[in] pString  String from which to copy.  This can be null.
///
/// @return  Reference to this string.
WideString& WideString::operator=( const wchar_t* pString )
{
    StringBase::operator=( pString );
    return *this;
}

/// Set this string to the contents of the given string.
///
/// If the given string is not the same as this string, this will always destroy the current string contents and
/// allocate a fresh buffer whose capacity matches the size of the given string.
///
/// @param[in] rSource  String from which to copy.
///
/// @return  Reference to this string.
WideString& WideString::operator=( const WideString& rSource )
{
    m_buffer = rSource.m_buffer;
    return *this;
}

/// Append a character to the end of this string.
///
/// @param[in] character  Character to append.
///
/// @return  Reference to this string.
WideString& WideString::operator+=( wchar_t character )
{
    Add( character );
    return *this;
}

/// Append the contents of a null-terminated C-style string to the end of this string.
///
/// Note that it is not safe to append a string to itself using this function.
///
/// @param[in] pString  String to append.
WideString& WideString::operator+=( const wchar_t* pString )
{
    Add( pString );
    return *this;
}

/// Append the contents of a string to the end of this string.
///
/// @param[in] rString  String to append.
///
/// @return  Reference to this string.
WideString& WideString::operator+=( const WideString& rString )
{
    Add( rString );
    return *this;
}

/// Check whether the contents of this string match the contents of a given null-terminated C-style string.
///
/// @param[in] pString  String with which to compare.  This can be null.
///
/// @return  True if the strings match, false if not.
bool WideString::operator==( const wchar_t* pString ) const
{
    return StringBase::operator==( pString );
}

/// Check whether the contents of this string match the contents of a given string.
///
/// @param[in] rString  String with which to compare.
///
/// @return  True if the strings match, false if not.
bool WideString::operator==( const WideString& rString ) const
{
    return StringBase::operator==( rString );
}

/// Check whether the contents of this string do not match the contents of a given null-terminated C-style string.
///
/// @param[in] pString  String with which to compare.  This can be null.
///
/// @return  True if the strings do not match, false if they do.
bool WideString::operator!=( const wchar_t* pString ) const
{
    return StringBase::operator!=( pString );
}

/// Check whether the contents of this string do not match the contents of a given string.
///
/// @param[in] rString  String with which to compare.
///
/// @return  True if the strings do not match, false if they do.
bool WideString::operator!=( const WideString& rString ) const
{
    return StringBase::operator!=( rString );
}

/// Default CharString hash.
///
/// @param[in] rKey  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Hash< CharString >::operator()( const CharString& rKey ) const
{
    return StringHash( rKey.GetData() );
}

/// Default WideString hash.
///
/// @param[in] rKey  Key for which to compute a hash value.
///
/// @return  Hash value.
size_t Hash< WideString >::operator()( const WideString& rKey ) const
{
    return StringHash( rKey.GetData() );
}

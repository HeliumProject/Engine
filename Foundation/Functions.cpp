#include "FoundationPch.h"
#include "FoundationPch.h"

#include "Foundation/Functions.h"
#include "Foundation/String.h"

using namespace Helium;

/// String "equals" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the two strings match, false if not.
bool Equals< char* >::operator()( const char* pA, const char* pB ) const
{
    return( CompareString( pA, pB ) == 0 );
}

/// Wide-character string "equals" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the two strings match, false if not.
bool Equals< wchar_t* >::operator()( const wchar_t* pA, const wchar_t* pB ) const
{
    return( CompareString( pA, pB ) == 0 );
}

/// String "equals" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the two strings match, false if not.
bool Equals< const char* >::operator()( const char* pA, const char* pB ) const
{
    return( CompareString( pA, pB ) == 0 );
}

/// Wide-character string "equals" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the two strings match, false if not.
bool Equals< const wchar_t* >::operator()( const wchar_t* pA, const wchar_t* pB ) const
{
    return( CompareString( pA, pB ) == 0 );
}

/// String "less-than" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the first string precedes the second string in basic (byte-wise) sorting, false if not.
bool Less< char* >::operator()( const char* pA, const char* pB ) const
{
    return( CompareString( pA, pB ) < 0 );
}

/// Wide-character string "less-than" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the first string precedes the second string in basic (byte-wise) sorting, false if not.
bool Less< wchar_t* >::operator()( const wchar_t* pA, const wchar_t* pB ) const
{
    return( CompareString( pA, pB ) < 0 );
}

/// String "less-than" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the first string precedes the second string in basic (byte-wise) sorting, false if not.
bool Less< const char* >::operator()( const char* pA, const char* pB ) const
{
    return( CompareString( pA, pB ) < 0 );
}

/// Wide-character string "less-than" function.
///
/// @param[in] pA  First string to compare.
/// @param[in] pB  Second string to compare.
///
/// @return  True if the first string precedes the second string in basic (byte-wise) sorting, false if not.
bool Less< const wchar_t* >::operator()( const wchar_t* pA, const wchar_t* pB ) const
{
    return( CompareString( pA, pB ) < 0 );
}

#pragma once

#include "Platform/Types.h"

#include "Foundation/API.h"

/// Comparison algorithm for strings sorted using case insensitive natural ordering.
HELIUM_FOUNDATION_API int strnatcmp(tchar_t const *a, tchar_t const *b);

/// Comparison algorithm for strings sorted using case insensitive natural ordering.
HELIUM_FOUNDATION_API int strinatcmp(tchar_t const *a, tchar_t const *b);

namespace Helium
{
	/// Comparison algorithm for strings sorted using case insensitive natural ordering.
	HELIUM_FOUNDATION_API inline int NaturalCompareString( tchar_t const* a, tchar_t const* b );

	/// Comparison algorithm for strings sorted using case insensitive natural ordering.
	HELIUM_FOUNDATION_API inline int CaseInsensitiveNaturalCompareString( tchar_t const* a, tchar_t const* b );

	/// Comparison algorithm for strings sorted using case insensitive natural ordering.
	struct HELIUM_FOUNDATION_API NaturalStringComparitor : public std::binary_function< tstring, tstring, bool >
	{
		inline bool operator()( const tstring& str1, const tstring& str2 ) const;
	};

	/// Comparison algorithm for strings sorted using case insensitive natural ordering.
	struct HELIUM_FOUNDATION_API CaseInsensitiveNaturalStringComparitor : public std::binary_function< tstring, tstring, bool >
	{
		inline bool operator()( const tstring& str1, const tstring& str2 ) const;
	};
}

#include "Foundation/Natural.inl"
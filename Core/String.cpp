//----------------------------------------------------------------------------------------------------------------------
// String.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/String.h"

namespace Lunar
{
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
    void CharString::Add( const char* pString )
    {
        StringBase::Add( pString );
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

    /// @copydoc StringBase::FindAny()
    size_t CharString::FindAny( const char* pCharacters, size_t startIndex, size_t characterCount ) const
    {
        return StringBase::FindAny( pCharacters, startIndex, characterCount );
    }

    /// Find the first instance of any of the characters in the given string, starting from the given offset.
    ///
    /// @param[in] rCharacters  String containing the characters to locate.
    /// @param[in] startIndex   Index from which to start searching.
    ///
    /// @return  Index of the first instance of any of the specified characters if found, or an invalid index if not
    ///          found.
    ///
    /// @see FindAnyReverse(), Find(), FindReverse()
    size_t CharString::FindAny( const CharString& rCharacters, size_t startIndex ) const
    {
        return StringBase::FindAny( rCharacters.m_buffer.GetData(), startIndex, rCharacters.GetSize() );
    }

    /// @copydoc StringBase::FindAnyReverse()
    size_t CharString::FindAnyReverse( const char* pCharacters, size_t startIndex, size_t characterCount ) const
    {
        return StringBase::FindAnyReverse( pCharacters, startIndex, characterCount );
    }

    /// Find the last instance of any of the characters in the given string, starting from the given offset and
    /// searching in reverse.
    ///
    /// @param[in] rCharacters  String containing the characters to locate.
    /// @param[in] startIndex   Index from which to start searching.
    ///
    /// @return  Index of the last instance of any of the specified characters if found, or an invalid index if not
    ///          found.
    ///
    /// @see FindAny(), Find(), FindReverse()
    size_t CharString::FindAnyReverse( const CharString& rCharacters, size_t startIndex ) const
    {
        return StringBase::FindAnyReverse( rCharacters.m_buffer.GetData(), startIndex, rCharacters.GetSize() );
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
    void WideString::Add( const wchar_t* pString )
    {
        StringBase::Add( pString );
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

    /// @copydoc StringBase::FindAny()
    size_t WideString::FindAny( const wchar_t* pCharacters, size_t startIndex, size_t characterCount ) const
    {
        return StringBase::FindAny( pCharacters, startIndex, characterCount );
    }

    /// Find the first instance of any of the characters in the given string, starting from the given offset.
    ///
    /// @param[in] rCharacters  String containing the characters to locate.
    /// @param[in] startIndex   Index from which to start searching.
    ///
    /// @return  Index of the first instance of any of the specified characters if found, or an invalid index if not
    ///          found.
    ///
    /// @see FindAnyReverse(), Find(), FindReverse()
    size_t WideString::FindAny( const WideString& rCharacters, size_t startIndex ) const
    {
        return StringBase::FindAny( rCharacters.m_buffer.GetData(), startIndex, rCharacters.GetSize() );
    }

    /// @copydoc StringBase::FindAnyReverse()
    size_t WideString::FindAnyReverse( const wchar_t* pCharacters, size_t startIndex, size_t characterCount ) const
    {
        return StringBase::FindAnyReverse( pCharacters, startIndex, characterCount );
    }

    /// Find the last instance of any of the characters in the given string, starting from the given offset and
    /// searching in reverse.
    ///
    /// @param[in] rCharacters  String containing the characters to locate.
    /// @param[in] startIndex   Index from which to start searching.
    ///
    /// @return  Index of the last instance of any of the specified characters if found, or an invalid index if not
    ///          found.
    ///
    /// @see FindAny(), Find(), FindReverse()
    size_t WideString::FindAnyReverse( const WideString& rCharacters, size_t startIndex ) const
    {
        return StringBase::FindAnyReverse( rCharacters.m_buffer.GetData(), startIndex, rCharacters.GetSize() );
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
}

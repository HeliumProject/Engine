/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * StrBuf.h - multipurpose buffers
 *
 * StrPtr, StrRef, and StrBuf are used throughout the system, as buffers
 * for storing just about any variable length byte data. 
 *
 * StrPtr is a low-cost (no constructor, no destructor, 8 byte) 
 * pointer/length pair to mutable data.  It has a variety of methods
 * to mangle it.
 *
 * StrRef is a kind-of StrPtr that allows the buffer pointer to be set.
 * As StrPtr doesn't allow this, a StrPtr object itself isn't useful.
 *
 * StrNum is a kind-of StrPtr with a temporary buffer whose only purpose
 * is to hold the string representation of an int.
 *
 * StrBuf is a kind-of StrPtr that allocates and extends it own buffer.
 *
 * StrFixed is a kind-of StrPtr that points to a character array that
 * is fixed at construction.
 *
 * Classes:
 *
 *	StrPtr - a pointer/length for arbitrary data
 *	StrRef - StrPtr that can be set
 *	StrBuf - StrPtr of privately allocated data
 *	StrFixed - StrPtr to a fixed length char buffer
 *	StrNum - StrPtr that holds a string of an int
 *
 * Methods:
 *
 *	StrPtr::Clear() - set length = 0
 *	StrPtr::Text() - return buffer pointer
 *	StrPtr::Value() - return buffer pointer (old name)
 *	StrPtr::Length() - return buffer length
 *	StrPtr::GetEnd() - return pointer to character past end
 *	StrPtr::Atoi() - convert to integer and return
 *	StrPtr::Atoi64() - convert to P4INT64 and return
 *	StrPtr::Itoa() - format an int given the end of a buffer
 *	StrPtr::Itoa64() - format a P4INT64 given the end of a buffer
 *	StrPtr::SetLength() - set only length
 *	StrPtr::SetEnd() - set length by calculating from start
 *	StrPtr::[] - get a single character
 *	StrPtr::XCompare() - case exact string compare
 *	StrPtr::CCompare() - case folding string compare
 *	StrPtr::SCompare() - case aware string compare -- see strbuf.cc
 *	StrPtr::SEqual() - case aware character compare -- see strbuf.cc
 *	StrPtr::Contains() - finds a substring
 *	StrPtr::== - compare contents with buffer
 *	StrPtr::!= - compare contents with buffer
 *	StrPtr::< - compare contents with buffer
 *	StrPtr::<= - compare contents with buffer
 *	StrPtr::> - compare contents with buffer
 *	StrPtr::>= - compare contents with buffer
 *	StrPtr::StrCpy() - copy string out to a buffer
 *	StrPtr::StrCat() - copy string out to end of a buffer
 *	StrPtr::CaseFolding() - (static) SCompare sorts A < a, a < B
 *	StrPtr::CaseIgnored() - (static) SCompare sorts A == a, a < B
 *	StrPtr::CaseHybrid() - (static) SCompare sorts Ax < ax, aa < AX
 *	StrPtr::SetCaseFolding() - (static) 0=UNIX, 1=NT, 2=HYBRID
 *
 *	---
 *
 *	StrRef::Set() - set pointer/length
 *	StrRef::+= - move pointer/length
 *
 *	---
 *
 *	StrBuf::StringInit() - mimic actions of constructor
 *	StrBuf::Set() - allocate and fill from buffer
 *	StrBuf::Append() - extend and terminate from buffer
 *	StrBuf::Extend() - append contents from buffer
 *	StrBuf::Terminate() - terminate buffer
 *	StrBuf::Alloc() - allocate space in buffer and return pointer
 *	StrBuf::<< - Append contents from buffer or number
 *	StrBuf::Indent() - fill by indenting contents of another buffer
 *	StrBuf::Expand() - expand a string doing %var substitutions
 *	
 */

class StrBuf;

class StrPtr {

    public:
	// Setting, getting

	char *	Text() const
		{ return buffer; }

	char *	Value() const
		{ return buffer; }

	unsigned char *UText() const
		{ return (unsigned char *)Text(); }

	size_t 	Length() const
		{ return length; }

	char *	End() const
		{ return Text() + length; }

	unsigned char *UEnd() const
		{ return UText() + length; }

	int	Atoi() const
		{ return Atoi( buffer ); }

	bool	IsNumeric() const;

	P4INT64	Atoi64() const
		{ return Atoi64( buffer ); }

	void	SetLength() 
		{ length = strlen( buffer ); }

	void	SetLength( size_t len )
		{ length = len; }

	void	SetEnd( char *p ) 
		{ length = p - buffer; }

	char	operator[]( size_t x ) const
		{ return buffer[x]; }

	// Compare -- p4ftp legacy

	int	Compare( const StrPtr &s ) const
		{ return SCompare( s ); }

	// CCompare/SCompare/XCompare

	int	CCompare( const StrPtr &s ) const
		{ return CCompare( buffer, s.buffer ); }

	int	SCompare( const StrPtr &s ) const
		{ return SCompare( buffer, s.buffer ); }

	int	NCompare( const StrPtr &s ) const
		{ return NCompare( buffer, s.buffer ); }

	static int CCompare( const char *a, const char *b );
	static int SCompare( const char *a, const char *b );
	static int NCompare( const char *a, const char *b );

	static int SCompare( unsigned char a, unsigned char b )
		{
		    return a==b ? 0 : SCompareF( a, b );
		}

	static int SEqual( unsigned char a, unsigned char b )
		{ 
		    switch( a^b ) 
		    { 
		    default: return 0;
		    case 0: return 1;
		    case 'A'^'a': return SEqualF( a, b );
		    }
		}

	int	SCompareN( const StrPtr &s ) const;

	int	XCompare( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ); }

	static int XCompare( const char *a, const char *b )
		{ return strcmp( a, b ); }

	int	XCompareN( const StrPtr &s ) const
		{ return strncmp( buffer, s.buffer, length ); }

	// More comparing

	const char *Contains( const StrPtr &s ) const
		{ return strstr( Text(), s.Text() ); }

	bool	operator ==( const char *buf ) const
		{ return strcmp( buffer, buf ) == 0; }

	bool	operator !=( const char *buf ) const
		{ return strcmp( buffer, buf ) != 0; }

	bool	operator <( const char *buf ) const
		{ return strcmp( buffer, buf ) < 0; }

	bool	operator <=( const char *buf ) const
		{ return strcmp( buffer, buf ) <= 0; }

	bool	operator >( const char *buf ) const
		{ return strcmp( buffer, buf ) > 0; }

	bool	operator >=( const char *buf ) const
		{ return strcmp( buffer, buf ) >= 0; }

	bool	operator ==( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ) == 0; }

	bool	operator !=( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ) != 0; }

	bool	operator <( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ) < 0; }

	bool	operator <=( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ) <= 0; }

	bool	operator >( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ) > 0; }

	bool	operator >=( const StrPtr &s ) const
		{ return strcmp( buffer, s.buffer ) >= 0; }

	// Copying out
	// Includes EOS

	void	StrCpy( char *b ) const
		{ memcpy( b, buffer, length + 1 ); }

	void	StrCat( char *b ) const
		{ memcpy( b + strlen( b ), buffer, length + 1 ); }

	// Formatting and parsing numbers as strings

	static int Atoi( const char *b ) { return atoi( b ); }
	static char *Itoa( int v, char *e ) { return Itoa64( v, e ); }

	static P4INT64 Atoi64( const char *buffer );
	static char *Itoa64( P4INT64 v, char *endbuf );
	static char *Itox( unsigned int v, char *endbuf );

    friend class StrBuf;
    friend class StrRef;

    protected:
	char	*buffer;
	size_t	length;

    public:

	// Case sensitive server?

	static bool CaseFolding()
		{ return caseUse != ST_UNIX; }

	static bool CaseIgnored()
		{ return caseUse == ST_WINDOWS; }

	static bool CaseHybrid()
		{ return caseUse == ST_HYBRID; }

	static void SetCaseFolding( int c )
		{ caseUse = (CaseUse)c; foldingSet = true; }

	static bool CaseFoldingAlreadySet()
		{ return foldingSet; }

	enum CaseUse { ST_UNIX, ST_WINDOWS, ST_HYBRID };

	static CaseUse CaseUsage() { return caseUse; }

    private:

	static CaseUse caseUse;
	static bool foldingSet;

	static int SEqualF( unsigned char a, unsigned char b );
	static int SCompareF( unsigned char a, unsigned char b );

	static int NCompareLeft( const unsigned char *a, 
	                         const unsigned char *b );
	static int NCompareRight( const unsigned char *a, 
	                          const unsigned char *b );
} ;

class StrRef : public StrPtr {

    public:

		StrRef() {}

		StrRef( const StrRef &s )
		{ Set( &s ); }

		StrRef( const StrPtr &s )
		{ Set( &s ); }

		StrRef( const char *buf )
		{ Set( (char *)buf ); }

		StrRef( const char *buf, size_t len )
		{ Set( (char *)buf, len ); }

	static const StrPtr &Null()
		{ return null; }

	const StrRef & operator =(const StrRef &s)
		{ Set( &s ); return *this; }

	const StrRef & operator =(const StrPtr &s)
		{ Set( &s ); return *this; }

	const StrRef & operator =(const char *buf)
		{ Set( (char *)buf ); return *this; }

	void	operator +=( int l )
		{ buffer += l; length -= l; }

	void 	Set( char *buf )
		{ Set( buf, strlen( buf ) ); }
		 
	void	Set( char *buf, size_t len )
		{ buffer = buf; length = len; }

	void	Set( const StrPtr *s )
		{ Set( s->buffer, s->length ); }

	void	Set( const StrPtr &s )
		{ Set( s.buffer, s.length ); }

    private:
    	static	StrRef null;

} ;

class StrBuf : public StrPtr {

    public:
		StrBuf() 
		{ StringInit(); }

	void	StringInit()
		{ length = size = 0; buffer = nullStrBuf; }

		~StrBuf()
		{ if( buffer != nullStrBuf ) delete []buffer; }

	// copy constructor, assignment

		StrBuf( const StrBuf &s )
		{ StringInit(); Set( &s ); }

		StrBuf( const StrRef &s )
		{ StringInit(); Set( &s ); }

		StrBuf( const StrPtr &s )
		{ StringInit(); Set( &s ); }

		StrBuf( const char *buf )
		{ StringInit(); Set( buf ); }

	const StrBuf & operator =(const StrBuf &s)
		{ if( this != &s ) Set( &s ); return *this; }

	const StrBuf & operator =(const StrRef &s)
		{ if( (const StrRef *)this != &s ) Set( &s ); return *this; }

	const StrBuf & operator =(const StrPtr &s)
		{ if( this != &s ) Set( &s ); return *this; }

	const StrBuf & operator =(const char *buf)
		{
		    if( (const char*)this != buf && buffer != buf )
			Set( buf );

		    return *this;
		}

	// Setting, getting

	void 	Clear( void )
		{ length = 0; }

	void	Set( const char *buf )
		{ Clear(); Append( buf ); }

	void	Set( const StrPtr *s )
		{ Clear(); Append( s ); }

	void	Set( const StrPtr &s )
		{ Clear(); Append( &s ); }

	void	Set( const char *buf, size_t len )
		{ Clear(); Append( buf, len ); }

	void	Extend( const char *buf, size_t len )
		{ memcpy( Alloc( len ), buf, len ); }

	void	Extend( char c )
		{ *Alloc(1) = c; }

	void 	Terminate() 
		{ Extend(0); --length; }

	void	TruncateBlanks();     // Removes blanks just from the end
	void	TrimBlanks();         // Removes blanks from start and end

	void	Append( const char *buf );     

	void	Append( const StrPtr *s );

	void	Append( const char *buf, size_t len );

	char *	Alloc( size_t len )
		{
	    size_t oldlen = length;

		    if( ( length += len ) > size )
			Grow( oldlen );

		    return buffer + oldlen;
		}

        void    Fill( const char *buf, size_t len );

        void    Fill( const char *buf )
                {
		    Fill( buf, Length() );
		}

	// string << -- append string/number

	StrBuf& operator <<( const char *s )
		{ Append( s ); return *this; }

	StrBuf& operator <<( const StrPtr *s )
		{ Append( s ); return *this; }

	StrBuf& operator <<( const StrPtr &s )
		{ Append( &s ); return *this; }

	StrBuf& operator <<( int v );

    private:
	size_t	size;

	void	Grow( size_t len );

	static char nullStrBuf[];
} ;

class StrFixed : public StrPtr {

    public:

		StrFixed( size_t l )
		{ this->length = l; this->buffer = new char[ l ]; }

		~StrFixed()
		{ delete []buffer; }
} ;


class StrNum : public StrPtr {

    public:
		StrNum() {} 

		StrNum( int v ) 
		{ Set( v ); }

		StrNum( int ok, int v )
		{ if( ok ) Set( v ); else buffer = buf, length = 0; }

	void	Set( int v )
		{
		    buffer = Itoa( v, buf + sizeof( buf ) );
		    length = buf + sizeof( buf ) - buffer - 1;
		}

	void	SetHex( int v )
		{
		    buffer = Itox( v, buf + sizeof( buf ) );
		    length = buf + sizeof( buf ) - buffer - 1;
		}

	void	Set( int v, int digits )
		{
		    Set( v );

		    while( (int)length < digits )
			*--buffer = '0', ++length;
		}

# ifdef HAVE_INT64

		StrNum( P4INT64 v )
		{ Set( v ); }

	void	Set( P4INT64 v )
		{
		    buffer = Itoa64( v, buf + sizeof( buf ) );
		    length = buf + sizeof( buf ) - buffer - 1;
		}

# endif

    private:
		char buf[24];

} ;

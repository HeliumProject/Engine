/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * StrOps.h - operations on strings
 *
 *	StrOps - member-less object with static functions
 *
 * Methods:
 *
 *	StrOps::Caps() - uppercase each character (in place) in a string
 *	StrOps::Dump() - pretty print contents, for debugging
 *	StrOps::Sub() - replace one character with another
 *	StrOps::Expand() - expand a string doing %var% substitutions
 *	StrOps::Expand2() - expand a string doing [%var%|opt] substitutions
 *	StrOps::Indent() - fill by indenting contents of another buffer
 *	StrOps::Replace() - replace all occurences of a string
 *	StrOps::Lines() - break buffer into \r\n or \n separated lines
 *	StrOps::Lower() - lowercase each character (in place) in a string
 *	StrOps::Upper() - uppercase each character (in place) in a string
 *	StrOps::Words() - break buffer into whitespace-separated words
 *	StrOps::OtoX() - turn an octet stream into hex
 *	StrOps::XtoO() - turn hex into an octet stream
 *	StrOps::WildToStr() - turn wildcards into %x escaped string
 *	StrOps::StrToWild() - turn %x escaped string into wildcards.
 *	StrOps::CompatWild() - turn %%d to %d for p4 where compatability.
 *	StrOps::ScrunchArgs() - try to display argv in a limited output buffer.
 *	StrOps::CommonPath() - construct common filepath across multiple calls.
 *	StrOps::StripNewline() - strip \r\n from end of buffer
 *
 * 	StrOps::CharCnt() - Count characters in text using the server's charset
 * 	StrOps::CharCopy() - copy counting characters (not bytes)
 *
 *	StrOps::UnpackInt() - extract an integer from front of buffer
 *	StrOps::UnpackInt64() - extract a long long from front of buffer
 *	StrOps::UnpackIntA() - extract an integer encoded in ascii
 *	StrOps::UnpackChar() - extract a char string from front of buffer
 *	StrOps::UnpackOctet() - extract a byte string from front of buffer
 *	StrOps::UnpackString() - extract a char string from front of buffer
 *	StrOps::UnpackStringA() - extract a char string with length in ascii
 *
 *	StrOps::PackInt() - append a formatted int to buffer
 *	StrOps::PackIntA() - append a formatted int to buffer in ascii
 *	StrOps::PackChar() - append a formatted string to buffer
 *	StrOps::PackOctet() - append byte array to buffer
 *	StrOps::PackString() - append a formatted string to buffer
 *	StrOps::PackStringA() - append a formatted string with len in ascii
 */

class StrPtr;
class StrRef;
class StrBuf;
class StrDict;

class StrOps {

    public:

	// Manipulating

	static void	Caps( StrBuf &o );
	static void	Dump( const StrPtr &o );
	static void	Sub( StrPtr &string, char target, char replacement );
	static void	Expand( StrBuf &o, const StrPtr &s, StrDict &d );
	static void	Expand2( StrBuf &o, const StrPtr &s, StrDict &d );
	static void	Indent( StrBuf &o, const StrPtr &s );
	static void	Replace( StrBuf &o, const StrPtr &i,
				const StrPtr &s, const StrPtr &r );
	static int 	Lines( StrBuf &o, char *vec[], int maxVec );
	static void	Lower( StrBuf &o );
	static void	Upper( StrBuf &o );
	static int	Words( StrBuf &tmp, const char *w, 
				char *vec[], int maxVec );

	static void	OtoX( const StrPtr &octet, StrBuf &hex );
	static void	XtoO( const StrPtr &hex, StrBuf &octet );
	static void	OtoX( const unsigned char *octet, int len, StrBuf &x );
	static void 	XtoO( char *x, unsigned char *octet, int octLen );

	static char	OtoX( unsigned char o )
			{ return o >= 10 ? o - 10 + 'A' : o + '0'; }

	static unsigned char XtoO( char h )
			{ return h - ( h > '9' ? ( h >= 'a' ? 'a' - 10 : 'A' - 10 ) : '0' ); }

	static int	IsDigest( const StrPtr &hex );
	static void	WildToStr( const StrPtr &i, StrBuf &o );
	static void	WildToStr( const StrPtr &i, StrBuf &o, const char *t );
	static void	StrToWild( const StrPtr &i, StrBuf &o );
	static void	WildCompat( const StrPtr &i, StrBuf &o );
	static void	MaskNonPrintable( const StrPtr &i, StrBuf &o );

	static void 	ScrunchArgs( StrBuf &out, int argc, 
				StrPtr *argv, int targetLength );

	static void     CommonPath( StrBuf &o, int &mdir, const StrPtr &n );
	static void     GetDepotName( const char *d, StrBuf &n );
	static void	StripNewline( StrBuf &o );

	// i18n

	static int	CharCnt( const StrPtr &i );
	static void	CharCopy( const StrPtr &s, StrBuf &t, int length );

	// Marshalling

	static void	PackInt( StrBuf &o, int v );
	static void	PackIntA( StrBuf &o, int v );
	static void	PackInt64( StrBuf &o, P4INT64 v );
	static void	PackChar( StrBuf &o, const char *c, int len );
	static void	PackOctet( StrBuf &o, const StrPtr &s );
	static void	PackString( StrBuf &o, const StrPtr &s );
	static void	PackStringA( StrBuf &o, const StrPtr &s );

	static int	UnpackInt( StrRef &o );
	static int	UnpackIntA( StrRef &o );
	static P4INT64	UnpackInt64( StrRef &o );
	static void	UnpackChar( StrRef &o, char *c, int length );
	static void	UnpackOctet( StrRef &o, const StrPtr &s );
	static void	UnpackString( StrRef &o, StrBuf &s );
	static void	UnpackStringA( StrRef &o, StrBuf &s );
	static void	UnpackString( StrRef &o, StrRef &s );
	static void	UnpackStringA( StrRef &o, StrRef &s );

} ;


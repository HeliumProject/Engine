/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 *
 * This file is part of Perforce - the FAST SCM System.
 */

/*
 * Options::Parse() - parse command line options
 *
 *	The "opts" string list flags.  Each (single character) flag x
 *	can be followed by an optional modifier:
 *
 *		x.	- flag takes an argument (-xarg)
 *		x:	- flag takes an argument (-xarg or -x arg)
 *		x+	- flag takes a flag and arg (-xyarg or -xy arg)
 *		x#	- flag takes a non-neg numeric arg (-xN or -x N)
 */

const int N_OPTS = 20;

enum OptFlag {
	// Bitwise selectors

	OPT_ONE = 0x01,		// exactly one
	OPT_TWO = 0x02,		// exactly two
	OPT_THREE = 0x04,	// exactly three
	OPT_MORE = 0x08,	// more than two
	OPT_NONE = 0x10,	// require none
	OPT_MAKEONE = 0x20,	// if none, make one that points to null

	// combos of the above

	OPT_OPT = 0x11,		// NONE, or ONE
	OPT_ANY = 0x1F,		// ONE, TWO, THREE, MORE, or NONE
	OPT_DEFAULT = 0x2F,	// ONE, TWO, THREE, MORE, or MAKEONE
	OPT_SOME = 0x0F		// ONE, TWO, THREE, or MORE
} ;

class Options
{
    public:
			Options() { optc = 0; }

	void		Parse( int &argc, char **&argv, const char *opts, 
		    		int flag, const ErrorId &usage, Error *e );

	void		Parse( int &argc, StrPtr *&argv, const char *opts, 
		    		int flag, const ErrorId &usage, Error *e );

	StrPtr *	operator [](char opt) 
			{ return GetValue( opt, 0, 0 ); }

	StrPtr *	GetValue( char opt, int subopt )
			{ return GetValue( opt, 0, subopt ); }

	StrPtr *	GetValue( char opt, char flag2, int subopt );

	int		FormatOption( int i, Error *e );
	int		HasOption( int i );
	void		GetOptionName( int i, StrBuf &sb );
	void		GetOptionValue( int i, StrBuf &sb );

    private:
	int 	optc;

	char	flags[ N_OPTS ];
	char	flags2[ N_OPTS ];
	StrRef	vals[ N_OPTS ];
} ;


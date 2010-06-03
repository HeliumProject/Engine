/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 */

/*
 * strarray.h - 0 based arrays of StrBufs, StrPtrs
 *
 * Class Defined:
 *
 *	StrArray - a linear list of StrBufs
 *	StrPtrArray - a linear list of StrPtrs
 *
 * Public methods:
 *
 * Private methods:
 */

class StrVarArray;

class StrArray {
	
    public:

			StrArray();
			~StrArray();

	StrBuf *	Put();
	const StrBuf *	Get(int i) const;
	StrBuf *	Edit(int i);
	int		Count() const ;
	void		Sort( int caseFolding );
	void		Remove( int i );

    private:

	StrVarArray	*array;
} ;

class StrPtrArray {

    public:
			StrPtrArray();
			~StrPtrArray();

	void		Put( const StrPtr &val );
	const StrPtr *	Get( int i ) const { return &tabVal[i]; }
	void		Clear() { tabLength = 0; }
	int		Count() const { return tabLength; }
	StrPtr *	Table() const { return tabVal; }

    private:

	// our own VarArray-like implementation
	
	StrRef		*tabVal;	
	int		tabSize;
	int		tabLength;

} ;


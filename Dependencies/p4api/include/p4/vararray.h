/*
 * Copyright 1995, 1996 Perforce Software.  All rights reserved.
 */

/*
 * VarArray.h - manage a list of void *'s
 *
 * Class Defined:
 *
 *	VarArray - list of void *'s
 *	VVarArray - VarArray with virtual Compare/Destroy functions
 *
 * Public methods:
 *
 *	VarArray::Clear() - zero out count for values
 *	VarArray::Count() - return count of elements in array
 *	VarArray::ElemTab() - return a pointer to the array
 *	VarArray::Get(i) - return a pointer to slot i in the array
 *	VarArray::Edit(i) - return a editable pointer to slot i in the array
 *	VarArray::Move(i,j) - move element i to j
 *	VarArray::Remove(i) - remove element i
 *	VarArray::Put(v) - set a new slot to v
 *	VarArray::WillGrow(i) - returns size of new vararray if it
 *	                        would have to grow in the next i rows.
 *
 *	VVarArray::Diff() - subtract/compare/add to VarArrays
 *		OpDiff - substract two VarArrays and drop duplicates
 *		OpIntersect - compare VarArrays to find matches
 *		OpMerge - add two VarArrays, eliminating duplicates
 *
 *	VVarArray::Sort() - Qsort of the VarArray in place
 *
 * Private methods:
 *
 *	VarArray::New() - return a pointer to a new slot in the array
 */

class VarArray {
	
    public:

			VarArray();
			VarArray( int max );
			~VarArray();

	void		Clear() { numElems = 0; }
	int		Count() const { return numElems; }
	void **		ElemTab() { return elems; }
	void *		Get( int i ) const { return i<numElems ? elems[i]:0; }
	void *		Edit( int i ) { return i<numElems ? elems[i]:0; }
	void 		Move( int i, int j ) { elems[j] = elems[i]; }
	void 		Remove( int i );
	void *		Put( void *v ) { return *New() = v; }
	void		SetCount( int i ) { numElems = i; }
	int		WillGrow( int interval );

	void 		Exchange( int i, int j )
	    		{ 
			    void *t = elems[j];
			    elems[j] = elems[i];
			    elems[i] = t; 
			}

	void		Swap( int i, int j ) 
			{ Exchange( i, j ); }

	void		Swap( int i, int j, int count )
			{ while( count-- > 0 ) Exchange( i++, j++ ); }

    private:
	friend class VVarArray;

	void **		New();

	int		maxElems;
	int		numElems;
	void		**elems;
} ;

class VVarArray : public VarArray {

    public:

	enum Op { OpDiff, OpIntersect, OpMerge };

	virtual		~VVarArray() {};
	virtual int	Compare( const void *, const void * ) const = 0;
	virtual void	Destroy( void * ) const = 0;

	void 		Diff( Op op, VarArray &that );
	void		Sort() { Sort( 0, Count() ); }
	void		Uniq();

    private:
	void		Sort( int l, int u );
	int		Med3( int i, int j, int k ) const;

	int		Compare( int i, int j ) const
			{ return Compare( Get( i ), Get( j ) ); }

} ;

namespace Helium
{
    /// Quick sort partition step.
    template< typename T, typename CompareFunction >
    static size_t _Partition( T* pBase, size_t count, CompareFunction& rCompare )
    {
        HELIUM_ASSERT( pBase );
        HELIUM_ASSERT( count > 2 );

        // Select the element closest to the middle of the array as our pivot (should hopefully help create a more even
        // distribution of work for each child job, particularly if the array is already sorted or nearly sorted).
        T& rPivotOriginal = pBase[ count / 2 ];
        T pivotValue = rPivotOriginal;

        T* pPivotAddress = pBase + count - 1;
        rPivotOriginal = *pPivotAddress;

        T* pStore = pBase;

        for( T* pTest = pBase; pTest < pPivotAddress; ++pTest )
        {
            if( rCompare( *pTest, pivotValue ) )
            {
                Swap( *pTest, *pStore );
                ++pStore;
                HELIUM_ASSERT( pStore <= pPivotAddress );
            }
        }

        *pPivotAddress = *pStore;
        *pStore = pivotValue;

        return static_cast< size_t >( pStore - pBase );
    }

    /// Single-threaded recursive quicksort.
    template< typename T, typename CompareFunction >
    static void _Quicksort( T* pBase, size_t count, CompareFunction& rCompare )
    {
        HELIUM_ASSERT( pBase );

        if( count == 2 )
        {
            if( rCompare( pBase[ 1 ], pBase[ 0 ] ) )
            {
                Swap( pBase[ 0 ], pBase[ 1 ] );
            }

            return;
        }

        size_t pivotIndex = _Partition( pBase, count, rCompare );
        if( pivotIndex > 1 )
        {
            _Quicksort( pBase, pivotIndex, rCompare );
        }

        size_t startIndex = pivotIndex + 1;
        HELIUM_ASSERT( startIndex <= count );
        size_t partitionSize = count - startIndex;
        if( partitionSize > 1 )
        {
            _Quicksort( pBase + startIndex, partitionSize, rCompare );
        }
    }

    /// Recursively sort an array of elements.
    ///
    /// @param[in] pContext  Context in which this job is running.
    template< typename T, typename CompareFunction >
    void SortJob< T, CompareFunction >::Run()
    {
		size_t count = m_parameters.count;
        if( count <= 1 )
        {
            return;
        }

        T* pBase = m_parameters.pBase;
        HELIUM_ASSERT( pBase );

        CompareFunction& rCompare = m_parameters.compare;
        
		_Quicksort( pBase, count, rCompare );
    }
}

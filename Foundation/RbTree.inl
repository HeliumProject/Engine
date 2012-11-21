/// Constructor.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::RbTree()
    : m_root( Invalid< size_t >() )
{
}

/// Copy constructor.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::RbTree( const RbTree& rSource )
    : m_values( rSource.m_values )
    , m_links( rSource.m_links )
    , m_blackNodes( rSource.m_blackNodes )
    , m_root( rSource.m_root )
{
}

/// Copy constructor.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
template< typename OtherAllocator >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::RbTree(
    const RbTree< Value, Key, ExtractKey, CompareKey, OtherAllocator, InternalValue >& rSource )
    : m_values( rSource.m_values )
    , m_links( rSource.m_links )
    , m_blackNodes( rSource.m_blackNodes )
    , m_root( rSource.m_root )
{
}

/// Get the number of elements in this tree.
///
/// @return  Number of elements currently in this tree.
///
/// @see GetCapacity(), IsEmpty()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::GetSize() const
{
    return m_values.GetSize();
}

/// Get whether this tree is empty.
///
/// @return  True if this tree is empty, false if not.
///
/// @see GetSize()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::IsEmpty() const
{
    return m_values.IsEmpty();
}

/// Get the maximum number of elements this tree can contain without requiring reallocation of memory.
///
/// Note that RbTree makes use of an internal BitArray, which may have a different capacity than the arrays used for
/// data storage and other tree information.
///
/// @return  Current tree capacity.
///
/// @see GetSize(), Reserve()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::GetCapacity() const
{
    return m_values.GetCapacity();
}

/// Explicitly increase the capacity of this tree to support at least the specified number of elements.
///
/// If the requested capacity is less than the current capacity, no memory will be reallocated.
///
/// @param[in] capacity  Desired capacity.
///
/// @see GetCapacity()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
void Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Reserve( size_t capacity )
{
    m_values.Reserve( capacity );
    m_links.Reserve( capacity );
    m_blackNodes.Reserve( capacity );
}

/// Resize the allocated tree memory to match the size actually in use.
///
/// @see GetCapacity()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
void Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Trim()
{
    m_values.Trim();
    m_links.Trim();
    m_blackNodes.Trim();
}

/// Clear out all elements from this tree and free all dynamically allocated memory.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
void Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Clear()
{
    m_values.Clear();
    m_links.Clear();
    m_blackNodes.Clear();

    SetInvalid( m_root );
}

/// Retrieve an iterator referencing the beginning of this tree.
///
/// @return  Iterator at the beginning of this tree.
///
/// @see End()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Begin()
{
    return Iterator( this, FindFirstNodeIndex() );
}

/// Retrieve a constant iterator referencing the beginning of this tree.
///
/// @return  Constant iterator at the beginning of this tree.
///
/// @see End()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Begin() const
{
    return ConstIterator( this, FindFirstNodeIndex() );
}

/// Retrieve an iterator referencing the end of this tree.
///
/// @return  Iterator at the end of this tree.
///
/// @see Begin()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::End()
{
    return Iterator( this, Invalid< size_t >() );
}

/// Retrieve a constant iterator referencing the end of this tree.
///
/// @return  Constant iterator at the end of this tree.
///
/// @see Begin()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::End() const
{
    return ConstIterator( this, Invalid< size_t >() );
}

/// Find a node in this tree with the specified key.
///
/// @param[in] rKey  Key for which to search.
///
/// @return  Iterator referencing the node with the specified key if found, otherwise an iterator referencing the end of
///          this tree if not found.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Find( const Key& rKey )
{
    return Iterator( this, FindNodeIndex( rKey ) );
}

/// Find a node in this tree with the specified key.
///
/// @param[in] rKey  Key for which to search.
///
/// @return  Constant iterator referencing the node with the specified key if found, otherwise a constant iiterator
///          referencing the end of this tree if not found.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Find( const Key& rKey ) const
{
    return ConstIterator( this, FindNodeIndex( rKey ) );
}

/// Attempt to insert a node with a unique key into this tree.
///
/// @param[in] rValue  Value of the node to insert.
///
/// @return  A pair containing an iterator and a boolean value.  If the node was inserted, the iterator will reference
///          the inserted node, and the boolean value will be set to true.  If a node with the same key already exists
///          in this tree, the iterator will reference the existing node, and the boolean value will be set to false.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::Pair< typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator, bool >
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Insert( const Value& rValue )
{
    Pair< Iterator, bool > result;
    result.Second() = Insert( result.First(), rValue );

    return result;
}

/// Attempt to insert a node with a unique key into this tree.
///
/// @param[out] rIterator  Iterator set to the inserted node if an existing node with the same key is not already in
///                        this tree, otherwise set to the existing node in this tree with the same key.
/// @param[in]  rValue     Value of the node to insert.
///
/// @return  True if a node with the same key as the given value did not already exist in this tree and a new node was
///          inserted, false if a node with the same key already existing in this tree.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Insert(
    ConstIterator& rIterator,
    const Value& rValue )
{
    ExtractKey keyExtract;
    CompareKey keyCompare;

    rIterator.m_pTree = this;

    const Key& rKey = keyExtract( rValue );

    // Search for an existing node with the same key in this tree.
    size_t nodeIndex = m_root;
    size_t parentNodeIndex = Invalid< size_t >();
    size_t childLinkIndex = Invalid< size_t >();
    while( IsValid( nodeIndex ) )
    {
        parentNodeIndex = nodeIndex;

        const Key& rNodeKey = keyExtract( m_values[ nodeIndex ] );
        if( keyCompare( rKey, rNodeKey ) )
        {
            nodeIndex = m_links[ nodeIndex ].children[ 0 ];
            childLinkIndex = 0;
        }
        else if( keyCompare( rNodeKey, rKey ) )
        {
            nodeIndex = m_links[ nodeIndex ].children[ 1 ];
            childLinkIndex = 1;
        }
        else
        {
            rIterator.m_index = nodeIndex;

            return false;
        }
    }

    // Existing node doesn't exist, so insert the new node.
    nodeIndex = m_values.GetSize();
    HELIUM_ASSERT( m_links.GetSize() == nodeIndex );
    HELIUM_ASSERT( m_blackNodes.GetSize() == nodeIndex );

    m_values.Push( rValue );

    LinkData* pLinkData = m_links.New();
    HELIUM_ASSERT( pLinkData );
    pLinkData->parent = parentNodeIndex;
    SetInvalid( pLinkData->children[ 0 ] );
    SetInvalid( pLinkData->children[ 1 ] );

    m_blackNodes.Push( false );

    rIterator.m_index = nodeIndex;

    if( IsInvalid( parentNodeIndex ) )
    {
        HELIUM_ASSERT( IsInvalid( m_root ) );

        m_root = nodeIndex;
        m_blackNodes[ nodeIndex ] = true;

        return true;
    }

    LinkData& rParentLinkData = m_links[ parentNodeIndex ];
    HELIUM_ASSERT( IsInvalid( rParentLinkData.children[ childLinkIndex ] ) );
    rParentLinkData.children[ childLinkIndex ] = nodeIndex;

    nodeIndex = parentNodeIndex;
    parentNodeIndex = rParentLinkData.parent;
    while( IsValid( parentNodeIndex ) )
    {
        // If the current node is black, we no longer need to correct anything, as there should be no more coloring
        // violations.
        BitArray< Allocator >::ReferenceType nodeIsBlack( m_blackNodes[ nodeIndex ] );
        if( nodeIsBlack )
        {
            break;
        }

        // Get the sibling of the current node and the direction from which we're ascending the tree to the parent node.
        const LinkData& rLinkData = m_links[ parentNodeIndex ];

        size_t childLeftIndex = rLinkData.children[ 0 ];
        size_t childRightIndex = rLinkData.children[ 1 ];

        size_t linkDirection, siblingIndex;
        if( childLeftIndex == nodeIndex )
        {
            linkDirection = 0;
            siblingIndex = childRightIndex;
        }
        else
        {
            HELIUM_ASSERT( childRightIndex == nodeIndex );
            linkDirection = 1;
            siblingIndex = childLeftIndex;
        }

        // If we have a red node with a red sibling, perform a color swap with the parent node to resolve any potential
        // red coloring violation between the current node and its children.
        bool bUpdatedNodes = false;

        if( IsValid( siblingIndex ) )
        {
            BitArray< Allocator >::ReferenceType siblingIsBlack( m_blackNodes[ siblingIndex ] );
            if( !siblingIsBlack )
            {
                // Parent must be black, since red nodes can't have red children (current node can be potentially
                // breaking this rule at this point, but its sibling can't).
                HELIUM_ASSERT( m_blackNodes[ parentNodeIndex ] );

                m_blackNodes[ parentNodeIndex ] = false;
                nodeIsBlack = true;
                siblingIsBlack = true;

                bUpdatedNodes = true;
            }
        }

        if( !bUpdatedNodes )
        {
            // The sibling is black (or doesn't exist), so perform a node rotation as necessary to resolve any potential
            // red coloring violation between the current node and its children.
            const LinkData& rNodeLinkData = m_links[ nodeIndex ];
            size_t grandChildIndex = rNodeLinkData.children[ linkDirection ];
            if( IsValid( grandChildIndex ) && !m_blackNodes[ grandChildIndex ] )
            {
                RotateNode( parentNodeIndex, linkDirection );

                // The (new) parent node is now black, so no more color violations should exist.
                break;
            }
            else
            {
                grandChildIndex = rNodeLinkData.children[ !linkDirection ];
                if( IsValid( grandChildIndex ) && !m_blackNodes[ grandChildIndex ] )
                {
                    RotateNode( nodeIndex, !linkDirection );
                    RotateNode( parentNodeIndex, linkDirection );

                    // The (new) parent node is now black, so no more color violations should exist.
                    break;
                }
            }
        }

        nodeIndex = parentNodeIndex;
        parentNodeIndex = rLinkData.parent;
    }

    // Force the root node to always be black.
    m_blackNodes[ m_root ] = true;

    return true;
}

/// Remove any entry with the specified key from this tree.
///
/// @param[in] rKey  Key to locate.
///
/// @return  True if an entry was found and removed, false if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Remove( const Key& rKey )
{
    size_t nodeIndex = FindNodeIndex( rKey );
    bool bIsValidNode = IsValid( nodeIndex );
    if( bIsValidNode )
    {
        Remove( Iterator( this, nodeIndex ) );
    }

    return bIsValidNode;
}

/// Remove the entry referenced by the specified iterator.
///
/// @param[in] iterator  Iterator for the entry to remove.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
void Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Remove( Iterator iterator )
{
    HELIUM_ASSERT( iterator.m_pTree == this );

    size_t removeNodeIndex = iterator.m_index;
    size_t nodeCount = m_values.GetSize();
    HELIUM_ASSERT( removeNodeIndex < nodeCount );

    // Special case: handle removal of the last node in the tree.
    if( nodeCount == 1 )
    {
        HELIUM_ASSERT( removeNodeIndex == 0 );
        HELIUM_ASSERT( m_root == removeNodeIndex );

        m_values.Remove( 0 );
        m_links.Remove( 0 );
        m_blackNodes.Pop();
        HELIUM_ASSERT( m_values.IsEmpty() );
        HELIUM_ASSERT( m_links.IsEmpty() );
        HELIUM_ASSERT( m_blackNodes.IsEmpty() );

        SetInvalid( m_root );

        return;
    }

    // If the node has two children, swap it with its immediate predecessor and try to remove the node from the old
    // predecessor location.
    LinkData& rRemoveNodeLinkData = m_links[ removeNodeIndex ];
    size_t parentNodeIndex = rRemoveNodeLinkData.parent;
    size_t child0Index = rRemoveNodeLinkData.children[ 0 ];
    size_t child1Index = rRemoveNodeLinkData.children[ 1 ];
    if( IsValid( child0Index ) && IsValid( child1Index ) )
    {
        size_t predecessorChildIndex = child0Index;
        size_t predecessorIndex;
        do
        {
            predecessorIndex = predecessorChildIndex;
            predecessorChildIndex = m_links[ predecessorChildIndex ].children[ 1 ];
        } while( IsValid( predecessorChildIndex ) );

        LinkData& rPredecessorLinkData = m_links[ predecessorIndex ];
        size_t predecessorParentIndex = rPredecessorLinkData.parent;
        size_t predecessorChild0Index = rPredecessorLinkData.children[ 0 ];
        size_t predecessorChild1Index = rPredecessorLinkData.children[ 1 ];

        HELIUM_ASSERT( IsValid( predecessorParentIndex ) );
        LinkData& rPredecessorParentLinkData = m_links[ predecessorParentIndex ];
        size_t predecessorParentLinkDirection = ( rPredecessorParentLinkData.children[ 1 ] == predecessorIndex );
        HELIUM_ASSERT( rPredecessorParentLinkData.children[ predecessorParentLinkDirection ] == predecessorIndex );
        rPredecessorParentLinkData.children[ predecessorParentLinkDirection ] = removeNodeIndex;

        if( IsValid( predecessorChild0Index ) )
        {
            HELIUM_ASSERT( m_links[ predecessorChild0Index ].parent == predecessorIndex );
            m_links[ predecessorChild0Index ].parent = removeNodeIndex;
        }

        if( IsValid( predecessorChild1Index ) )
        {
            HELIUM_ASSERT( m_links[ predecessorChild1Index ].parent == predecessorIndex );
            m_links[ predecessorChild1Index ].parent = removeNodeIndex;
        }

        if( IsValid( parentNodeIndex ) )
        {
            HELIUM_ASSERT( m_root != removeNodeIndex );

            LinkData& rParentLinkData = m_links[ parentNodeIndex ];
            size_t parentLinkDirection = ( rParentLinkData.children[ 1 ] == removeNodeIndex );
            HELIUM_ASSERT( rParentLinkData.children[ parentLinkDirection ] == removeNodeIndex );
            rParentLinkData.children[ parentLinkDirection ] = predecessorIndex;
        }
        else
        {
            HELIUM_ASSERT( m_root == removeNodeIndex );
            m_root = predecessorIndex;
        }

        HELIUM_ASSERT( m_links[ child0Index ].parent == removeNodeIndex );
        m_links[ child0Index ].parent = predecessorIndex;

        HELIUM_ASSERT( m_links[ child1Index ].parent == removeNodeIndex );
        m_links[ child1Index ].parent = predecessorIndex;

        // We may have just remapped indices in the link structures we're about to swap (as is the case if the immediate
        // predecessor of the node we want to remove is a child of said node), so re-fetch the node link data from the
        // link structures.
        parentNodeIndex = rPredecessorLinkData.parent;
        child0Index = rPredecessorLinkData.children[ 0 ];
        child1Index = rPredecessorLinkData.children[ 1 ];

        rPredecessorLinkData = rRemoveNodeLinkData;

        rRemoveNodeLinkData.parent = parentNodeIndex;
        rRemoveNodeLinkData.children[ 0 ] = child0Index;
        rRemoveNodeLinkData.children[ 1 ] = child1Index;

        BitArray< DefaultAllocator >::ReferenceType removeNodeIsBlack( m_blackNodes[ removeNodeIndex ] );
        BitArray< DefaultAllocator >::ReferenceType predecessorIsBlack( m_blackNodes[ predecessorIndex ] );

        bool bBlack = removeNodeIsBlack;
        removeNodeIsBlack = static_cast< bool >( predecessorIsBlack );
        predecessorIsBlack = bBlack;
    }

    HELIUM_ASSERT( IsInvalid( child0Index ) || IsInvalid( child1Index ) );

    // If attempting to delete a black node, try to rebalance the tree to make the node we want to delete red.
    if( m_blackNodes[ removeNodeIndex ] )
    {
        size_t nodeIndex = removeNodeIndex;
        while( IsValid( parentNodeIndex ) )
        {
            LinkData& rParentLinkData = m_links[ parentNodeIndex ];
            size_t parentLinkDirection = ( rParentLinkData.children[ 1 ] == nodeIndex );
            HELIUM_ASSERT( rParentLinkData.children[ parentLinkDirection ] == nodeIndex );

            size_t siblingNodeIndex = rParentLinkData.children[ !parentLinkDirection ];
            HELIUM_ASSERT( IsValid( siblingNodeIndex ) );
            if( !m_blackNodes[ siblingNodeIndex ] )
            {
                RotateNode( parentNodeIndex, !parentLinkDirection );

                HELIUM_ASSERT( rParentLinkData.children[ parentLinkDirection ] == nodeIndex );
                siblingNodeIndex = rParentLinkData.children[ !parentLinkDirection ];
                HELIUM_ASSERT( IsValid( siblingNodeIndex ) );
            }

            {
                BitArray< Allocator >::ReferenceType siblingIsBlack( m_blackNodes[ siblingNodeIndex ] );
                if( !siblingIsBlack )
                {
                    break;
                }

                LinkData& rSiblingLinkData = m_links[ siblingNodeIndex ];
                size_t siblingChildIndex = rSiblingLinkData.children[ parentLinkDirection ];
                if( IsInvalid( siblingChildIndex ) || m_blackNodes[ siblingChildIndex ] )
                {
                    siblingChildIndex = rSiblingLinkData.children[ !parentLinkDirection ];
                    if( IsInvalid( siblingChildIndex ) || m_blackNodes[ siblingChildIndex ] )
                    {
                        siblingIsBlack = false;

                        BitArray< Allocator >::ReferenceType parentIsBlack( m_blackNodes[ parentNodeIndex ] );
                        if( !parentIsBlack )
                        {
                            parentIsBlack = true;

                            break;
                        }

                        nodeIndex = parentNodeIndex;
                        parentNodeIndex = rParentLinkData.parent;

                        continue;
                    }
                }
                else
                {
                    siblingChildIndex = rSiblingLinkData.children[ !parentLinkDirection ];
                    if( IsInvalid( siblingChildIndex ) || m_blackNodes[ siblingChildIndex ] )
                    {
                        siblingNodeIndex = RotateNode( siblingNodeIndex, parentLinkDirection );
                        HELIUM_ASSERT( IsValid( siblingNodeIndex ) );
                    }
                }
            }

            BitArray< Allocator >::ReferenceType parentIsBlack( m_blackNodes[ parentNodeIndex ] );
            bool bParentIsBlackState = parentIsBlack;

            LinkData& rSiblingLinkData = m_links[ siblingNodeIndex ];
            size_t siblingChildIndex = rSiblingLinkData.children[ !parentLinkDirection ];
            HELIUM_ASSERT( IsValid( siblingChildIndex ) );
            HELIUM_ASSERT( !m_blackNodes[ siblingChildIndex ] );

            RotateNode( parentNodeIndex, !parentLinkDirection );

            m_blackNodes[ parentNodeIndex ] = true;
            m_blackNodes[ siblingChildIndex ] = true;
            m_blackNodes[ siblingNodeIndex ] = bParentIsBlackState;

            break;
        }
    }

    // Remove the node from the tree.
    HELIUM_ASSERT( rRemoveNodeLinkData.children[ 0 ] == child0Index );
    HELIUM_ASSERT( rRemoveNodeLinkData.children[ 1 ] == child1Index );

    size_t childNodeIndex = child0Index;
    if( IsInvalid( childNodeIndex ) )
    {
        childNodeIndex = child1Index;
    }

    parentNodeIndex = rRemoveNodeLinkData.parent;
    if( IsValid( parentNodeIndex ) )
    {
        HELIUM_ASSERT( m_root != removeNodeIndex );

        LinkData& rParentLinkData = m_links[ parentNodeIndex ];
        size_t parentLinkDirection = ( rParentLinkData.children[ 1 ] == removeNodeIndex );
        rParentLinkData.children[ parentLinkDirection ] = childNodeIndex;
    }
    else
    {
        HELIUM_ASSERT( m_root == removeNodeIndex );
        m_root = childNodeIndex;
    }

    if( IsValid( childNodeIndex ) )
    {
        m_links[ childNodeIndex ].parent = parentNodeIndex;
    }

    // Update indices referencing the node that will be replacing the node we want to remove (the node from the end of
    // each internal array will be swapped into the array slots occupied by the node we are removing).
    size_t replacementNodeIndex = nodeCount - 1;

    if( removeNodeIndex != replacementNodeIndex )
    {
        LinkData& rReplacementLinkData = m_links[ replacementNodeIndex ];

        parentNodeIndex = rReplacementLinkData.parent;
        if( IsValid( parentNodeIndex ) )
        {
            HELIUM_ASSERT( m_root != replacementNodeIndex );

            LinkData& rParentLinkData = m_links[ parentNodeIndex ];
            size_t parentLinkDirection = ( rParentLinkData.children[ 1 ] == replacementNodeIndex );
            HELIUM_ASSERT( rParentLinkData.children[ parentLinkDirection ] == replacementNodeIndex );
            rParentLinkData.children[ parentLinkDirection ] = removeNodeIndex;
        }
        else
        {
            HELIUM_ASSERT( m_root == replacementNodeIndex );
            m_root = removeNodeIndex;
        }

        child0Index = rReplacementLinkData.children[ 0 ];
        if( IsValid( child0Index ) )
        {
            LinkData& rChildLinkData = m_links[ child0Index ];
            HELIUM_ASSERT( rChildLinkData.parent == replacementNodeIndex );
            rChildLinkData.parent = removeNodeIndex;
        }

        child1Index = rReplacementLinkData.children[ 1 ];
        if( IsValid( child1Index ) )
        {
            LinkData& rChildLinkData = m_links[ child1Index ];
            HELIUM_ASSERT( rChildLinkData.parent == replacementNodeIndex );
            rChildLinkData.parent = removeNodeIndex;
        }
    }

    // Remove the node from each internal array, swapping in the replacement node from the ends of each array.
    m_values.RemoveSwap( removeNodeIndex );
    m_links.RemoveSwap( removeNodeIndex );

    m_blackNodes[ removeNodeIndex ] = static_cast< bool >( m_blackNodes[ replacementNodeIndex ] );
    m_blackNodes.Pop();

    // Enforce the root node color to be black.
    m_blackNodes[ m_root ] = true;
}

/// Swap the contents of this tree with another tree.
///
/// @param[in] rTree  Tree with which to swap.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
void Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Swap( RbTree& rTree )
{
    m_values.Swap( rTree.m_values );
    m_links.Swap( rTree.m_links );
    m_blackNodes.Swap( rTree.m_blackNodes );
    Swap( m_root, rTree.m_root );
}

/// Check this tree for validity.
///
/// The following tests are performed:
/// - The direct children of a node are sorted properly with relation to their parent (left child's key should precede
///   the parent's key, and the right child's key should succeed the parent's key).
/// - Child nodes reference their parent.
/// - All node indices are valid.
/// - No red node has an immediate child node that is red.
/// - The depth of black nodes is consistent.
///
/// Tree verification is provided for debugging purposes.  Verifying a tree is slow and should not be performed during
/// game runtime in a release build.
///
/// @return  True if this tree is valid, false if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Verify() const
{
    if( !IsValid( m_root ) )
    {
        return true;
    }

    size_t blackCount = RecursiveVerify( m_root );

    return IsValid( blackCount );
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >&
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::operator=( const RbTree& rSource )
{
    if( this != &rSource )
    {
        Copy( rSource );
    }

    return *this;
}

/// Assignment operator.
///
/// @param[in] rSource  Source object from which to copy.
///
/// @return  Reference to this object.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
template< typename OtherAllocator >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >&
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::operator=(
        const RbTree< Value, Key, ExtractKey, CompareKey, OtherAllocator, InternalValue >& rSource )
{
    if( this != &rSource )
    {
        Copy( rSource );
    }

    return *this;
}

/// Copy the contents of another object into this object.
///
/// @param[in] rSource  Source object from which to copy.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
template< typename OtherAllocator >
void Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Copy(
    const RbTree< Value, Key, ExtractKey, CompareKey, OtherAllocator, InternalValue >& rSource )
{
    m_values = rSource.m_values;
    m_links = rSource.m_links;
    m_blackNodes = rSource.m_blackNodes;

    m_root = rSource.m_root;
}

/// Find the node in this tree with the given key.
///
/// @param[in] rKey  Key to locate.
///
/// @return  Index of the node with the given key.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::FindNodeIndex(
    const Key& rKey ) const
{
    ExtractKey keyExtract;
    CompareKey keyCompare;

    size_t nodeIndex = m_root;
    while( IsValid( nodeIndex ) )
    {
        const Key& rNodeKey = keyExtract( m_values[ nodeIndex ] );
        if( keyCompare( rKey, rNodeKey ) )
        {
            nodeIndex = m_links[ nodeIndex ].children[ 0 ];
        }
        else if( keyCompare( rNodeKey, rKey ) )
        {
            nodeIndex = m_links[ nodeIndex ].children[ 1 ];
        }
        else
        {
            return nodeIndex;
        }
    }

    return Invalid< size_t >();
}

/// Retrieve the index of the node in this tree with the lowest sort order.
///
/// @return  Index of the lowest-sorted node in this tree.
///
/// @see FindLastNodeIndex()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::FindFirstNodeIndex() const
{
    size_t childIndex = m_root;
    size_t nodeIndex = Invalid< size_t >();
    while( IsValid( childIndex ) )
    {
        nodeIndex = childIndex;
        childIndex = m_links[ childIndex ].children[ 0 ];
    }

    return nodeIndex;
}

/// Retrieve the index of the node in this tree with the highest sort order.
///
/// @return  Index of the highest-sorted node in this tree.
///
/// @see FindLastNodeIndex()
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::FindLastNodeIndex() const
{
    size_t childIndex = m_root;
    size_t nodeIndex = Invalid< size_t >();
    while( IsValid( childIndex ) )
    {
        nodeIndex = childIndex;
        childIndex = m_links[ childIndex ].children[ 1 ];
    }

    return nodeIndex;
}

/// Rotate a node in this tree with one of its children.
///
/// @param[in] nodeIndex       Index of the node to rotate.
/// @param[in] childLinkIndex  Link index of the child node with which to rotate (0 or 1).
///
/// @return  Index of the rotated child node.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::RotateNode(
    size_t nodeIndex,
    size_t childLinkIndex )
{
    HELIUM_ASSERT( childLinkIndex <= 1 );

    LinkData& rNodeLinkData = m_links[ nodeIndex ];
    size_t parentNodeIndex = rNodeLinkData.parent;
    size_t childNodeIndex = rNodeLinkData.children[ childLinkIndex ];
    HELIUM_ASSERT( IsValid( childNodeIndex ) );

    LinkData& rChildLinkData = m_links[ childNodeIndex ];
    size_t grandChildNodeIndex = rChildLinkData.children[ !childLinkIndex ];

    rNodeLinkData.parent = childNodeIndex;
    rNodeLinkData.children[ childLinkIndex ] = grandChildNodeIndex;

    rChildLinkData.parent = parentNodeIndex;
    rChildLinkData.children[ !childLinkIndex ] = nodeIndex;

    m_blackNodes[ nodeIndex ] = false;
    m_blackNodes[ childNodeIndex ] = true;

    if( IsValid( parentNodeIndex ) )
    {
        LinkData& rParentLinkData = m_links[ parentNodeIndex ];
        if( rParentLinkData.children[ 0 ] == nodeIndex )
        {
            rParentLinkData.children[ 0 ] = childNodeIndex;
        }
        else
        {
            HELIUM_ASSERT( rParentLinkData.children[ 1 ] == nodeIndex );
            rParentLinkData.children[ 1 ] = childNodeIndex;
        }
    }
    else
    {
        HELIUM_ASSERT( m_root == nodeIndex );
        m_root = childNodeIndex;
    }

    if( IsValid( grandChildNodeIndex ) )
    {
        HELIUM_ASSERT( m_links[ grandChildNodeIndex ].parent == childNodeIndex );
        m_links[ grandChildNodeIndex ].parent = nodeIndex;
    }

    return childNodeIndex;
}

/// Verify that the current node is valid, recursively verifying the children as well.
///
/// @param[in] nodeIndex  Index of the current node.
///
/// @return  Number of black child nodes in the current tree branch (for black node height verification), invalid index
///          if an error was encountered..
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
size_t Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::RecursiveVerify(
    size_t nodeIndex ) const
{
    // Verify that the node index is valid.
    if( nodeIndex >= m_values.GetSize() )
    {
        HELIUM_TRACE(
            TraceLevels::Debug,
            TXT( "Invalid RbTree node index %" ) TPRIuSZ TXT( " (max: %" ) TPRIuSZ TXT( ").\n" ),
            nodeIndex,
            m_values.GetSize() );

        return Invalid< size_t >();
    }

    const LinkData& rLinkData = m_links[ nodeIndex ];
    size_t child0Index = rLinkData.children[ 0 ];
    size_t child1Index = rLinkData.children[ 1 ];

    bool bIsBlack = m_blackNodes[ nodeIndex ];

    // If this node is red, make sure none of the child nodes are red.
    if( !m_blackNodes[ nodeIndex ] )
    {
        if( IsValid( child0Index ) && !m_blackNodes[ child0Index ] )
        {
            HELIUM_TRACE(
                TraceLevels::Debug,
                TXT( "Consecutive red nodes found in RbTree (nodes %" ) TPRIuSZ TXT( " and %" ) TPRIuSZ TXT( ").\n" ),
                nodeIndex,
                child0Index );

            return Invalid< size_t >();
        }

        if( IsValid( child1Index ) && !m_blackNodes[ child1Index ] )
        {
            HELIUM_TRACE(
                TraceLevels::Debug,
                TXT( "Consecutive red nodes found in RbTree (nodes %" ) TPRIuSZ TXT( " and %" ) TPRIuSZ TXT( ").\n" ),
                nodeIndex,
                child1Index );

            return Invalid< size_t >();
        }
    }

    // Make sure the child nodes are sorted properly.
    ExtractKey keyExtract;
    CompareKey keyCompare;
    const Key& rNodeKey = keyExtract( m_values[ nodeIndex ] );

    if( IsValid( child0Index ) )
    {
        const Key& rChildKey = keyExtract( m_values[ child0Index ] );
        if( !keyCompare( rChildKey, rNodeKey ) )
        {
            HELIUM_TRACE(
                TraceLevels::Debug,
                TXT( "RbTree sort mismatch between node %" ) TPRIuSZ TXT( " and child node %" ) TPRIuSZ TXT( ".\n" ),
                nodeIndex,
                child0Index );

            return Invalid< size_t >();
        }
    }

    if( IsValid( child1Index ) )
    {
        const Key& rChildKey = keyExtract( m_values[ child1Index ] );
        if( !keyCompare( rNodeKey, rChildKey ) )
        {
            HELIUM_TRACE(
                TraceLevels::Debug,
                TXT( "RbTree sort mismatch between node %" ) TPRIuSZ TXT( " and child node %" ) TPRIuSZ TXT( ".\n" ),
                nodeIndex,
                child1Index );

            return Invalid< size_t >();
        }
    }

    // Recursively verify the child nodes.
    size_t child0BlackNodeCount = 0;
    if( IsValid( child0Index ) )
    {
        child0BlackNodeCount = RecursiveVerify( child0Index );
        if( IsInvalid( child0BlackNodeCount ) )
        {
            return Invalid< size_t >();
        }
    }

    size_t child1BlackNodeCount = 0;
    if( IsValid( child1Index ) )
    {
        child1BlackNodeCount = RecursiveVerify( child1Index );
        if( IsInvalid( child1BlackNodeCount ) )
        {
            return Invalid< size_t >();
        }
    }

    // Verify the black node depth matches between each subtree.
    if( child0BlackNodeCount != child1BlackNodeCount )
    {
        HELIUM_TRACE(
            TraceLevels::Debug,
            ( TXT( "RbTree black node depth mismatch between subtrees at nodes %" ) TPRIuSZ TXT( " (%" ) TPRIuSZ
              TXT( " black nodes) and %" ) TPRIuSZ TXT( " (%" ) TPRIuSZ TXT( " black nodes).\n" ) ),
            child0Index,
            child0BlackNodeCount,
            child1Index,
            child1BlackNodeCount );

        return Invalid< size_t >();
    }

    // Subtree is valid.
    return ( bIsBlack ? child0BlackNodeCount + 1 : child0BlackNodeCount );
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::ConstIterator()
{
}

/// Constructor.
///
/// @param[in] pTree  Tree to iterate.
/// @param[in] index  Node index at which to start iterating.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::ConstIterator(
    const RbTree* pTree,
    size_t index )
    : m_pTree( const_cast< RbTree* >( pTree ) )
    , m_index( index )
{
}

/// Access the current tree entry.
///
/// @return  Constant reference to the current tree entry.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
const Value& Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator*() const
{
    HELIUM_ASSERT( m_pTree );
    HELIUM_ASSERT( m_index < m_pTree->m_values.GetSize() );

    return m_pTree->m_values[ m_index ];
}

/// Access the current tree entry.
///
/// @return  Constant pointer to the current tree entry.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
const Value* Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator->() const
{
    HELIUM_ASSERT( m_pTree );
    HELIUM_ASSERT( m_index < m_pTree->m_values.GetSize() );

    return &m_pTree->m_values[ m_index ];
}

/// Increment this iterator to the next tree entry.
///
/// @return  Reference to this iterator.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator&
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator++()
{
    HELIUM_ASSERT( m_pTree );

    const DynamicArray< RbTree::LinkData, Allocator >& rLinks = m_pTree->m_links;
    size_t nodeIndex = m_index;
    HELIUM_ASSERT( nodeIndex < rLinks.GetSize() );

    const RbTree::LinkData& rLinkData = rLinks[ nodeIndex ];
    size_t childNodeIndex = rLinkData.children[ 1 ];
    if( IsValid( childNodeIndex ) )
    {
        size_t nodeIndex;
        do
        {
            nodeIndex = childNodeIndex;
            childNodeIndex = rLinks[ childNodeIndex ].children[ 0 ];
        } while( IsValid( childNodeIndex ) );

        m_index = nodeIndex;

        return *this;
    }

    size_t parentNodeIndex = rLinkData.parent;
    if( IsInvalid( parentNodeIndex ) )
    {
        SetInvalid( m_index );

        return *this;
    }

    const RbTree::LinkData* pParentLinkData = &rLinks[ parentNodeIndex ];
    size_t child1NodeIndex = pParentLinkData->children[ 1 ];
    while( child1NodeIndex == nodeIndex )
    {
        nodeIndex = parentNodeIndex;
        parentNodeIndex = pParentLinkData->parent;
        if( IsInvalid( parentNodeIndex ) )
        {
            SetInvalid( m_index );

            return *this;
        }

        pParentLinkData = &rLinks[ parentNodeIndex ];
        child1NodeIndex = pParentLinkData->children[ 1 ];
    }

    HELIUM_ASSERT( pParentLinkData->children[ 0 ] == nodeIndex );

    m_index = parentNodeIndex;

    return *this;
}

/// Post-increment this iterator to the next tree entry.
///
/// @return  Copy of this iterator at the location prior to incrementing.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator++( int )
{
    ConstIterator iterator = *this;
    ++( *this );

    return iterator;
}

/// Decrement this iterator to the previous tree entry.
///
/// @return  Reference to this iterator.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator&
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator--()
{
    HELIUM_ASSERT( m_pTree );

    // Allow decrementing from the End() iterator (the node index will be invalid in such cases).
    size_t nodeIndex = m_index;
    if( IsInvalid( nodeIndex ) )
    {
        m_index = m_pTree->FindLastNodeIndex();

        return *this;
    }

    const DynamicArray< RbTree::LinkData, Allocator >& rLinks = m_pTree->m_links;
    HELIUM_ASSERT( nodeIndex < rLinks.GetSize() );

    const RbTree::LinkData& rLinkData = rLinks[ nodeIndex ];
    size_t childNodeIndex = rLinkData.children[ 0 ];
    if( IsValid( childNodeIndex ) )
    {
        size_t nodeIndex;
        do
        {
            nodeIndex = childNodeIndex;
            childNodeIndex = rLinks[ childNodeIndex ].children[ 1 ];
        } while( IsValid( childNodeIndex ) );

        m_index = nodeIndex;

        return *this;
    }

    size_t parentNodeIndex = rLinkData.parent;
    if( IsInvalid( parentNodeIndex ) )
    {
        SetInvalid( m_index );

        return *this;
    }

    const RbTree::LinkData* pParentLinkData = &rLinks[ parentNodeIndex ];
    size_t child0NodeIndex = pParentLinkData->children[ 0 ];
    while( child0NodeIndex == nodeIndex )
    {
        nodeIndex = parentNodeIndex;
        parentNodeIndex = pParentLinkData->parent;
        if( IsInvalid( parentNodeIndex ) )
        {
            SetInvalid( m_index );

            return *this;
        }

        pParentLinkData = &rLinks[ parentNodeIndex ];
        child0NodeIndex = pParentLinkData->children[ 0 ];
    }

    HELIUM_ASSERT( pParentLinkData->children[ 1 ] == nodeIndex );

    m_index = parentNodeIndex;

    return *this;
}

/// Post-decrement this iterator to the previous tree entry.
///
/// @return  Copy of this iterator at the location prior to decrementing.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator--( int )
{
    ConstIterator iterator = *this;
    --( *this );

    return iterator;
}

/// Get whether this iterator references the same tree location as another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references the same tree location as the given iterator, false if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator==(
    const ConstIterator& rOther ) const
{
    return ( m_pTree == rOther.m_pTree && m_index == rOther.m_index );
}

/// Get whether this iterator does not reference the same tree location as another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator does not reference the same tree location as the given iterator, false if they do
///          match.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator!=(
    const ConstIterator& rOther ) const
{
    return ( m_pTree != rOther.m_pTree || m_index != rOther.m_index );
}

/// Get whether this iterator references a tree location that precedes that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a tree location that precedes that of the given iterator, false if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator<(
    const ConstIterator& rOther ) const
{
    // The index value will be invalid if an iterator is set to RbTree::End(), so treat such cases as always being
    // greater than any other valid location.
    size_t thisIndex = m_index;
    if( IsInvalid( thisIndex ) )
    {
        return false;
    }

    size_t otherIndex = rOther.m_index;
    if( IsInvalid( otherIndex ) )
    {
        return true;
    }

    // If the iterators reference the same location, then we know they're equal.
    if( thisIndex == otherIndex )
    {
        return false;
    }

    // Compare the keys of each value.
    ExtractKey keyExtract;
    CompareKey keyCompare;

    HELIUM_ASSERT( m_pTree );
    HELIUM_ASSERT( rOther.m_pTree );

    return keyCompare( keyExtract( m_pTree->m_values[ thisIndex ], rOther.m_pTree->m_values[ otherIndex ] ) );
}

/// Get whether this iterator references a tree location that succeeds that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a tree location that succeeds that of the given iterator, false if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator>(
    const ConstIterator& rOther ) const
{
    // The index value will be invalid if an iterator is set to RbTree::End(), so treat such cases as always being
    // greater than any other valid location.
    size_t otherIndex = rOther.m_index;
    if( IsInvalid( otherIndex ) )
    {
        return false;
    }

    size_t thisIndex = m_index;
    if( IsInvalid( thisIndex ) )
    {
        return true;
    }

    // If the iterators reference the same location, then we know they're equal.
    if( thisIndex == otherIndex )
    {
        return false;
    }

    // Compare the keys of each value.
    ExtractKey keyExtract;
    CompareKey keyCompare;

    HELIUM_ASSERT( m_pTree );
    HELIUM_ASSERT( rOther.m_pTree );

    return keyCompare( keyExtract( rOther.m_pTree->m_values[ otherIndex ], m_pTree->m_values[ thisIndex ] ) );
}

/// Get whether this iterator references a tree location that matches or precedes that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a tree location that matches or precedes that of the given iterator, false
///          if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator<=(
    const ConstIterator& rOther ) const
{
    return !( ( *this ) > rOther );
}

/// Get whether this iterator references a tree location that matches or succeeds that of another iterator.
///
/// @param[in] rOther  Iterator against which to compare.
///
/// @return  True if this iterator references a tree location that matches or succeeds that of the given iterator, false
///          if not.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
bool Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::ConstIterator::operator>=(
    const ConstIterator& rOther ) const
{
    return !( ( *this ) < rOther );
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::Iterator()
{
}

/// Constructor.
///
/// @param[in] pTree  Tree to iterate.
/// @param[in] index  Node index at which to start iterating.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::Iterator(
    RbTree* pTree,
    size_t index )
    : ConstIterator( pTree, index )
{
}

/// Access the current tree entry.
///
/// @return  Reference to the current tree entry.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Value& Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::operator*() const
{
    HELIUM_ASSERT( m_pTree );
    HELIUM_ASSERT( m_index < m_pTree->m_values.GetSize() );

    return m_pTree->m_values[ m_index ];
}

/// Access the current tree entry.
///
/// @return  Pointer to the current tree entry.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
Value* Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::operator->() const
{
    HELIUM_ASSERT( m_pTree );
    HELIUM_ASSERT( m_index < m_pTree->m_values.GetSize() );

    return &m_pTree->m_values[ m_index ];
}

/// Increment this iterator to the next tree entry.
///
/// @return  Reference to this iterator.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator&
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::operator++()
{
    ConstIterator::operator++();

    return *this;
}

/// Post-increment this iterator to the next tree entry.
///
/// @return  Copy of this iterator at the location prior to incrementing.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::operator++( int )
{
    Iterator iterator = *this;
    ++( *this );

    return iterator;
}

/// Decrement this iterator to the previous tree entry.
///
/// @return  Reference to this iterator.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator&
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::operator--()
{
    ConstIterator::operator--();

    return *this;
}

/// Post-decrement this iterator to the previous tree entry.
///
/// @return  Copy of this iterator at the location prior to decrementing.
template< typename Value, typename Key, typename ExtractKey, typename CompareKey, typename Allocator, typename InternalValue >
typename Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator
    Helium::RbTree< Value, Key, ExtractKey, CompareKey, Allocator, InternalValue >::Iterator::operator--( int )
{
    Iterator iterator = *this;
    --( *this );

    return iterator;
}

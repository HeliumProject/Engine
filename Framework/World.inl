//----------------------------------------------------------------------------------------------------------------------
// World.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the number of slices currently active in this world.
    ///
    /// @return  Slice count.
    ///
    /// @see GetSlice()
    size_t World::GetSliceCount() const
    {
        return m_slices.GetSize();
    }
}

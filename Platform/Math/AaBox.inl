//----------------------------------------------------------------------------------------------------------------------
// AaBox.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

namespace Helium
{
    /// Get the minimum box coordinates.
    ///
    /// @return  Minimum box coordinates.
    ///
    /// @see GetMaximum()
    const Vector3& AaBox::GetMinimum() const
    {
        return m_minimum;
    }

    /// Get the maximum box coordinates.
    ///
    /// @return  Maximum box coordinates.
    ///
    /// @see GetMinimum()
    const Vector3& AaBox::GetMaximum() const
    {
        return m_maximum;
    }
}

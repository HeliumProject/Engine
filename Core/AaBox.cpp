//----------------------------------------------------------------------------------------------------------------------
// AaBox.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "CorePch.h"
#include "Core/AaBox.h"

namespace Lunar
{
    /// Constructor.
    ///
    /// This will initialize this bounding box with both minimum and maximum coordinates set to the origin.
    AaBox::AaBox()
        : m_minimum( 0.0f )
        , m_maximum( 0.0f )
    {
    }

    /// Constructor.
    ///
    /// @param[in] rMinimum  Value to which the minimum coordinates should be initialized.
    /// @param[in] rMaximum  Value to which the maximum coordinates should be initialized.
    AaBox::AaBox( const Vector3& rMinimum, const Vector3& rMaximum )
    {
        Set( rMinimum, rMaximum );
    }

    /// Set the bounding box minimum and maximum coordinates.
    ///
    /// @param[in] rMinimum  Value to which the minimum coordinates should be initialized.
    /// @param[in] rMaximum  Value to which the maximum coordinates should be initialized.
    void AaBox::Set( const Vector3& rMinimum, const Vector3& rMaximum )
    {
        L_ASSERT(
            reinterpret_cast< const float32_t* >( &rMinimum )[ 0 ] <=
            reinterpret_cast< const float32_t* >( &rMaximum )[ 0 ] );
        L_ASSERT(
            reinterpret_cast< const float32_t* >( &rMinimum )[ 1 ] <=
            reinterpret_cast< const float32_t* >( &rMaximum )[ 1 ] );
        L_ASSERT(
            reinterpret_cast< const float32_t* >( &rMinimum )[ 2 ] <=
            reinterpret_cast< const float32_t* >( &rMaximum )[ 2 ] );

        m_minimum = rMinimum;
        m_maximum = rMaximum;
    }
}

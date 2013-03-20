#include "MathSimdPch.h"
#include "MathSimd/AaBox.h"

#include "Reflect/DataDeduction.h"

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::AaBox );

void Helium::Simd::AaBox::PopulateComposite( Reflect::Composite& comp )
{
	comp.AddField( &AaBox::m_minimum, TXT( "m_minimum" ) );
	comp.AddField( &AaBox::m_maximum, TXT( "m_maximum" ) );
}

/// Constructor.
///
/// This will initialize this bounding box with both minimum and maximum coordinates set to the origin.
Helium::Simd::AaBox::AaBox()
	: m_minimum( 0.0f )
	, m_maximum( 0.0f )
{
}

/// Constructor.
///
/// @param[in] rMinimum  Value to which the minimum coordinates should be initialized.
/// @param[in] rMaximum  Value to which the maximum coordinates should be initialized.
Helium::Simd::AaBox::AaBox( const Vector3& rMinimum, const Vector3& rMaximum )
{
	Set( rMinimum, rMaximum );
}

/// Set the bounding box minimum and maximum coordinates.
///
/// @param[in] rMinimum  Value to which the minimum coordinates should be initialized.
/// @param[in] rMaximum  Value to which the maximum coordinates should be initialized.
void Helium::Simd::AaBox::Set( const Vector3& rMinimum, const Vector3& rMaximum )
{
	HELIUM_ASSERT(
		reinterpret_cast< const float32_t* >( &rMinimum )[ 0 ] <=
		reinterpret_cast< const float32_t* >( &rMaximum )[ 0 ] );
	HELIUM_ASSERT(
		reinterpret_cast< const float32_t* >( &rMinimum )[ 1 ] <=
		reinterpret_cast< const float32_t* >( &rMaximum )[ 1 ] );
	HELIUM_ASSERT(
		reinterpret_cast< const float32_t* >( &rMinimum )[ 2 ] <=
		reinterpret_cast< const float32_t* >( &rMaximum )[ 2 ] );

	m_minimum = rMinimum;
	m_maximum = rMaximum;
}

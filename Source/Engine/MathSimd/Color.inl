/// Constructor.
///
/// No elements are initialized by default.  Color values will need to be set before use.
Helium::Color::Color()
{
}

/// Constructor.
///
/// @param[in] r  Red component.
/// @param[in] g  Green component.
/// @param[in] b  Blue component.
/// @param[in] a  Alpha component.
Helium::Color::Color( uint8_t r, uint8_t g, uint8_t b, uint8_t a )
{
	m_components.r = r;
	m_components.g = g;
	m_components.b = b;
	m_components.a = a;
}

/// Constructor.
///
/// @param[in] r  Red component.
/// @param[in] g  Green component.
/// @param[in] b  Blue component.
/// @param[in] a  Alpha component.
Helium::Color::Color( float32_t r, float32_t g, float32_t b, float32_t a )
{
	m_components.r = static_cast< uint8_t >( Clamp( r, 0.0f, 1.0f ) * 255.0f + 0.5f );
	m_components.g = static_cast< uint8_t >( Clamp( g, 0.0f, 1.0f ) * 255.0f + 0.5f );
	m_components.b = static_cast< uint8_t >( Clamp( b, 0.0f, 1.0f ) * 255.0f + 0.5f );
	m_components.a = static_cast< uint8_t >( Clamp( a, 0.0f, 1.0f ) * 255.0f + 0.5f );
}

/// Constructor.
///
/// @param[in] argb  Packed color value (alpha stored in the most-significant byte, followed by red, then green, and
///                  finally blue).
Helium::Color::Color( uint32_t argb )
{
	m_color = argb;
}

/// Get the red component value.
///
/// @return  Red component.
///
/// @see GetG(), GetB(), GetA(), GetArgb(), SetR(), SetG(), SetB(), SetA(), SetArgb()
uint8_t Helium::Color::GetR() const
{
	return m_components.r;
}

/// Get the green component value.
///
/// @return  Green component.
///
/// @see GetR(), GetB(), GetA(), GetArgb(), SetR(), SetG(), SetB(), SetA(), SetArgb()
uint8_t Helium::Color::GetG() const
{
	return m_components.g;
}

/// Get the blue component value.
///
/// @return  Blue component.
///
/// @see GetR(), GetG(), GetA(), GetArgb(), SetR(), SetG(), SetB(), SetA(), SetArgb()
uint8_t Helium::Color::GetB() const
{
	return m_components.b;
}

/// Get the alpha component value.
///
/// @return  Alpha component.
///
/// @see GetR(), GetG(), GetB(), GetArgb(), SetR(), SetG(), SetB(), SetA(), SetArgb()
uint8_t Helium::Color::GetA() const
{
	return m_components.a;
}

/// Get the 32-bit packed color value.
///
/// @return  Packed color value (alpha stored in the most-significant byte, followed by red, then green, and finally
///          blue).
/// @see GetR(), GetG(), GetB(), GetA(), SetR(), SetG(), SetB(), SetA(), SetArgb()
uint32_t Helium::Color::GetArgb() const
{
	return m_color;
}

/// Set the red component value.
///
/// @param[in] r  Red component.
///
/// @see SetG(), SetB(), SetA(), SetArgb(), GetR(), GetG(), GetB(), GetA(), GetArgb()
void Helium::Color::SetR( uint8_t r )
{
	m_components.r = r;
}

/// Set the green component value.
///
/// @param[in] g  Green component.
///
/// @see SetR(), SetB(), SetA(), SetArgb(), GetR(), GetG(), GetB(), GetA(), GetArgb()
void Helium::Color::SetG( uint8_t g )
{
	m_components.g = g;
}

/// Set the blue component value.
///
/// @param[in] b  Blue component.
///
/// @see SetR(), SetG(), SetA(), SetArgb(), GetR(), GetG(), GetB(), GetA(), GetArgb()
void Helium::Color::SetB( uint8_t b )
{
	m_components.b = b;
}

/// Set the alpha component value.
///
/// @param[in] a  Alpha component.
///
/// @see SetR(), SetG(), SetB(), SetArgb(), GetR(), GetG(), GetB(), GetA(), GetArgb()
void Helium::Color::SetA( uint8_t a )
{
	m_components.a = a;
}

/// Set the 32-bit packed color value.
///
/// @param[in] argb  Packed color value (alpha stored in the most-significant byte, followed by red, then green, and
///                  finally blue).
///
/// @see SetR(), SetG(), SetB(), SetA(), GetR(), GetG(), GetB(), GetA(), GetArgb()
void Helium::Color::SetArgb( uint32_t argb )
{
	m_color = argb;
}

/// Get the red component as a floating-point value in the range from 0 to 1.
///
/// @return  Red component.
///
/// @see GetFloatG(), GetFloatB(), GetFloatA(), SetFloatR(), SetFloatG(), SetFloatB(), SetFloatA()
float32_t Helium::Color::GetFloatR() const
{
	return static_cast< float32_t >( m_components.r ) * ( 1.0f / 255.0f );
}

/// Get the green component as a floating-point value in the range from 0 to 1.
///
/// @return  Green component.
///
/// @see GetFloatR(), GetFloatB(), GetFloatA(), SetFloatR(), SetFloatG(), SetFloatB(), SetFloatA()
float32_t Helium::Color::GetFloatG() const
{
	return static_cast< float32_t >( m_components.g ) * ( 1.0f / 255.0f );
}

/// Get the blue component as a floating-point value in the range from 0 to 1.
///
/// @return  Blue component.
///
/// @see GetFloatR(), GetFloatG(), GetFloatA(), SetFloatR(), SetFloatG(), SetFloatB(), SetFloatA()
float32_t Helium::Color::GetFloatB() const
{
	return static_cast< float32_t >( m_components.b ) * ( 1.0f / 255.0f );
}

/// Get the alpha component as a floating-point value in the range from 0 to 1.
///
/// @return  Alpha component.
///
/// @see GetFloatR(), GetFloatG(), GetFloatB(), SetFloatR(), SetFloatG(), SetFloatB(), SetFloatA()
float32_t Helium::Color::GetFloatA() const
{
	return static_cast< float32_t >( m_components.a ) * ( 1.0f / 255.0f );
}

/// Set the red component as a floating-point value in the range from 0 to 1.
///
/// @param r  Red component.
///
/// @see SetFloatG(), SetFloatB(), SetFloatA(), GetFloatR(), GetFloatG(), GetFloatB(), GetFloatA()
void Helium::Color::SetFloatR( float32_t r )
{
	m_components.r = static_cast< uint8_t >( Clamp( r, 0.0f, 1.0f ) * 255.0f + 0.5f );
}

/// Set the green component as a floating-point value in the range from 0 to 1.
///
/// @param g  Green component.
///
/// @see SetFloatR(), SetFloatB(), SetFloatA(), GetFloatR(), GetFloatG(), GetFloatB(), GetFloatA()
void Helium::Color::SetFloatG( float32_t g )
{
	m_components.g = static_cast< uint8_t >( Clamp( g, 0.0f, 1.0f ) * 255.0f + 0.5f );
}

/// Set the blue component as a floating-point value in the range from 0 to 1.
///
/// @param b  Blue component.
///
/// @see SetFloatR(), SetFloatG(), SetFloatA(), GetFloatR(), GetFloatG(), GetFloatB(), GetFloatA()
void Helium::Color::SetFloatB( float32_t b )
{
	m_components.b = static_cast< uint8_t >( Clamp( b, 0.0f, 1.0f ) * 255.0f + 0.5f );
}

/// Set the alpha component as a floating-point value in the range from 0 to 1.
///
/// @param a  Alpha component.
///
/// @see SetFloatR(), SetFloatG(), SetFloatB(), GetFloatR(), GetFloatG(), GetFloatB(), GetFloatA()
void Helium::Color::SetFloatA( float32_t a )
{
	m_components.a = static_cast< uint8_t >( Clamp( a, 0.0f, 1.0f ) * 255.0f + 0.5f );
}

/// Equality comparison operator.
///
/// @param[in] rOther  Color with which to compare.
///
/// @return  True if this color and the given color are the same, false if not.
///
/// @see operator!=()
bool Helium::Color::operator==( const Color& rOther ) const
{
	return ( m_color == rOther.m_color );
}

/// Inequality comparison operator.
///
/// @param[in] rOther  Color with which to compare.
///
/// @return  True if this color and the given color are different, false if they are the same.
///
/// @see operator==()
bool Helium::Color::operator!=( const Color& rOther ) const
{
	return ( m_color != rOther.m_color );
}

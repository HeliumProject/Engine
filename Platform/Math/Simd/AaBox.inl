/// Get the minimum box coordinates.
///
/// @return  Minimum box coordinates.
///
/// @see GetMaximum()
const Helium::Simd::Vector3& Helium::Simd::AaBox::GetMinimum() const
{
    return m_minimum;
}

/// Get the maximum box coordinates.
///
/// @return  Maximum box coordinates.
///
/// @see GetMinimum()
const Helium::Simd::Vector3& Helium::Simd::AaBox::GetMaximum() const
{
    return m_maximum;
}

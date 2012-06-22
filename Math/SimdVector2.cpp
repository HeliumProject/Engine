#include "MathPch.h"

#include "Math/SimdVector2.h"
#include "Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Vector2 );

void Helium::Simd::Vector2::PopulateComposite( Reflect::Composite& comp )
{
#pragma TODO("Support static arrays in reflect")
    comp.AddField( &Vector2::m_x,       TXT( "m_x" ) );
    comp.AddField( &Vector2::m_y,       TXT( "m_y" ) );
}
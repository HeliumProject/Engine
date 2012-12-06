#include "MathSimdPch.h"

#include "MathSimd/Vector4.h"

#include "Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Vector4 );

void Helium::Simd::Vector4::PopulateComposite( Reflect::Composite& comp )
{
#pragma TODO("Support static arrays in reflect")
    //comp.AddField( &Vector3::m_vectorAsFloatArray[0],       TXT( "m_vectorAsFloatArray" ) );
    comp.AddField( &Vector4::m_x,       TXT( "m_x" ) );
    comp.AddField( &Vector4::m_y,       TXT( "m_y" ) );
    comp.AddField( &Vector4::m_z,       TXT( "m_z" ) );
    comp.AddField( &Vector4::m_w,       TXT( "m_w" ) );
}
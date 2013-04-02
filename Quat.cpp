#include "MathSimdPch.h"
#include "MathSimd/Quat.h"
#include "Reflect/Data/DataDeduction.h"

const Helium::Simd::Quat Helium::Simd::Quat::IDENTITY( 0.0f, 0.0f, 0.0f, 1.0f );

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Quat );

void Helium::Simd::Quat::PopulateComposite( Reflect::Composite& comp )
{
#pragma TODO("Support static arrays in reflect")
    //comp.AddField( &Vector3::m_vectorAsFloatArray[0],       TXT( "m_vectorAsFloatArray" ) );
    comp.AddField( &Quat::m_x,       TXT( "m_x" ) );
    comp.AddField( &Quat::m_y,       TXT( "m_y" ) );
    comp.AddField( &Quat::m_z,       TXT( "m_z" ) );
    comp.AddField( &Quat::m_w,       TXT( "m_w" ) );
}
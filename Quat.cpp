#include "MathSimdPch.h"
#include "MathSimd/Quat.h"
#include "Reflect/TranslatorDeduction.h"

const Helium::Simd::Quat Helium::Simd::Quat::IDENTITY( 0.0f, 0.0f, 0.0f, 1.0f );

HELIUM_DEFINE_BASE_STRUCT( Helium::Simd::Quat );

void Helium::Simd::Quat::PopulateMetaType( Reflect::MetaStruct& comp )
{
#pragma TODO("Support static arrays in reflect")
    //comp.AddField( &Vector3::m_vectorAsFloatArray[0],       TXT( "m_vectorAsFloatArray" ) );
    comp.AddField( &Quat::m_x,       TXT( "m_x" ) );
    comp.AddField( &Quat::m_y,       TXT( "m_y" ) );
    comp.AddField( &Quat::m_z,       TXT( "m_z" ) );
    comp.AddField( &Quat::m_w,       TXT( "m_w" ) );
}
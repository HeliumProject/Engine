#include "MathSimdPch.h"

#include "MathSimd/Vector4.h"

#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

const Simd::Vector4 Simd::Vector4::Zero;
const Simd::Vector4 Simd::Vector4::Unit   (1.0, 1.0, 1.0, 1.0);
const Simd::Vector4 Simd::Vector4::BasisX (1.0, 0.0, 0.0, 0.0);
const Simd::Vector4 Simd::Vector4::BasisY (0.0, 1.0, 0.0, 0.0);
const Simd::Vector4 Simd::Vector4::BasisZ (0.0, 0.0, 1.0, 0.0);
const Simd::Vector4 Simd::Vector4::BasisW (0.0, 0.0, 0.0, 1.0);

HELIUM_DEFINE_BASE_STRUCT( Helium::Simd::Vector4 );

void Helium::Simd::Vector4::PopulateMetaType( Reflect::MetaStruct& comp )
{
#pragma TODO("Support static arrays in reflect")
    //comp.AddField( &Vector3::m_vectorAsFloatArray[0],       TXT( "m_vectorAsFloatArray" ) );
    comp.AddField( &Vector4::m_x,       TXT( "m_x" ) );
    comp.AddField( &Vector4::m_y,       TXT( "m_y" ) );
    comp.AddField( &Vector4::m_z,       TXT( "m_z" ) );
    comp.AddField( &Vector4::m_w,       TXT( "m_w" ) );
}
#include "MathSimdPch.h"

#include "MathSimd/Vector3.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

const Simd::Vector3 Simd::Vector3::Zero;
const Simd::Vector3 Simd::Vector3::Unit   (1.0, 1.0, 1.0);
const Simd::Vector3 Simd::Vector3::BasisX (1.0, 0.0, 0.0);
const Simd::Vector3 Simd::Vector3::BasisY (0.0, 1.0, 0.0);
const Simd::Vector3 Simd::Vector3::BasisZ (0.0, 0.0, 1.0);

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Vector3 );

void Simd::Vector3::PopulateStructure( Reflect::Structure& comp )
{
#pragma TODO("Support static arrays in reflect")
    //comp.AddField( &Vector3::m_vectorAsFloatArray[0],       TXT( "m_vectorAsFloatArray" ) );
    comp.AddField( &Vector3::m_x,       TXT( "m_x" ) );
    comp.AddField( &Vector3::m_y,       TXT( "m_y" ) );
    comp.AddField( &Vector3::m_z,       TXT( "m_z" ) );
    comp.AddField( &Vector3::m_w,       TXT( "m_w" ) );
}
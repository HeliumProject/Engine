#include "MathSimdPch.h"

#include "MathSimd/Vector2.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

const Simd::Vector2 Simd::Vector2::Zero;
const Simd::Vector2 Simd::Vector2::Unit   (1.0, 1.0);
const Simd::Vector2 Simd::Vector2::BasisX (1.0, 0.0);
const Simd::Vector2 Simd::Vector2::BasisY (0.0, 1.0);

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Vector2 );

void Helium::Simd::Vector2::PopulateStructure( Reflect::Structure& comp )
{
#pragma TODO("Support static arrays in reflect")
    comp.AddField( &Vector2::m_x,       TXT( "m_x" ) );
    comp.AddField( &Vector2::m_y,       TXT( "m_y" ) );
}
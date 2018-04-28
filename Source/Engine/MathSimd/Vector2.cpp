#include "Precompile.h"

#include "MathSimd/Vector2.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

const Simd::Vector2 Simd::Vector2::Zero;
const Simd::Vector2 Simd::Vector2::Unit   (1.0, 1.0);
const Simd::Vector2 Simd::Vector2::BasisX (1.0, 0.0);
const Simd::Vector2 Simd::Vector2::BasisY (0.0, 1.0);

HELIUM_DEFINE_BASE_STRUCT( Helium::Simd::Vector2 );

void Helium::Simd::Vector2::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Vector2::m_vectorAsFloatArray, "m_vectorAsFloatArray" );
}
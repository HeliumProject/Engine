#include "Precompile.h"

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
	comp.AddField( &Vector4::m_vectorAsFloatArray, "m_vectorAsFloatArray" );
}
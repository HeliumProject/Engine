#include "Precompile.h"

#include "MathSimd/Vector3.h"
#include "Reflect/TranslatorDeduction.h"

using namespace Helium;

const Simd::Vector3 Simd::Vector3::Zero;
const Simd::Vector3 Simd::Vector3::Unit   (1.0, 1.0, 1.0);
const Simd::Vector3 Simd::Vector3::BasisX (1.0, 0.0, 0.0);
const Simd::Vector3 Simd::Vector3::BasisY (0.0, 1.0, 0.0);
const Simd::Vector3 Simd::Vector3::BasisZ (0.0, 0.0, 1.0);

HELIUM_DEFINE_BASE_STRUCT( Helium::Simd::Vector3 );

void Simd::Vector3::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Vector3::m_vectorAsFloatArray, "m_vectorAsFloatArray" );
}
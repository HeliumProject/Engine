#include "Precompile.h"
#include "MathSimd/Quat.h"
#include "Reflect/TranslatorDeduction.h"

const Helium::Simd::Quat Helium::Simd::Quat::IDENTITY( 0.0f, 0.0f, 0.0f, 1.0f );

HELIUM_DEFINE_BASE_STRUCT( Helium::Simd::Quat );

void Helium::Simd::Quat::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &Quat::m_quatAsFloatArray, "m_quatAsFloatArray" );
}
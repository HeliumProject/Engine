#include "FoundationPch.h"

#include "Foundation/Math/SimdVector3.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_BASE_STRUCTURE( Helium::Simd::Vector3 );

void Helium::Simd::Vector3::PopulateComposite( Reflect::Composite& comp )
{
#pragma TODO("Support static arrays in reflect")
    //comp.AddField( &Vector3::m_vectorAsFloatArray[0],       TXT( "m_vectorAsFloatArray" ) );
    comp.AddField( &Vector3::m_x,       TXT( "m_x" ) );
    comp.AddField( &Vector3::m_y,       TXT( "m_y" ) );
    comp.AddField( &Vector3::m_z,       TXT( "m_z" ) );
    comp.AddField( &Vector3::m_w,       TXT( "m_w" ) );
}
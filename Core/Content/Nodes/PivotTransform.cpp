#include "Core/Content/Nodes/PivotTransform.h"

using namespace Helium;
using namespace Helium::Content;

REFLECT_DEFINE_CLASS(PivotTransform)

void PivotTransform::EnumerateClass( Reflect::Compositor<PivotTransform>& comp )
{
    Reflect::Field* fieldShear = comp.AddField( &PivotTransform::m_Shear, "m_Shear", 0, Reflect::GetType< Reflect::Vector3Serializer >() );

    Reflect::Field* fieldScalePivot = comp.AddField( &PivotTransform::m_ScalePivot, "m_ScalePivot" );
    Reflect::Field* fieldScalePivotTranslate = comp.AddField( &PivotTransform::m_ScalePivotTranslate, "m_ScalePivotTranslate" );

    Reflect::Field* fieldRotatePivot = comp.AddField( &PivotTransform::m_RotatePivot, "m_RotatePivot" );
    Reflect::Field* fieldRotatePivotTranslate = comp.AddField( &PivotTransform::m_RotatePivotTranslate, "m_RotatePivotTranslate" );

    Reflect::Field* fieldTranslatePivot = comp.AddField( &PivotTransform::m_TranslatePivot, "m_TranslatePivot" );
    Reflect::Field* fieldSnapPivots = comp.AddField( &PivotTransform::m_SnapPivots, "m_SnapPivots" );
}


bool PivotTransform::ProcessComponent( Reflect::ElementPtr element, const tstring& memberName )
{
    if ( memberName == TXT( "m_LockPivots" ) )
    {
        Reflect::Serializer::GetValue( Reflect::AssertCast< Reflect::Serializer >( element ), m_SnapPivots );
        return true;
    }

    return __super::ProcessComponent( element, memberName );
}

void PivotTransform::ResetTransform()
{
    __super::ResetTransform();

    m_Shear = Math::Shear::Identity;
    m_ScalePivot = Math::Vector3::Zero;
    m_ScalePivotTranslate = Math::Vector3::Zero;
    m_RotatePivot = Math::Vector3::Zero;
    m_RotatePivotTranslate = Math::Vector3::Zero;
    m_TranslatePivot = Math::Vector3::Zero;
}
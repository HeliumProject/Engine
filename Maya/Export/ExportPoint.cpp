#include "Precompile.h"
#include "ExportPoint.h"

using namespace MayaContent;

ExportPoint::ExportPoint( const MPoint& mayaPoint )
: ExportBase( new Content::Point() )
{
  GetContentPoint()->m_Position = ( Math::Vector3( 
    (float)mayaPoint.x * Math::CentimetersToMeters,
    (float)mayaPoint.y * Math::CentimetersToMeters,
    (float)mayaPoint.z * Math::CentimetersToMeters ) );
}

Content::Point* ExportPoint::GetContentPoint() const
{
  return Reflect::DangerousCast< Content::Point >( m_ContentObject );
}

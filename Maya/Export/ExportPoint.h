#pragma once

#include "ExportBase.h"
#include "Content/Point.h"

#include <maya/MPoint.h>

namespace MayaContent
{
  class MAYA_API ExportPoint : public ExportBase
  {
  public:
    ExportPoint( const MPoint& mayaPoint );

    Content::Point* GetContentPoint() const;
  };
  typedef Nocturnal::SmartPtr< ExportPoint > ExportPointPtr;
}

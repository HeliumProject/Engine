#pragma once

#include "ExportBase.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportPivotTransform : public ExportBase
  {
  public:
    ExportPivotTransform( const MObject& mayaObject, const Nocturnal::UID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::PivotTransform( id );
    }

    // Gather the necessary maya data
    void GatherMayaData( V_ExportBase &newExportObjects );
  };

  typedef Nocturnal::SmartPtr<ExportPivotTransform> ExportPivotTransformPtr;
}

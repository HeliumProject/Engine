#pragma once

#include "ExportBase.h"
#include "Content/WaterPlane.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportWaterPlane : public ExportBase
  {
  public:
    ExportWaterPlane( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::WaterPlane( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Nocturnal::SmartPtr<ExportWaterPlane> ExportWaterPlanePtr;
}

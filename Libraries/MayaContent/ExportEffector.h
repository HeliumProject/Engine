#pragma once

#include "ExportBase.h"
#include "Content/Effector.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportEffector : public ExportBase
  {
  public:
    ExportEffector( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase ( mayaObject )
    {
      m_ContentObject = new Content::Effector( id );
    }

    // Gather the necessary maya data
    void GatherMayaData( V_ExportBase &newExportObjects );
  };

  typedef Nocturnal::SmartPtr<ExportEffector> ExportEffectorPtr;
}

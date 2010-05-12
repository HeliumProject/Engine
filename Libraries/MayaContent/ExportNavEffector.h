#pragma once

#include "ExportBase.h"
#include "Content/NavEffector.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportNavEffector : public ExportBase
  {
  public:
    ExportNavEffector( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::NavEffector( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Nocturnal::SmartPtr<ExportNavEffector> ExportNavEffectorPtr;
}

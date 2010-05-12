#pragma once

#include "ExportBase.h"
#include "Content/Volume.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportGameplay : public ExportBase
  {
  public:
    ExportGameplay( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::Volume( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Nocturnal::SmartPtr<ExportGameplay> ExportGameplayPtr;
}

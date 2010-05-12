#pragma once

#include "ExportBase.h"
#include "Content/NavClue.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportNavClue : public ExportBase
  {
  public:
    ExportNavClue( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::NavClue( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Nocturnal::SmartPtr<ExportNavClue> ExportNavCluePtr;
}

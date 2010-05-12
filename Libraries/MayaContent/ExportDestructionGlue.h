#pragma once

#include "ExportBase.h"
#include "Content/DestructionGlue.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportDestructionGlue : public ExportBase
  {
  public:
    ExportDestructionGlue( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::DestructionGluePrim( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Nocturnal::SmartPtr<ExportDestructionGlue> ExportDestructionGluePtr;
}

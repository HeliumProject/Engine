#pragma once

#include "ExportBase.h"
#include "Pipeline/Asset/Classes/EntityInstance.h"

namespace MayaContent
{
  class MAYA_API ExportEntity : public ExportBase
  {
  public:
    ExportEntity( const MObject& mayaObject, Helium::TUID id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Asset::EntityInstance( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Helium::SmartPtr<ExportEntity> ExportEntityPtr;
}

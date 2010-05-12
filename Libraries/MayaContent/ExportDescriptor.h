#pragma once

#include "ExportPivotTransform.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportDescriptor : public ExportPivotTransform
  {
  public:
    ExportDescriptor( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportPivotTransform ( mayaObject, id )
    {
      m_ContentObject = new Content::Descriptor( id );
    }

    // Gather the necessary maya data
    void GatherMayaData( V_ExportBase &newExportObjects );
  };

  typedef Nocturnal::SmartPtr<ExportDescriptor> ExportDescriptorPtr;
}

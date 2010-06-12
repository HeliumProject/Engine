#pragma once

#include "Content/Curve.h"

#include "ExportBase.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportCurve : public ExportBase
  {
  public:
      ExportCurve( const MObject& mayaCurve, const Nocturnal::TUID& id ) 
      : ExportBase( mayaCurve )
    {
      m_ContentObject = new Content::Curve( id );
    }

    virtual void GatherMayaData( V_ExportBase& newExportObjects ) NOC_OVERRIDE;
  };
}
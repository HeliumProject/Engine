#pragma once

#include "ExportBase.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportJoint : public ExportBase
  {
  public:
    ExportJoint()
    {

    }

    ExportJoint( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::JointTransform( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
  };

  typedef Nocturnal::SmartPtr<ExportJoint> ExportJointPtr;
}

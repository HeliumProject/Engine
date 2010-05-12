#pragma once

#include "ExportBase.h"
#include "Content/PhysicsJoint.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportPhysicsJoint : public ExportBase
  {
  public:
    ExportPhysicsJoint( const MObject& mayaObject, UniqueID::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::PhysicsJoint( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
  };

  typedef Nocturnal::SmartPtr< ExportPhysicsJoint > ExportPhysicsJointPtr;
}

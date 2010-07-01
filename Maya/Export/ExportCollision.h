#pragma once

#include "ExportBase.h"
#include "Pipeline/Content/Nodes/CollisionPrimitive.h"

namespace MayaContent
{
  namespace CollisionFlags
  {
    enum CollisionFlag
    {
      LowResolution = 1 << 0,
      Move          = 1 << 1,
      Projectile    = 1 << 2,
      Physics       = 1 << 3,
      Ground        = 1 << 4,
      Effects       = 1 << 5,
      NoMapping     = 1 << 6,
      NoCollision   = 1 << 7,
    };
  }

  class MAYA_API ExportCollision : public ExportBase
  {
  public:
    ExportCollision( const MObject& mayaObject, Nocturnal::TUID& id )
      : ExportBase( mayaObject )
    {
      m_ContentObject = new Content::CollisionPrimitive( id );
    }

    // Gather the necessary maya data
    virtual void GatherMayaData( V_ExportBase &newExportObjects );
   };

  typedef Nocturnal::SmartPtr<ExportCollision> ExportCollisionPtr;
}

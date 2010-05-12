#pragma once

#include "Content/CineScene.h"
#include "ExportAnimationBase.h"

#include <maya/MObjectArray.h>
#include <maya/MDagPathArray.h>

#include "MayaUtils/Utils.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportCineScene : public ExportAnimationBase
  {
  protected:
    MObjectArray m_ExportObjects;
    MDagPathArray m_CameraShape;
    MDagPathArray m_LightShape;
    MDagPathArray m_FXNodeShape;
    MDagPathArray m_EffectSpecPlayerNodeShape;

  public:
    ExportCineScene( const MObjectArray& exportObjects )
    {
      Maya::appendObjectArray( m_ExportObjects, exportObjects );

      m_ContentObject = new Content::CineScene( );

      SetFullTimeRange( true );
    }

    // Get the pointer to exported data
    Content::CineScenePtr CineScene()
    {
      return Reflect::DangerousCast< Content::CineScene >( m_ContentObject );
    }

    // Gather the necessary maya data other than frame-by-frame data
    void GatherMayaData( V_ExportBase & newExportObjects );

    // gather data for a single frame
    virtual void SampleOneFrame( const MTime & currentTime, bool extraFrame = false ) NOC_OVERRIDE;

  protected:
    void GatherMayaData_Cameras( V_ExportBase &newExportObjects );
    void GatherMayaData_Lights( V_ExportBase &newExportObjects );
    void GatherMayaData_Effects( V_ExportBase &newExportObjects );
    void GatherMayaData_EffectSpecPlayers( V_ExportBase &newExportObjects );

    void SampleOneFrame_Cameras( const MTime & currentTime );
    void SampleOneFrame_Lights( const MTime & currentTime );
    void SampleOneFrame_Effects( const MTime & currentTime );
    void SampleOneFrame_EffectSpecPlayers( const MTime & currentTime );
  };

  typedef Nocturnal::SmartPtr<ExportCineScene> ExportCineScenePtr;
}

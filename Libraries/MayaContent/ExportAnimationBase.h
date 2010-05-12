#pragma once

#include "ExportBase.h"
#include <vector>

namespace MayaContent
{
  class ExportAnimationBase;
  typedef Nocturnal::SmartPtr<ExportAnimationBase> ExportAnimationBasePtr;
  typedef std::vector<ExportAnimationBasePtr> V_ExportAnimationBasePtr;

  class MAYA_CONTENT_API ExportAnimationBase : public ExportBase
  {
  protected:
    bool m_IsFullTimeRange;

  public:
    ExportAnimationBase()
      : ExportBase(),
        m_IsFullTimeRange( false )
    {
      RememberAnimationExporter( this );
    }

    ExportAnimationBase( const MObject& mayaObject )
      : ExportBase( mayaObject ),
        m_IsFullTimeRange( false )
    {
      RememberAnimationExporter( this );
    }

    ExportAnimationBase( const Content::SceneNodePtr& contentObject )
      : ExportBase( contentObject ),
        m_IsFullTimeRange( false )
    {
      RememberAnimationExporter( this );
    }

    ExportAnimationBase( const MObject& mayaObject, const Content::SceneNodePtr& contentObject ) 
      : ExportBase( mayaObject, contentObject ),
        m_IsFullTimeRange( false )
    {
      RememberAnimationExporter( this );            
    }

    virtual ~ExportAnimationBase()
    {
      ForgetAnimationExporter( this );
    }

    // gather data for a single frame.  if extraFrame is set, this frame should not get put into the main
    // animation data
    virtual void SampleOneFrame( const MTime & currentTime, bool extraFrame = false ) = 0;

    bool IsFullTimeRange()
    {
      return m_IsFullTimeRange;
    }

    void SetFullTimeRange( bool isFull = true )
    {
      m_IsFullTimeRange = isFull;
    }

    // Convert rate to FPS
    static int Rate( MTime::Unit units );

    // Sample the animation data for all current ExportAnimationBase objects
    static void SampleMayaAnimationData();

  protected:
    static V_ExportAnimationBasePtr s_AnimationExporters;
    static bool HasFullTimeRangeExporter();
    static void RememberAnimationExporter( ExportAnimationBase * exporter );
    static void ForgetAnimationExporter( ExportAnimationBase * exporter );
  };
}

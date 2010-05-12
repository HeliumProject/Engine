#pragma once

#include "ExportShader.h"
#include "UniqueID/TUID.h"
#include "Content/BlendShader.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportBlendShader : public ExportShader
  {
  public:
    ExportBlendShader( const MObject& mayaShader, UniqueID::TUID& id )
      : ExportShader ( mayaShader, id )
    {
      m_ContentObject = new Content::BlendShader( id );
    }

    const Content::BlendShaderPtr GetContentBlendShader() const
    {
      return Reflect::DangerousCast< Content::BlendShader >( m_ContentObject );
    }

    void GatherMayaData( V_ExportBase &newExportObjects );
  };

  typedef Nocturnal::SmartPtr< ExportBlendShader > ExportBlendShaderPtr;
  typedef std::vector< ExportBlendShaderPtr > V_ExportBlendShader;
}
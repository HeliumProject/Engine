#pragma once

#include "ExportBase.h"
#include "UniqueID/TUID.h"
#include "Content/Shader.h"

namespace MayaContent
{
  class MAYA_CONTENT_API ExportShader : public ExportBase
  {
  public:
    float m_OffsetU;
    float m_OffsetV;
    float m_RotateUV;

    Math::Vector2 m_URotate;
    Math::Vector2 m_VRotate;
    Math::Vector2 m_UVTranslate;

    ExportShader( const Content::ShaderPtr& contentShader ) 
      : ExportBase( contentShader )
      , m_OffsetU( 0.0f )
      , m_OffsetV( 0.0f )
      , m_RotateUV( 0.0f )
    {

    }

    ExportShader( const MObject& mayaShader, UniqueID::TUID& id ) 
      : ExportBase( mayaShader )
      , m_OffsetU( 0.0f )
      , m_OffsetV( 0.0f )
      , m_RotateUV( 0.0f )
    {
      m_ContentObject = new Content::Shader( id );
    }

    virtual void GatherMayaData( V_ExportBase &newExportObjects ) NOC_OVERRIDE;

    virtual void ProcessMayaData();

    const Content::ShaderPtr GetContentShader() const
    {
      return Reflect::DangerousCast< Content::Shader >( m_ContentObject );
    }
  };

  typedef Nocturnal::SmartPtr< ExportShader > ExportShaderPtr;
  typedef std::vector< ExportShaderPtr > V_ExportShader;
  typedef stdext::hash_map< UniqueID::TUID, ExportShaderPtr, UniqueID::TUIDHasher > M_UIDExportShader;
}
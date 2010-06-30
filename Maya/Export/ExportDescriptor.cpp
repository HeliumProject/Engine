#include "Precompile.h"
#include "ExportDescriptor.h"
#include "MayaNodes/ExportNode.h"

using namespace MayaContent;

// should be the same as s_GeometrySimulationAttrName in \MayaNodes\ExportNode.cpp
const char* s_GeometrySimulationAttrName  = "GeometrySimulation";

void ExportDescriptor::GatherMayaData( V_ExportBase &newExportObjects )
{  
  EXPORT_SCOPE_TIMER( ("") );

  Content::Descriptor* transform = Reflect::DangerousCast< Content::Descriptor >( m_ContentObject );

  // get the export attributes and set them on the Content::ExportNode
  MPlug plug( m_MayaObject, ExportNode::s_attr_contentType );
  int tmp;
  plug.getValue( tmp );
  transform->m_ExportType = (Content::ContentType)tmp;

  if ( transform->m_ExportType == Content::ContentTypes::Bangle )
  {
    MStatus status;
    MFnDependencyNode nodeFn( m_MayaObject );

    if ( nodeFn.hasAttribute( s_GeometrySimulationAttrName, &status ) )
    {
      MPlug geomSimPlug = nodeFn.findPlug( s_GeometrySimulationAttrName, &status );

      int tmpGeomSim = 0;
      status = geomSimPlug.getValue( tmpGeomSim );
      
      if ( status )
      {
        transform->m_BangleGeometrySimulation = (Content::GeometrySimulation) tmpGeomSim;
      }
    }
  }

  plug.setAttribute( ExportNode::s_attr_contentNumber );
  plug.getValue( tmp );
  transform->m_ContentNum = (u16)tmp;

  __super::GatherMayaData( newExportObjects );
}
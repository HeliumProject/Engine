#include "stdafx.h"

#include "ExportWaterPlane.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

#include "MayaMeshSetup/MaterialDisplayNode.h"

#include <maya/MFnStringData.h>

using namespace Content;
using namespace MayaContent;

void ExportWaterPlane::GatherMayaData( V_ExportBase &newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::WaterPlane
  Content::WaterPlane* waterPlane = Reflect::DangerousCast< Content::WaterPlane >( m_ContentObject );

  MStatus status;
  MFnDagNode nodeFn( m_MayaObject );
  waterPlane->m_DefaultName = nodeFn.name().asChar();

  // get the amplitude
  waterPlane->m_Amplitude = 0.0f;
  if ( nodeFn.hasAttribute( "amplitude", &status ) )
  {
    MPlug amplitudePlug = nodeFn.findPlug( "amplitude", &status );
    float amplitude = 0.0f;    
    status = amplitudePlug.getValue( amplitude );
    if ( status )
    {
      waterPlane->m_Amplitude = amplitude;
    }
  }

  // get the max wavelength
  waterPlane->m_MaxWavelength = 0.0f;
  if ( nodeFn.hasAttribute( "maxWavelength", &status ) )
  {
    MPlug maxWavelengthPlug = nodeFn.findPlug( "maxWavelength", &status );
    float maxWavelength = 0.0f;    
    status = maxWavelengthPlug.getValue( maxWavelength );
    if ( status )
    {
      waterPlane->m_MaxWavelength = maxWavelength;
    }
  }
  
  // get the murkiness (deprecated, just exporting it)
  waterPlane->m_Murkiness= 0.0f;
  if ( nodeFn.hasAttribute( "murkiness", &status ) )
  {
    MPlug murkinessPlug = nodeFn.findPlug( "murkiness", &status );
    float murkiness = 0.0f;    
    status = murkinessPlug.getValue( murkiness );
    if ( status )
    {
      waterPlane->m_Murkiness = murkiness;
    }
  }
  
  // get the alpha
  waterPlane->m_WaterAlpha = 0.0f;
  if ( nodeFn.hasAttribute( "waterAlpha", &status ) )
  {
    MPlug waterAlphaPlug = nodeFn.findPlug( "waterAlpha", &status );
    float waterAlpha = 0.0f;    
    status = waterAlphaPlug.getValue( waterAlpha );
    if ( status )
    {
      waterPlane->m_WaterAlpha = waterAlpha;
    }
  }
  
  // get the color
  waterPlane->m_Color.Set( 0.0f, 0.0f, 0.0f );
  if ( nodeFn.hasAttribute( "waterColor", &status ) )
  {
    MPlug colorPlug = nodeFn.findPlug( "waterColor", &status );
    MObject colorObject;
    status = colorPlug.getValue( colorObject );
    
    if ( status )
    {
      MFnNumericData nData;
      nData.setObject( colorObject );
      f32 red = 0.0f;
      f32 green = 0.0f;
      f32 blue = 0.0f;
      status = nData.getData3Float( red, green, blue );
      if ( status )
      {
        waterPlane->m_Color.Set( red, green, blue );
      }
    }
  }

  // get the clip map file, if it exists
  waterPlane->m_ClipMapTuid = TUID::Null;

  if ( nodeFn.hasAttribute( "tuid", &status ) )
  {
    tuid id = Maya::GetTUIDAttribute( m_MayaObject, "tuid", &status );
    if ( status )
    {
      waterPlane->m_ClipMapTuid = id;
    }
  }

  waterPlane->m_Material = Maya::MaterialDisplayNode::GetObjectMaterial( m_MayaObject, &status );
  if ( !waterPlane->m_Material )
  {
    waterPlane->m_Material = new Content::Material();
  }

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), waterPlane->m_GlobalTransform );
  waterPlane->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  waterPlane->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  waterPlane->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  // link it into the hierarchy
  MObject parent = nodeFn.parent( 0 );
  waterPlane->m_ParentID = Maya::GetNodeID( parent );

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID( parent ) );
  newExportObjects.push_back( parentTransform );
  
  __super::GatherMayaData( newExportObjects );
}

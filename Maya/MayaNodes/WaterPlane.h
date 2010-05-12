#pragma once

#include "Plane.h"

class MGLFunctionTable;

////////////////////////////////////////////////////////////////////////////////////////////////
//
//  WaterPlane()
//
//  written by: Giac Veltri
//
//  WaterPlane locator
//
////////////////////////////////////////////////////////////////////////////////////////////////

class MAYANODES_API WaterPlane : public Plane
{
public:
  static const MTypeId s_TypeID;
  static const char* s_TypeName;
  static MStatus Initialize();
  static void* Creator() { return new WaterPlane; }

  WaterPlane();
  ~WaterPlane();

  virtual void postConstructor();
  virtual void draw ( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );
  virtual bool isTransparent() const { return true; }
  
  static MObject amplitude;
  static MObject max_wavelength;
  static MObject murkiness;
  static MObject water_alpha;
  static MObject color;
  static MObject clipmap_name;
  static MObject tuid;

  MGLFunctionTable*    m_glft;
  GLuint               m_program;
  MHwTextureFileHandle m_texture_handle;
  MCallbackId          m_cb_id;
};

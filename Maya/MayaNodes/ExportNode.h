#pragma once

#include "API.h"

#include "Content/AnimationClip.h"
#include "Content/ContentTypes.h"

#include <maya/MNodeMessage.h>
#include <maya/MPxTransform.h>

class MAYANODES_API ExportNode : public MPxTransform 
{
public:
  ExportNode();
  ~ExportNode();

  static  void *      Creator();
  static  MStatus			Initialize();	

  virtual void postConstructor();

  static MObject CreateExportNode( Content::ContentType exportType = Content::ContentTypes::Default, int num = 0 );

  static void FindExportNodes( MObjectArray& exportNodes, Content::ContentType exportType = Content::ContentTypes::Default, MObject node = MObject::kNullObj, int num = -1 );
  static bool HasContentType( MObject& object, Content::ContentType exportType );
  //
  // ExportNode-related callbacks
  //

  static void NodeAddedCB( MObject &node, void *clientData );
  static void AttributeChangedCB( MNodeMessage::AttributeMessage msg, 
                                  MPlug& plug, MPlug& otherPlug, void*);

  // per-instance callback adder/removers
  void addAttributeChangedCB();
  void removeAttributeChangedCB();

private:
  // per-instance callback id
  MCallbackId m_AttributeChangedCBId;

public:
  // custom node info
  static const MTypeId s_TypeID;
  static const char* s_TypeName;

  // attribs
  static MObject  s_attr_prev_contentType;
  static MObject  s_attr_contentType;
  static MObject  s_attr_contentNumber;

  // attribute names
  static MString s_KeyPoseAttrName;
  static MString s_DefaultDrawAttrName;
  static MString s_CheapChunkAttrName;
  static MString s_GeomSimAttrName;

  static MString s_RisingWaterEnumAttrName;
  static MString s_RisingWaterNameAttrName;
  static MString s_RisingWaterEnumIDAttrName;

  static MString s_UseWrinkleMapAttrName;
  static MString s_WrinkleMapRegionAttrNames[Content::MaxCountWrinkleMapRegions];
};


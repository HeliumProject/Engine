#include "stdafx.h"

#include "ExportDestructionGlue.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

#include "MayaMeshSetup/MaterialDisplayNode.h"

using namespace Content;
using namespace MayaContent;

void ExportDestructionGlue::GatherMayaData( V_ExportBase &newExportObjects )
{
  MFnDagNode nodeFn (m_MayaObject);

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CollisionPrimitive
  Content::DestructionGluePrim* primitive = Reflect::DangerousCast< Content::DestructionGluePrim >( m_ContentObject );

  primitive->m_DefaultName = nodeFn.name().asChar();

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), primitive->m_GlobalTransform );
  primitive->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  MStatus status;


  MPlug joint_a_plug = nodeFn.findPlug("Joint_Name_A", &status);
  if(status == MStatus::kSuccess)
  {
    MString joint_a_name;
    joint_a_plug.getValue(joint_a_name);
    primitive->m_joint_a_name = joint_a_name.asChar();
  }

  MPlug joint_b_plug = nodeFn.findPlug("Joint_Name_B", &status);
  if(status == MStatus::kSuccess)
  {
    MString joint_b_name;
    joint_b_plug.getValue(joint_b_name);
    primitive->m_joint_b_name = joint_b_name.asChar();
  }

  MPlug elastic_limit_normal_plug = nodeFn.findPlug("Elastic_Limit_Normal", &status);
  if(status == MStatus::kSuccess)
  {
    elastic_limit_normal_plug.getValue(primitive->m_elastic_limit_normal);
  }

  MPlug elastic_limit_torsion_plug = nodeFn.findPlug("Elastic_Limit_Torsion", &status);
  if(status == MStatus::kSuccess)
  {
    elastic_limit_torsion_plug.getValue(primitive->m_elastic_limit_torsion);
  }

  MPlug plastic_limit_normal_plug = nodeFn.findPlug("Plastic_Limit_Normal", &status);
  if(status == MStatus::kSuccess)
  {
    plastic_limit_normal_plug.getValue(primitive->m_plastic_limit_normal);
  }

  MPlug plastic_limit_torsion_plug = nodeFn.findPlug("Plastic_Limit_Torsion", &status);
  if(status = MStatus::kSuccess)
  {
    plastic_limit_torsion_plug.getValue(primitive->m_plastic_limit_torsion);
  }

  MPlug material_index_plug = nodeFn.findPlug("Material_Type", &status);
  if(status = MStatus::kSuccess)
  {
    material_index_plug.getValue(*(int *)&primitive->m_material_index);
  }

  bool bool_val;

  MPlug joint_a_valid_plug = nodeFn.findPlug("Joint_A_Valid", &status);
  if(status = MStatus::kSuccess)
  {
    joint_a_valid_plug.getValue(bool_val);

    primitive->m_joint_a_valid = (bool_val!=false);
  }

  MPlug joint_b_valid_plug = nodeFn.findPlug("Joint_B_Valid", &status);
  if(status = MStatus::kSuccess)
  {
    joint_b_valid_plug.getValue(bool_val);

    primitive->m_joint_b_valid = (bool_val!=false);

  }
#if 0
  char tmp_string[512];
  sprintf(tmp_string, "joint_a_valid: %d\n", primitive->m_joint_a_valid);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "joint_b_valid: %d\n", primitive->m_joint_b_valid);
  MGlobal::displayInfo(tmp_string);
#endif


  u32 typeID = nodeFn.typeId().id();


  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  primitive->m_ParentID = Maya::GetNodeID(parent);

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}

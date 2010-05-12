#include "stdafx.h"

#include "ExportCollision.h"
#include "MayaContentCmd.h"
#include "ExportPivotTransform.h"

#include "MayaMeshSetup/MaterialDisplayNode.h"

using namespace Content;
using namespace MayaContent;

void ExportCollision::GatherMayaData( V_ExportBase &newExportObjects )
{
  MFnDagNode nodeFn (m_MayaObject);

  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CollisionPrimitive
  Content::CollisionPrimitive* primitive = Reflect::DangerousCast< Content::CollisionPrimitive >( m_ContentObject );

  primitive->m_DefaultName = nodeFn.name().asChar();

  // this will retrieve and setup the global matrix for this mesh (because meshs are transforms in Content but not Maya)
  MayaContentCmd::ConvertMatrix( MDagPath::getAPathTo( m_MayaObject ).inclusiveMatrix(), primitive->m_GlobalTransform );
  primitive->m_GlobalTransform.t.x *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.y *= Math::CentimetersToMeters;
  primitive->m_GlobalTransform.t.z *= Math::CentimetersToMeters;

  u32 typeID = nodeFn.typeId().id();

  switch( typeID )
  {
  case IGL_COLL_SPHERE:
    primitive->m_Shape = CollisionShapes::Sphere; 
    break;

  case IGL_COLL_CAPSULE:
    primitive->m_Shape = CollisionShapes::Capsule;
    break;

  case IGL_COLL_CAPSULE_CHILD:
    primitive->m_Shape = CollisionShapes::CapsuleChild; 
    break;

  case IGL_COLL_CYLINDER:
    primitive->m_Shape = CollisionShapes::Cylinder; 
    break;

  case IGL_COLL_CYLINDER_CHILD:
    primitive->m_Shape = CollisionShapes::CylinderChild; 
    break;

  case IGL_COLL_CUBOID:
    primitive->m_Shape = CollisionShapes::Cube; 
    break;

  case IGL_LOOSE_ATTACHMENT_SPHERE:
    primitive->m_Shape = CollisionShapes::LooseAttachmentSphere; 
    break;

  case IGL_LOOSE_ATTACHMENT_CAPSULE:
    primitive->m_Shape = CollisionShapes::LooseAttachmentCapsule; 
    break;

  case IGL_LOOSE_ATTACHMENT_CAPSULE_CHILD:
    primitive->m_Shape = CollisionShapes::LooseAttachmentCapsuleChild; 
    break;
  }

  MStatus status;

  primitive->m_Material = Maya::MaterialDisplayNode::GetObjectMaterial( m_MayaObject, &status );
  if ( !primitive->m_Material )
  {
    primitive->m_Material = new Content::Material();
  }

  MPlug bangleIndexPlug = nodeFn.findPlug("Bangle_Index");
  int bangle_index = -1;
  bangleIndexPlug.getValue(bangle_index);
  if(bangle_index < 0 || bangle_index >= 62)
  {
    primitive->m_BangleIndex = 0xff;
  }
  else
  {
    primitive->m_BangleIndex = bangle_index;
  }

  MPlug prim_falg_plug = nodeFn.findPlug("primLowRez", &status);
  int prim_flag;
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::LowResolution;
    }
  }
  prim_falg_plug = nodeFn.findPlug("primMove", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Move;
    }
  }
  prim_falg_plug = nodeFn.findPlug("primProjectile", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Projectile;
    }
  }

  MPlug num_particles_plug = nodeFn.findPlug("NumParticles", &status);
  if(status == MStatus::kSuccess)
  {
    int num_particles = 0;
    num_particles_plug.getValue(num_particles);
    primitive->m_NumParticles = num_particles;
  }


  MPlug psfPlug = nodeFn.findPlug("ParticlesSpeedFactor", &status);
  if(status == MStatus::kSuccess)
  {
    float psf = 0.0f;
    psfPlug.getValue(psf);
    primitive->m_ParticleSpeedFactor = psf;
  }


  MPlug prim_bs_plug = nodeFn.findPlug("primInBSphere", &status);
  int prim_bs_flag;
  if(status == MStatus::kSuccess)
  {
    prim_bs_flag = 0;
    prim_bs_plug.getValue(prim_bs_flag);
    if (prim_bs_flag)
    {
      primitive->m_ContributesToBsphere = true;
    }
  }

#pragma TODO( "Remove this legacy support for 'UnUsed1' in preference to the Physics one below" )
  prim_falg_plug = nodeFn.findPlug("primUnUsed1", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Physics;
    }
  }
  prim_falg_plug = nodeFn.findPlug("primPhysics", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Physics;
    }
  }

#pragma TODO( "Remove this legacy support for 'UnUsed2' in preference to the Ground one below" )
  prim_falg_plug = nodeFn.findPlug("primUnUsed2", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Ground;
    }
  }
  prim_falg_plug = nodeFn.findPlug("primGround", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Ground;
    }
  }

#pragma TODO( "Remove this legacy support for 'UnUsed3' in preference to the Effects one below" )
  prim_falg_plug = nodeFn.findPlug("primUnUsed3", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Effects;
    }
  }
  prim_falg_plug = nodeFn.findPlug("primEffects", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::Effects;
    }
  }

  prim_falg_plug = nodeFn.findPlug("primNoMapping", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::NoMapping;
    }
  }
  prim_falg_plug = nodeFn.findPlug("primNoColl", &status);
  if(status == MStatus::kSuccess)
  {
    prim_flag = 0;
    prim_falg_plug.getValue(prim_flag);
    if (prim_flag)
    {
      primitive->m_CollisionFlag |= CollisionFlags::NoCollision;
    }
  }

  MPlug densityPlug = nodeFn.findPlug("Density");
  float density = 0.0f;
  densityPlug.getValue(density);
  primitive->m_Density = density;


  if( typeID == IGL_COLL_CAPSULE || typeID == IGL_COLL_CYLINDER  ||  typeID == IGL_LOOSE_ATTACHMENT_CAPSULE)
  {
    MFnDagNode parentFn(nodeFn.parent(0));

    MStatus mstatus;
    MPlug Plug = parentFn.findPlug("scaleX");

    MPlugArray test;
    if(Plug.connectedTo(test, true, true))
    {
      //2 expression and one connection to child sphere
      if(test.length() == 3)
      {
        MFnDagNode dn;
        //get the transform of the child spshere
        for (int i=0; i<3; i++)
        {
          dn.setObject(test[i].node());
          if (stricmp(dn.typeName().asChar(), "transform") == 0)
          {
            dn.setObject(dn.child(0));
            //Maya::FullPathNameWithoutNamespace( dn, primitive->m_ChildName );
            primitive->m_ChildID = Maya::GetNodeID(dn.object());
            break;
          }
        }
      }
    }
  }

  // link it into the hierarchy
  MObject parent = nodeFn.parent(0);
  primitive->m_ParentID = Maya::GetNodeID(parent);

  ExportPivotTransformPtr parentTransform = new ExportPivotTransform( parent, Maya::GetNodeID(parent) );
  newExportObjects.push_back( parentTransform );

  __super::GatherMayaData( newExportObjects );
}

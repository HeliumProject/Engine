#include "stdafx.h"
#include "LooseAttachmentSphere.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId LooseAttachmentSphere::s_TypeID( IGL_LOOSE_ATTACHMENT_SPHERE );
const char* LooseAttachmentSphere::s_TypeName = "igLooseAttachmentSphere";

MStatus LooseAttachmentSphere::Initialize()
{ 
  return inheritAttributesFrom( Sphere::s_TypeName );
}

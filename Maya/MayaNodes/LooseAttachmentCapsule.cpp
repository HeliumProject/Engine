#include "stdafx.h"
#include "LooseAttachmentCapsule.h"

#include "MayaUtils/NodeTypes.h"

const MTypeId LooseAttachmentCapsule::s_TypeID( IGL_LOOSE_ATTACHMENT_CAPSULE );
const char* LooseAttachmentCapsule::s_TypeName = "igLooseAttachmentCapsule";

MStatus LooseAttachmentCapsule::Initialize()
{ 
  return inheritAttributesFrom( Capsule::s_TypeName );
}

const MTypeId LooseAttachmentCapsuleChild::s_TypeID( IGL_LOOSE_ATTACHMENT_CAPSULE_CHILD );
const char* LooseAttachmentCapsuleChild::s_TypeName = "igLooseAttachmentCapsuleChild";

MStatus LooseAttachmentCapsuleChild::Initialize()
{
  return inheritAttributesFrom( CapsuleChild::s_TypeName );
}
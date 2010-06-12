#pragma once

//
// Locators
//

#define IGL_COLL_SPHERE                         0x20007   // Collision sphere
#define IGL_COLL_CAPSULE                        0x20008   // Collision capsule
#define IGL_COLL_CAPSULE_CHILD                  0x20009   // Collision capsule child sphere
#define IGL_COLL_CUBOID                         0x2000A   // Collision cuboid
#define IGL_CLOTH_EDGE                          0x2000B   // Edge widget for specifying cloth
#define IGL_CLOTH_EDGE_CHILD                    0x2000C   // Child sphere (allows the edge to have a parent per side)
#define IGL_CENTER_OF_MASS                      0x2000D   // Rigid body's center of mass
#define IGL_TRANSFER_NODE                       0x2000E   // Transfer node... uvs, etc...
#define IGL_COLL_VOLUME                         0x2000F   // Base class for volume types
#define IGL_COLL_CYLINDER                       0x20010   // Collision cylinder
#define IGL_JOINT_EFFECTOR                      0x20015   // Collision sphere
#define IGL_COLL_MODE_DISP_NODE_ID              0x2001A   // CollisionModeDisplayNode (locator node to display coll meshs by mode)
#define IGL_DESTRUCTION_GLUE                    0x2001B   // Destruction Glue (binds bangles together for destruction sim) -ec
#define IGL_DESTRUCTION_PIN                     0x2001C   // Destruction Pin (binds bangle to the world) -ec
#define IGT_NAV_SPHERE_ID                       0x2001D   // nav sphere efffector
#define IGT_NAV_CYLINDER_ID                     0x2001E   // nav Cylinder efffector
#define IGT_NAV_CUBOID_ID                       0x2001F   // nav cuboid efffector
#define IGL_FXEMITTER_NODE_ID                   0x20022   // FX Emitter node
#define IGL_GAMEOBJECTEMITTER_NODE_ID           0x20024   // Game Object Emitter node
#define IGL_REFERENCETRANSFORM_ID               0x20025   // ReferenceTransform id
#define IGL_NAV_CLUE_CUBOID_ID                  0x20026   // Nav clue cuboid
#define IGL_LIGHTINGBATCHSET_ID                 0x20027   // Lighting batch set
#define IGL_GLASSNODE_ID                        0x20028   // Glass transfrom
#define IGL_EXPORTNODE_ID                       0x2002B   // Export Node
#define IGL_EXPORTNODESET_ID                    0x2002C   // Export Node Set
#define IGL_CAMERA_CONTROLLER_ID                0x2002D   // igCameraControllerNode
#define IGL_ENTITYNODE_ID                       0x2002E   // MayaNodes.mll
#define IGT_ENTITYASSETNODE_ID                  0x2002F   // MayaNodes.mll
#define IGT_ENTITYGROUPNODE_ID                  0x20031   // MayaNodes.mll
#define IGL_EFFECTSPECPLAYER_NODE_ID            0x20032   // Effect Spec Player node
#define IGL_COLL_CYLINDER_CHILD                 0x20033   // Collision cylinder child
#define IGL_PLANE                               0x20034   // Plane
#define IGL_WATER_PLANE                         0x20035   // Water plane
#define IGL_MATERIAL_DISP_NODE_ID               0x20036   // MaterialDisplayNode (locator node to display coll meshs by materials)
#define IGL_LOOSE_ATTACHMENT_SPHERE             0x20037   // Loose Attachment Collision Sphere
#define IGL_LOOSE_ATTACHMENT_CAPSULE            0x20038   // Loose Attachment Collision Capsule
#define IGL_LOOSE_ATTACHMENT_CAPSULE_CHILD      0x20039   // Loose Attachment Collision Capsule Child Sphere
#define IGL_CUBOID                              0x2003A   // Basic cuboid
#define IGL_SPHERE                              0x2003B   // Basic sphere
#define IGL_CYLINDER                            0x2003C   // Basic cylinder
#define IGL_CYLINDER_CHILD                      0x2003D   // Basic cylinder child
#define IGL_CAPSULE                             0x2003E   // Basic capsule
#define IGL_CAPSULE_CHILD                       0x2003F   // Basic capsule child
#define IGL_GP_CUBOID                           0x20040   // Gameplay cuboid
#define IGL_GP_SPHERE                           0x20041   // Gameplay sphere
#define IGL_GP_CYLINDER                         0x20042   // Gameplay cylinder
#define IGL_GP_CAPSULE                          0x20043   // Gameplay capsule
#define IGL_SETUP_LIGHTING_NODE                 0x20044   // Setup Lighting node (stores certain state pertaining to setup lighting plugin in file)
//
// Blind data
//

#define SEGMENT_BLIND_DATA_ID           1001  // Segment flagging blind data template id 
#define MATERIAL_BLIND_DATA_ID          1002  // Material flagging blind data template id 

//
// Registration
//

#define REGISTER_NODE( codeType, nodeType )                               \
{                                                                         \
  MString name( codeType::s_TypeName );                                   \
  MStatus localStatus = plugin.registerNode( name,                        \
    codeType::s_TypeID,                                                   \
    &codeType::Creator,                                                   \
    &codeType::Initialize,                                                \
    MPxNode::nodeType );                                                  \
  if ( !localStatus )                                                     \
  {                                                                       \
    localStatus.perror( MString( "Failed to register node " ) + name );   \
    return localStatus;                                                   \
  }                                                                       \
}

#define DEREGISTER_NODE( codeType )                                       \
{                                                                         \
  MString name( codeType::s_TypeName );                                   \
  MStatus localStatus = plugin.deregisterNode( codeType::s_TypeID );      \
  if ( !localStatus )                                                     \
  {                                                                       \
    localStatus.perror( MString( "Failed to deregister node " ) + name ); \
    return localStatus;                                                   \
  }                                                                       \
}

#define REGISTER_TRANSFORM( codeType, matrixCreatorPtr, matrixId )             \
{                                                                              \
  MString name( codeType::s_TypeName );                                        \
  MStatus localStatus = plugin.registerTransform( name,                        \
    codeType::s_TypeID,                                                        \
    &codeType::Creator,                                                        \
    &codeType::Initialize,                                                     \
    matrixCreatorPtr,                                                          \
    matrixId );                                                                \
  if ( !localStatus )                                                          \
  {                                                                            \
    localStatus.perror( MString( "Failed to register codeType " ) + name );    \
    return localStatus;                                                        \
  }                                                                            \
}

#define DEREGISTER_TRANSFORM( codeType )                                       \
{                                                                              \
  MString name( codeType::s_TypeName );                                        \
  MStatus localStatus = plugin.deregisterNode( codeType::s_TypeID );           \
  if ( !localStatus )                                                          \
  {                                                                            \
    localStatus.perror( MString( "Failed to deregister codeType " ) + name );  \
    return localStatus;                                                        \
  }                                                                            \
}

#define REGISTER_COMMAND( codeType )                                          \
{                                                                             \
  MString name( codeType::CommandName );                                      \
  MStatus localStatus = plugin.registerCommand( name,                         \
    &codeType::creator,                                                       \
    &codeType::newSyntax );                                                   \
  if ( !localStatus )                                                         \
  {                                                                           \
    localStatus.perror( MString( "Failed to register command " ) + name );    \
    return localStatus;                                                       \
  }                                                                           \
}

#define DEREGISTER_COMMAND( codeType )                                        \
{                                                                             \
  MString name( codeType::CommandName );                                      \
  MStatus localStatus = plugin.deregisterCommand( name );                     \
  if ( !localStatus )                                                         \
  {                                                                           \
    localStatus.perror( MString( "Failed to deregister command " ) + name );  \
    return localStatus;                                                       \
  }                                                                           \
}

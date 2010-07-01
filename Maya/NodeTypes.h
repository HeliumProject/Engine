#pragma once

//
// Nodes, start at 0x20007?
//

#define IGL_COLL_VOLUME                         0x2000F   // Base class for volume types
#define IGL_COLL_CUBOID                         0x2000A   // Collision cuboid
#define IGL_COLL_SPHERE                         0x20007   // Collision sphere
#define IGL_COLL_CAPSULE                        0x20008   // Collision capsule
#define IGL_COLL_CAPSULE_CHILD                  0x20009   // Collision capsule child sphere
#define IGL_COLL_CYLINDER                       0x20010   // Collision cylinder
#define IGL_COLL_CYLINDER_CHILD                 0x20033   // Collision cylinder child

#define IGL_GP_CUBOID                           0x20040   // Gameplay cuboid
#define IGL_GP_SPHERE                           0x20041   // Gameplay sphere
#define IGL_GP_CYLINDER                         0x20042   // Gameplay cylinder
#define IGL_GP_CAPSULE                          0x20043   // Gameplay capsule

#define IGL_CUBOID                              0x2003A   // Basic cuboid
#define IGL_SPHERE                              0x2003B   // Basic sphere
#define IGL_CYLINDER                            0x2003C   // Basic cylinder
#define IGL_CYLINDER_CHILD                      0x2003D   // Basic cylinder child
#define IGL_CAPSULE                             0x2003E   // Basic capsule
#define IGL_CAPSULE_CHILD                       0x2003F   // Basic capsule child

#define IGL_EXPORTNODE_ID                       0x2002B   // Export Node
#define IGL_EXPORTNODESET_ID                    0x2002C   // Export Node Set

#define IGL_ENTITYNODE_ID                       0x2002E   // MayaNodes.mll
#define IGT_ENTITYASSETNODE_ID                  0x2002F   // MayaNodes.mll
#define IGT_ENTITYGROUPNODE_ID                  0x20031   // MayaNodes.mll

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

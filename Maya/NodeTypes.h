#pragma once

//
// Nodes, start at 0x20007?
//

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

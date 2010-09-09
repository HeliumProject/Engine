#include "Precompile.h"
#include "EntityNode.h"

#include "Maya/Duplicate.h"
#include "Maya/Utils.h"

#include "Maya/NodeTypes.h"
#include "Maya/ErrorHelpers.h"

#include "Foundation/Component/ComponentHandle.h"

#include "Foundation/Reflect/Archive.h"
#include "Foundation/Reflect/Element.h"

#include "Core/Asset/Classes/Entity.h"
#include "Core/Content/ContentScene.h"

#include "Export/MayaContentCmd.h"

#include <maya/MSceneMessage.h>
#include <maya/MUserEventMessage.h>
#include <maya/MObjectHandle.h>
#include <maya/MDagModifier.h>

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Component;
using namespace Helium::Asset;

const MTypeId         EntityNode::s_TypeID(IGT_ENTITYASSETNODE_ID);
const char*           EntityNode::s_TypeName( "EntityNode" );

const MTypeId       EntityGroupNode::s_TypeID( IGT_ENTITYGROUPNODE_ID );  
const char*         EntityGroupNode::s_TypeName( "EntityGroupNode" );

// callbacks
MIntArray EntityNode::s_CallbackIDs; 

//attribs
MObject EntityNode::s_ArtFilePath;
MObject EntityNode::m_ImportNodes;

// statics
EntityNode       EntityNode::Null;

MCallbackId           EntityNode::s_EditNodeAddedCBId;
MCallbackId           EntityNode::s_ImportNodeAddedCBId;
M_IdClassTransform    EntityNode::s_ClassTransformsMap;
MObject               EntityNode::s_EntityAssetGroup;
bool                  EntityNode::s_DoRemoveNodeCallback = true;

namespace NodeArrays
{
    enum
    {
        KeepNodes,
        DeleteNodes,
        Count,
    };
}

MStatus EntityNode::AddCallbacks() 
{
    MStatus stat;

    s_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kBeforeNew, EntityNode::FlushCallback, NULL, &stat));
    if (!stat)
        MGlobal::displayError("Unable to add Before New callback EntityNode::FlushCallback for EntityNode.\n");  

    s_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kBeforeOpen, EntityNode::FlushCallback, NULL, &stat));
    if (!stat)
        MGlobal::displayError("Unable to add before open callback EntityNode::FlushCallback for EntityNode.\n"); 

    s_CallbackIDs.append((int)MDGMessage::addNodeAddedCallback( EntityNode::NodeAddedCallback, EntityNode::s_TypeName, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityNode::NodeAddedCallback for EntityNode.\n");

    s_CallbackIDs.append((int)MDGMessage::addNodeRemovedCallback( EntityNode::NodeRemovedCallback, EntityNode::s_TypeName, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityNode::NodeRemovedCallback for EntityNode.\n");

    s_CallbackIDs.append((int)MUserEventMessage::addUserEventCallback( MString( kUnselectInstanceData ), EntityNode::UnselectAllCallback, NULL, &stat));
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityNode::UnselectAllCallback for EntityNode.\n"); 

    return stat; 
}

MStatus EntityNode::RemoveCallbacks()
{
    MStatus stat;

    // Remove all the callbacks
    stat = MMessage::removeCallbacks(s_CallbackIDs);
    if (!stat)
    {
        MGlobal::displayError("Unable to delete callbacks");  
    }

    return MS::kSuccess;
}

EntityNode::EntityNode()
{
}

EntityNode::~EntityNode()
{
}

void * EntityNode::Creator()
{
    return new EntityNode();
}

MStatus EntityNode::Initialize()
{
    MAYA_START_EXCEPTION_HANDLING();

    MStatus stat;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Create a string attribute "ArtFilePath" 
    MFnTypedAttribute tAttr;

    s_ArtFilePath = tAttr.create("ArtFilePath", "afp", MFnData::kString, &stat);
    MCheckErr(stat, "Unable to create attr: ArtFilePath");

    tAttr.setReadable(true);
    tAttr.setWritable(false);

    stat = addAttribute(s_ArtFilePath);
    MCheckErr(stat, "Unable to add attr: ArtFilePath");

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Create an array attribute "ImportNodes"
    MFnMessageAttribute mAttr;

    m_ImportNodes = mAttr.create( "ImortObjects", "ios", &stat );
    MCheckErr(stat, "Unable to create attr: ImportNodes");

    mAttr.setArray( true );

    stat = addAttribute(m_ImportNodes);
    MCheckErr(stat, "Unable to add attr: ImportNodes");

    MAYA_FINISH_EXCEPTION_HANDLING();

    return MS::kSuccess;
}

void EntityNode::postConstructor()
{
    MPlug visibility( thisMObject(), MPxTransform::visibility );

    visibility.setValue( false );
    visibility.setLocked( true );

    EntityNode::s_DoRemoveNodeCallback = true;
}

void EntityNode::LoadAllArt()
{
    M_IdClassTransform::iterator classItor = s_ClassTransformsMap.begin();
    M_IdClassTransform::iterator classEnd  = s_ClassTransformsMap.end();
    for( ; classItor != classEnd; ++classItor )
    {
        classItor->second->LoadArt();
    }
}

void EntityNode::UnloadAllArt()
{
    M_IdClassTransform::iterator classItor = s_ClassTransformsMap.begin();
    M_IdClassTransform::iterator classEnd  = s_ClassTransformsMap.end();
    for( ; classItor != classEnd; ++classItor )
    {
        classItor->second->UnloadArt();
    }
}

void EntityNode::FlattenInstances()
{
    M_IdClassTransform::iterator classItor = s_ClassTransformsMap.begin();
    M_IdClassTransform::iterator classEnd  = s_ClassTransformsMap.end();
    for( ; classItor != classEnd; ++classItor )
    {
        M_EntityNode::iterator itor = classItor->second->m_Instances.begin();
        M_EntityNode::iterator end  = classItor->second->m_Instances.end();
        for( ; itor != end; ++itor )
        {
            itor->second->Flatten();
        }
    }

    MGlobal::deleteNode( EntityInstanceNode::s_EntityNodeGroup );

    EntityInstanceNode::s_EntityNodeGroup = MObject::kNullObj;
}

EntityNode& EntityNode::Get( const Helium::Path& path, bool createIfNotExisting )
{
    MFnDagNode dagFn;

    try
    {
        M_IdClassTransform::iterator findItor = s_ClassTransformsMap.find( path.Hash() );
        if( findItor != s_ClassTransformsMap.end() )
        {
            return *findItor->second;
        }
        else if ( createIfNotExisting )
        {
            // we couldn't find it, so create it and return the loaded art class
            Asset::AssetClassPtr assetClass = Asset::AssetClass::LoadAssetClass( path );

            if ( assetClass.ReferencesObject() )
            {
                tstring artFilePath = assetClass->GetPath().Get();

                MObject classTransform = dagFn.create( EntityNode::s_TypeID, assetClass->GetShortName().c_str() );
                dagFn.setDoNotWrite( true );

                EntityNode* artClass = static_cast<EntityNode*>( dagFn.userNode() );

                artClass->m_AssetPath = path;
                artClass->SetArtFilePath( artFilePath.c_str() );

                s_ClassTransformsMap[ path.Hash() ] = artClass;
                artClass->LoadArt();

                return *artClass;
            }
        }
    }
    catch (Helium::Exception& )
    {
        if ( createIfNotExisting )
        {
            MGlobal::displayError( MString("Unable to create EntityNode!") );
        }
    }

    return EntityNode::Null;
}

std::pair< EntityNode*, EntityInstanceNode*> EntityNode::CreateInstance( const Content::EntityInstancePtr& entity )
{
    EntityNode* artClass = &Get( entity->GetEntity()->GetPath() );
    M_EntityNode::iterator instItor = artClass->m_Instances.find( entity->m_ID );

    EntityInstanceNode* entityNode = NULL;
    if( instItor == artClass->m_Instances.end() )
    {
        MFnDagNode nodeFn;
        MObject instanceObject = nodeFn.create( EntityInstanceNode::s_TypeID, entity->GetName().c_str() );
        nodeFn.setDoNotWrite( true );

        entityNode = static_cast< EntityInstanceNode* >( nodeFn.userNode() );

        artClass->m_Instances[ entity->m_ID ] = entityNode;
        entityNode->SetBackingEntity( entity );
        entityNode->Show( *artClass );
    }
    else
    {
        entityNode = instItor->second;
        entityNode->SetBackingEntity( entity );
    }

    return std::pair< EntityNode*, EntityInstanceNode* >( artClass, entityNode );
}

void EntityNode::RemoveInstance( EntityInstanceNode* entityNode )
{
    EntityNode& artClass = EntityNode::Get( entityNode->GetEntity()->GetEntity()->GetPath(), false );

    if ( artClass != EntityNode::Null )
    {
        artClass.m_Instances.erase( entityNode->GetUID() );
    }
}

void EntityNode::GetArtFilePath( MString& artFilePath )
{
    MPlug plug( thisMObject(), s_ArtFilePath );
    plug.getValue( artFilePath );
}

void EntityNode::SetArtFilePath( const MString& artFilePath )
{
    MPlug plug( thisMObject(), s_ArtFilePath );
    plug.setValue( MString(artFilePath.asTChar()) );
}

void EntityNode::LoadArt()
{
    MStatus stat;
    bool callbackRemoved = false;
    if( s_EditNodeAddedCBId != -1 )
    {
        MDGMessage::removeCallback( s_EditNodeAddedCBId );
        s_EditNodeAddedCBId = -1;
        callbackRemoved = true;
    }

    UnloadArt();

    MString artFilePath;
    GetArtFilePath( artFilePath );
    std::cout << "Importing from: " << artFilePath.asTChar() << "..." << std::endl;

    // add the callback to catch all the imported objects
    MObjectArray objectArrays[NodeArrays::Count];
    s_ImportNodeAddedCBId = MDGMessage::addNodeAddedCallback( ImportNodeAddedCallback, kDefaultNodeType, &objectArrays );

    // import stuff
    MFileIO::import( artFilePath, NULL, false );

    // remove the callback
    MDGMessage::removeCallback( s_ImportNodeAddedCBId );

    // add all imported stuff under this guy's transform
    MFnDagNode nodeFn( thisMObject() );
    for( u32 i=0; i<objectArrays[NodeArrays::KeepNodes].length(); ++i )
    {
        if( objectArrays[NodeArrays::KeepNodes][i].isNull() )
        {
            continue;
        }

        MObjectHandle handle( objectArrays[NodeArrays::KeepNodes][i] );
        if ( !handle.isValid() )
        {
            continue;
        }

        // We should be getting only MFnDagNode in here, but just in case
        MFnDagNode dagFn( objectArrays[NodeArrays::KeepNodes][i], &stat );
        if ( stat == MStatus::kSuccess )
        {
            stat = dagFn.setDoNotWrite( true );
        }
        else
        {
            MFnDependencyNode depNodeFn( objectArrays[NodeArrays::KeepNodes][i], &stat );
            stat = depNodeFn.setDoNotWrite( true );
        }

#ifdef _DEBUG
        MString name = dagFn.name();
        MString type = dagFn.typeName();
        std::string nodeTypeStr( objectArrays[NodeArrays::KeepNodes][i].apiTypeStr() );

        if ( name.length() )
        {
            std::cout << " - Importing Child: " << name.asTChar() << " " << type.asTChar() << "(" << nodeTypeStr << ")" << std::endl;
        }
#endif

        AddImportNode( objectArrays[NodeArrays::KeepNodes][i] );   
        nodeFn.addChild( objectArrays[NodeArrays::KeepNodes][i] );
    }

    for( u32 i=0; i<objectArrays[NodeArrays::DeleteNodes].length(); ++i )
    {
        MGlobal::deleteNode( objectArrays[NodeArrays::DeleteNodes][i] );
    }

    MFnDagNode otherFn;
    for( u32 i=0 ; i<nodeFn.childCount(); ++i )
    {
        MObject child = nodeFn.child( i );
        if( !child.hasFn( MFn::kDagNode ) ) 
        {
            continue;
        }

        otherFn.setObject( child );
        if( otherFn.typeId() == EntityNode::s_TypeID )
        {
            MDagModifier mod;
            u32 numChild = otherFn.childCount();
            for( u32 j = 0; j < numChild; ++j )
            {
                stat = mod.reparentNode( otherFn.child( j ), thisMObject() );
            }
            mod.doIt();

            MGlobal::deleteNode( child );
            break;
        }
    }

    std::cout << "Done" << std::endl;

}

void EntityNode::UnloadArt()
{
    MStatus stat;

    // remove anything currently imported/referenced
    MFnDagNode dagFn( thisMObject() );
    while( dagFn.childCount() )
    {
        stat = MGlobal::deleteNode( dagFn.child( 0 ) );
        MContinueErr( stat, ("Unable to delete" + dagFn.fullPathName() ).asChar() );
    }

    ClearInstances();

    MObjectArray forDelete;
    GetImportNodes( forDelete );
    MFnDependencyNode depFn;

    u32 num = forDelete.length();
    for( u32 i = 0; i < num; ++i )
    {
        MObject& node = forDelete[i];

        MObjectHandle handle( node );
        if( !handle.isValid() )
            continue;

        depFn.setObject( node );

        stat = MGlobal::deleteNode( node );
        MContinueErr( stat, ("Unable to delete" + depFn.name() ).asChar() );
    }

    forDelete.clear();
}

void EntityNode::AddImportNode( const MObject& object )
{
    MStatus stat;
    MObjectHandle handle( object );
    if( !handle.isValid() )
    {
        return;
    }

    MFnDependencyNode nodeFn( object, &stat );
    MCheckNoErr( stat, "Unable to create depenency node fn set" );
    nodeFn.setDoNotWrite( true );

    //MItDependencyGraph childrenIt (MItDependencyGraph::kUpstream);
    //for ( childrenIt.reset( nodeFn.object() ); !childrenIt.isDone(); childrenIt.next() )
    //{
    //  MFnDependencyNode( childrenIt.item() ).setDoNotWrite( true );
    //}

    MString name = nodeFn.name( &stat );
    MCheckNoErr( stat, "Unable to get node name" );

    if( name == "" )
    {
        return;
    }

    // don't connect dag nodes to the ImportNodes attributeb array
    if( object.hasFn( MFn::kDagNode ) )
    {
        return;
    }

#ifdef _DEBUG
    std::cout << "Adding: " << name.asTChar() << std::endl;
#endif

    //create ImportMessage attrib on the imported object if necessary
    if( !nodeFn.hasAttribute( "ImportMessage", &stat ) )
    {
        MFnMessageAttribute mAttr;
        MObject importMessage = mAttr.create( "ImportMessage", "imp", &stat );
        MCheckNoErr(stat, "Unable to create attr: ImportMessage");

        stat = nodeFn.addAttribute( importMessage );
        MCheckNoErr(stat, "Unable to add attr: ImportNodes"); 
    }

    MPlug importMsg = nodeFn.findPlug( "ImportMessage", &stat );
    MCheckNoErr(stat, "Unable to find attr: ImportMessage");

    MPlug importPlug( thisMObject(), m_ImportNodes );
    u32 currentIdx = importPlug.numConnectedElements();
    MPlug importElement = importPlug.elementByLogicalIndex( currentIdx );

    MDGModifier mod;

    //if it's currently connected, disconnect it
    if( importMsg.isConnected() )
    {
        MPlugArray plugs;
        importMsg.connectedTo( plugs, true, false );
        if( plugs.length() == 1 )
        {
            mod.disconnect( importMsg, plugs[0] );
        }
    }
    stat = mod.connect( importMsg, importElement );
    mod.doIt();
}

void EntityNode::GetImportNodes( MObjectArray& objects )
{
    MPlug plug(thisMObject(), m_ImportNodes);
    u32 num = plug.numElements();
    for( u32 i = 0; i < num; ++i )
    {
        MPlug elementPlug = plug.elementByLogicalIndex( i );
        if( elementPlug.isConnected() )
        {
            MPlugArray plugs;
            elementPlug.connectedTo( plugs, true, false );

            //should be one and only one
            HELIUM_ASSERT( plugs.length() == 1 );
            objects.append ( plugs[0].node() );
        }
    }
}

void EntityNode::AddToInstances( MObject &addedNode )
{
    MStatus stat;

    MFnDagNode instanceFn;
    MFnDagNode nodeFn( addedNode );
    MDagPath source;
    nodeFn.getPath( source );

    M_EntityNode::iterator itor = m_Instances.begin();
    M_EntityNode::iterator end  = m_Instances.end();
    for( ; itor != end; ++itor)
    {
        instanceFn.setObject( itor->second->thisMObject() );

        instanceFn.setObject( instanceFn.parent( 0 ) );
        MDagPath parent;
        instanceFn.getPath( parent );

        MDagPath result;
        Maya::duplicate( source, parent, result, true );
    }
}

void EntityNode::ClearInstances()
{
    MStatus stat;

    M_EntityNode::iterator itor = m_Instances.begin();
    M_EntityNode::iterator end  = m_Instances.end();
    for( ; itor != end; ++itor)
    {
        itor->second->Hide();    
    }
}

void EntityNode::ReloadInstances()
{
    MFnDagNode nodeFn( thisMObject() );
    u32 len = nodeFn.childCount();
    for( u32 i = 0; i < len; ++i )
    {
        AddToInstances( nodeFn.child( i ) );
    }
}

void EntityNode::Unselect( MSelectionList& list )
{
    MObjectArray importNodes;
    GetImportNodes( importNodes );

    u32 len = importNodes.length();
    for( u32 i = 0; i < len; ++i )
    {
        MGlobal::unselect( importNodes[i] );
    }
}

void EntityNode::UnselectAll( MSelectionList& list )
{
    MStatus status;
    M_IdClassTransform::iterator classItor = s_ClassTransformsMap.begin();
    M_IdClassTransform::iterator classEnd  = s_ClassTransformsMap.end();
    for( ; classItor != classEnd; ++classItor )
    {
        classItor->second->Unselect( list ); 
    }  

    Maya::RemoveHierarchy( EntityNode::s_EntityAssetGroup, list );
    Maya::RemoveHierarchy( EntityInstanceNode::s_EntityNodeGroup, list );
}

void EntityNode::FlushCallback( void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    MDGMessage::removeCallback( s_EditNodeAddedCBId );
    s_ClassTransformsMap.clear();

    s_EntityAssetGroup = MObject::kNullObj;
    EntityInstanceNode::s_EntityNodeGroup = MObject::kNullObj;
    EntityInstanceNode::s_ReplaceSelection = false;
    EntityNode::s_DoRemoveNodeCallback = false;
    EntityInstanceNode::s_RelativeTransform = Math::Matrix4::Identity;

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::NodeAddedCallback( MObject& node, void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    if( MFileIO::isReadingFile() )
    {
        return;
    }

    //
    // For some reason, if you do this in the postContructor, it doesn't work!
    // (the new node doesn't get parented to the s_EntityAssetGroup)
    //
    MFnDagNode nodeFn( s_EntityAssetGroup );
    if(!MObjectHandle(s_EntityAssetGroup).isValid())
    {
        s_EntityAssetGroup = nodeFn.create( EntityGroupNode::s_TypeID, "EntityAssetGroup" );
        nodeFn.setDoNotWrite( true );
    }

    nodeFn.addChild( node );

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::NodeRemovedCallback( MObject& node, void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    if( !s_DoRemoveNodeCallback )
    {
        return;
    }

    s_DoRemoveNodeCallback = false;
    {
        MFnDependencyNode nodeFn( node );

        EntityNode* artClass = (EntityNode*)(nodeFn.userNode());
        artClass->UnloadArt();

        M_EntityNode::iterator itor = artClass->m_Instances.begin();
        M_EntityNode::iterator end  = artClass->m_Instances.end();
        for( ; itor != end; ++itor)
        {
            MGlobal::deleteNode( itor->second->thisMObject() );
        }
        artClass->m_Instances.clear();

        s_ClassTransformsMap.erase( artClass->m_AssetPath.Hash() );
    }
    s_DoRemoveNodeCallback = true;

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::ImportNodeAddedCallback( MObject &addedNode, void* param )
{
    MAYA_START_EXCEPTION_HANDLING();

    MObjectArray (*nodeArrays)[NodeArrays::Count] = (MObjectArray (*)[NodeArrays::Count])param;


    MObjectHandle handle( addedNode );
    if( !handle.isValid() ) 
    {
        return;
    }

    MFnDependencyNode nodeFn( addedNode );
    nodeFn.setDoNotWrite( true ); 

    // don't gather certain nodes
    if( addedNode.hasFn( MFn::kReference ) ||
        addedNode.hasFn( MFn::kPartition ) ||
        addedNode.hasFn( MFn::kDisplayLayerManager ) ||
        addedNode.hasFn( MFn::kDisplayLayer ) ||
        addedNode.hasFn( MFn::kRenderLayerManager ) ||
        addedNode.hasFn( MFn::kRenderLayer ) ||
        addedNode.hasFn( MFn::kScript ) ||
        addedNode.hasFn( MFn::kLightLink ) )
    {
        return;
    }

    // don't gather reference objects
    if( nodeFn.isFromReferencedFile() )
    {
        return;
    }

    // don't gather maya-designated "shared" nodes
    if( nodeFn.isShared() )
    {
        return;
    }

#ifdef _DEBUG
    MString name = nodeFn.name();
    MString type = nodeFn.typeName();
    std::cout << " - Importing: " << name.asTChar() << " " << type.asTChar() << std::endl;
#endif

    nodeArrays[ NodeArrays::KeepNodes ]->append( addedNode );

    MAYA_FINISH_EXCEPTION_HANDLING();
}



void EntityNode::UnselectAllCallback( void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    EntityNode::UnselectAll( *(MSelectionList*)(clientData) );

    MAYA_FINISH_EXCEPTION_HANDLING();
}

#include "Precompile.h"
#include "EntityInstanceNode.h"
#include "EntityNode.h"

#include "Platform/Types.h"
#include "Maya/NodeTypes.h"
#include "Maya/ErrorHelpers.h"

#include "Foundation/TUID.h"
#include "Foundation/Math/EulerAngles.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Maya/Duplicate.h"
#include "Maya/Utils.h"

#include <maya/MPointArray.h>
#include <maya/MModelMessage.h>
#include <maya/MFnStringData.h>
#include <maya/MObjectHandle.h>

using namespace Helium;
using namespace Helium::Asset;
using namespace Helium::Component;

// enable this to watch all dag changes, just for debugging
//#define WATCH_ALL_DAG_CHANGES

const MTypeId       EntityInstanceNode::s_TypeID(IGL_ENTITYNODE_ID);
const char*         EntityInstanceNode::s_TypeName("EntityInstanceNode");

// callbacks
MIntArray           EntityInstanceNode::s_CallbackIDs; 

// attributes 
MObject             EntityInstanceNode::s_ArtFilePath;

// statics
MPointArray         EntityInstanceNode::s_PointerPoints;
MColor              EntityInstanceNode::s_DrawColor(0,.2f,1);
bool                EntityInstanceNode::s_PointerYUp;
Math::Matrix4       EntityInstanceNode::s_RelativeTransform;
V_EntityNodePtr     EntityInstanceNode::s_ShowNodes;
V_EntityNodePtr     EntityInstanceNode::s_DupeNodes;
MObject             EntityInstanceNode::s_EntityNodeGroup;
bool                EntityInstanceNode::s_ReplaceSelection = false;

MStatus EntityInstanceNode::AddCallbacks() 
{
    MStatus stat;

    s_CallbackIDs.append((int)MDGMessage::addNodeAddedCallback( EntityInstanceNode::NodeAddedCallback, EntityInstanceNode::s_TypeName, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityInstanceNode::NodeAddedCallback for EntityNode.\n");

    s_CallbackIDs.append((int)MDGMessage::addNodeRemovedCallback( EntityInstanceNode::NodeRemovedCallback, EntityInstanceNode::s_TypeName, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityInstanceNode::NodeRemovedCallback for EntityNode.\n");

    s_CallbackIDs.append((int)MModelMessage::addAfterDuplicateCallback( EntityInstanceNode::AfterDuplicateCallback, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityInstanceNode::AfterDuplicateCallback for EntityNode.\n");

    s_CallbackIDs.append((int)MModelMessage::addCallback( MModelMessage::kActiveListModified, EntityInstanceNode::SelectionChangedCallback, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityInstanceNode::SelectionChangedCallback for EntityNode.\n");

#ifdef WATCH_ALL_DAG_CHANGES

    s_CallbackIDs.append( MDagMessage::addAllDagChangesCallback( EntityInstanceNode::DagChangesCallback, NULL, &stat ) );
    if (!stat)
        MGlobal::displayError("Unable to add user callback EntityInstanceNode::DagChangesCallback for EntityNode.\n");

#endif

    return MS::kSuccess; 
}

MStatus EntityInstanceNode::RemoveCallbacks()
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

EntityInstanceNode::EntityInstanceNode() 
: m_UID( TUID::Null )
, m_AttributeChangedCB( -1 )
, m_ChildAddedCB( -1 )
{

}

EntityInstanceNode::~EntityInstanceNode()
{
    MNodeMessage::removeCallback( m_AttributeChangedCB );
    m_Entity = NULL;
}

void* EntityInstanceNode::Creator()
{
    return new EntityInstanceNode();
}

MStatus EntityInstanceNode::Initialize()
{
    MAYA_START_EXCEPTION_HANDLING();

    MStatus stat;

    // wtf is this - rachel
    MFnStringData dataFn;
    MObject stringData = dataFn.create( "" );

    // fileName attribute
    MFnTypedAttribute tAttr;
    s_ArtFilePath = tAttr.create("ArtFilePath", "fn", MFnData::kString, &stat);
    tAttr.setDefault( stringData );
    MCheckErr(stat, "Unable to create attr: ArtFilePath");

    tAttr.setReadable(true);
    tAttr.setWritable(false);

    stat = addAttribute(s_ArtFilePath);
    MCheckErr(stat, "Unable to add attr: ArtFilePath");

    MAYA_FINISH_EXCEPTION_HANDLING();

    return MS::kSuccess;
}

void EntityInstanceNode::postConstructor() 
{
    MPlug plug ( thisMObject(), s_ArtFilePath );

    m_Plug = plug;

    s_ReplaceSelection = true;
}

void EntityInstanceNode::copyInternalData( MPxNode* node )
{
    MAYA_START_EXCEPTION_HANDLING();

    EntityInstanceNode* source = (EntityInstanceNode*)(node);
    source->Hide();
    m_Entity = Reflect::ObjectCast< Asset::EntityInstance >( source->m_Entity->Clone() );
    TUID::Generate( m_Entity->m_ID );
    m_UID = m_Entity->m_ID;

    EntityNode& instanceClassNode = EntityNode::Get( m_Entity->GetEntity()->GetPath() );
    if( instanceClassNode == EntityNode::Null )
    {
        return;
    }

    instanceClassNode.m_Instances.insert( std::pair< TUID, EntityInstanceNode*>( m_UID, this ) );

    s_ShowNodes.push_back( source );
    s_DupeNodes.push_back( this ); 

    MAYA_FINISH_EXCEPTION_HANDLING();
}

bool EntityInstanceNode::isBounded() const 
{ 
    return false;
}

bool EntityInstanceNode::isAbstractClass() const 
{
    return false;
}

bool EntityInstanceNode::excludeAsLocator() const 
{
    return false;
}

MStatus EntityInstanceNode::compute(const MPlug& plug, MDataBlock& data) 
{ 
    return MS::kUnknownParameter;
}

void EntityInstanceNode::draw( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status ) 
{ 
    MStatus stat;

    // Check whether need to redo points
    if (s_PointerPoints.length())
    {
        if (s_PointerYUp != MGlobal::isYAxisUp())
        {
            // Store the up axis
            s_PointerYUp = MGlobal::isYAxisUp();

            // Points are for Y-UP
            float pointerVerts[][4] =
            {
                {-200, -100,   0, 1},
                {-200,    0, 100, 1},
                {-200,    0, 100, 1},
                {-200,  100,   0, 1},
                {-200,  100,   0, 1},
                {   0,    0,   0, 1},
                {   0,    0,   0, 1},
                {-200, -100,   0, 1},
                {-200,    0, 100, 1},
                {   0,    0,   0, 1},
                {-200, -100,   0, 1},
                {-200,  100,   0, 1}
            };

            s_PointerPoints = MPointArray(pointerVerts, 12); 

            // Flip the points if Y-Up
            if (s_PointerYUp) 
            {
                for(unsigned i = 0; i < s_PointerPoints.length(); ++i) 
                {
                    MPoint p = s_PointerPoints[i];
                    s_PointerPoints[i].x = p.y;
                    s_PointerPoints[i].y = p.z;
                    s_PointerPoints[i].z = p.x;
                }
            }
        }
    }

    // Go ahead and draw pointer selected if this instance is selected
    switch (status)
    {
    case M3dView::kDormant:
        {
            stat = view.setDrawColor(s_DrawColor);
            MErr(stat, "Unable to do: view.setDrawColor");
            break;
        }

    case M3dView::kTemplate:
        {
            MColor templateColor = view.templateColor(&stat);
            MErr(stat, "Unable to do: view.templateColor");
            stat = view.setDrawColor(templateColor);
            MErr(stat, "Unable to do: view.setDrawColor");
            break;
        }

    default:
        {
            stat = view.setDrawColor(15, M3dView::kActiveColors);
            MErr(stat, "Unable to do: view.setDrawColor"); 
            break;
        }
    }

    glPushMatrix();
    {
        glBegin( GL_LINES );
        {
            u32 len = s_PointerPoints.length();
            for( unsigned i = 0; i < len; i += 2 ) 
            {
                glVertex3f((GLfloat)s_PointerPoints[i].x,   (GLfloat)s_PointerPoints[i].y,   (GLfloat)s_PointerPoints[i].z);
                glVertex3f((GLfloat)s_PointerPoints[i+1].x, (GLfloat)s_PointerPoints[i+1].y, (GLfloat)s_PointerPoints[i+1].z);
            }
        }
        glEnd();
    }
    glPopMatrix();
}

void EntityInstanceNode::SetBackingEntity( const Asset::EntityInstancePtr& entity )
{
    m_Entity = entity;
    m_UID = m_Entity->m_ID;

    m_Plug.setAttribute( s_ArtFilePath );
    m_Plug.setValue( MString (entity->GetEntity()->GetPath().c_str()) );

    Math::Matrix4 gm = m_Entity->m_GlobalTransform * s_RelativeTransform.Inverted();
    MMatrix mat;

    gm.t.x *= 100.0f;
    gm.t.y *= 100.0f;
    gm.t.z *= 100.0f;

    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            mat[i][j] = gm[i][j];
        }
    }

    MFnTransform transFn( thisMObject() );
    MTransformationMatrix transMat (mat);
    transFn.set(transMat);

    AddAttributeChangedCallback();
}

void EntityInstanceNode::UpdateBackingEntity()
{
    MFnTransform transFn( thisMObject() );
    MMatrix mat = transFn.transformationMatrix();

    mat[3][0] *= 0.01;
    mat[3][1] *= 0.01;
    mat[3][2] *= 0.01;

    for (int i=0; i<4; i++)
    {
        for (int j=0; j<4; j++)
        {
            m_Entity->m_GlobalTransform[i][j] = (float)mat[i][j];
        }
    }
    m_Entity->m_GlobalTransform *= s_RelativeTransform;

    // there can be no hierarchy in the Scratch Pad scene, so this should be fine
    Math::Scale scale;
    Math::Vector3 translate;
    Math::EulerAngles rotate;
    m_Entity->m_GlobalTransform.Decompose( scale, rotate, translate );

    m_Entity->m_Scale.x = scale.x;
    m_Entity->m_Scale.y = scale.y;
    m_Entity->m_Scale.z = scale.z;

    m_Entity->m_Rotate.x = rotate.angles.x;
    m_Entity->m_Rotate.y = rotate.angles.y;
    m_Entity->m_Rotate.z = rotate.angles.z;

    m_Entity->m_Translate = translate;

    m_Entity->m_ObjectTransform = m_Entity->m_GlobalTransform;
}

void EntityInstanceNode::GetArtFilePath( MString& artFilePath )
{
    m_Plug.setAttribute( s_ArtFilePath );
    m_Plug.getValue( artFilePath );
}

void EntityInstanceNode::Show( const EntityNode& instanceClassNode )
{
    if( instanceClassNode == EntityNode::Null )
        return;

    MFnDagNode instanceFn( thisMObject() );
    MDagPath path;
    instanceFn.getPath( path );

    MFnDagNode nodeFn( instanceClassNode.thisMObject() );
    u32 len = nodeFn.childCount();

    for( u32 i = 0; i < len; ++i )
    {
        MFnDagNode nodeFn( nodeFn.child( i ) );

        MDagPath child;
        nodeFn.getPath( child );
        MDagPath result;
        Maya::duplicate( child, path, result, true, true );
    }
}

void EntityInstanceNode::Show()
{
    EntityNode& instanceClassNode = EntityNode::Get( m_Entity->GetEntity()->GetPath() );

    Show( instanceClassNode );
}

void EntityInstanceNode::Hide()
{
    MFnTransform transformFn( thisMObject() );

    u32 len = transformFn.childCount();

    MFnDagNode nodeFn;
    for( u32 i = 0; i < len; ++i )
    {
        nodeFn.setObject( transformFn.child( 0 ) );
        MDagPath path;
        nodeFn.getPath( path );        
        MGlobal::deleteNode( path.node() );
    }
}

void EntityInstanceNode::Flatten()
{
    EntityNode& instanceClassNode = EntityNode::Get( m_Entity->GetEntity()->GetPath() );

    if( instanceClassNode == EntityNode::Null )
        return;

    MFnDagNode instanceFn( thisMObject() );

    MFnDagNode nodeFn( instanceClassNode.thisMObject() );
    u32 len = nodeFn.childCount();

    MDagPath path;
    MFnTransform newTransformFn;
    MObject newTransform = newTransformFn.create();

    MTransformationMatrix matrix = instanceFn.transformationMatrix();
    newTransformFn.set( matrix );
    newTransformFn.getPath( path );

    for( u32 i = 0; i < len; ++i )
    {
        MFnDagNode nodeFn( nodeFn.child( i ) );

        MDagPath child;
        nodeFn.getPath( child );
        MDagPath result;

        Maya::duplicate( child, path, result, false, false );
    }

    Maya::LockHierarchy( thisMObject(), false );
}

void EntityInstanceNode::Unselect( MSelectionList& list )
{
    Maya::RemoveHierarchy( thisMObject(), list );
}

void EntityInstanceNode::NodeAddedCallback( MObject& node, void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    if( MFileIO::isReadingFile() )
    {
        return;
    }

    //
    // This is because for some reason, if you do this in the postContructor, it doesn't work!
    //
    MFnDagNode nodeFn( s_EntityNodeGroup );

    //I swapped this check to see if the s_EntityNodeGroup is still valid by its handle, instead
    //of just checking if the object is null because if the object gets deleted, it won't be Null
    //but it will be fucked and crash Maya.
    if( !MObjectHandle( s_EntityNodeGroup ).isValid() )
    {
        s_EntityNodeGroup = nodeFn.create( EntityGroupNode::s_TypeID, "EntityNodeGroup" );
        nodeFn.setDoNotWrite( true );
    }

    nodeFn.addChild( node );

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityInstanceNode::NodeRemovedCallback( MObject& node, void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    if( !EntityNode::s_DoRemoveNodeCallback )
    {
        return;
    }

    MFnDependencyNode nodeFn( node );

    EntityNode::RemoveInstance( (EntityInstanceNode*)(nodeFn.userNode()) );

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityInstanceNode::AddAttributeChangedCallback()
{
    if( m_AttributeChangedCB == -1 )
    {
        m_AttributeChangedCB = MNodeMessage::addAttributeChangedCallback( thisMObject(), AttributeChangedCallback );
    } 
}

void EntityInstanceNode::RemoveAttributeChangedCallback()
{
    if( m_AttributeChangedCB != -1 )
    {
        MNodeMessage::removeCallback( m_AttributeChangedCB );
        m_AttributeChangedCB = -1;
    }
}

void EntityInstanceNode::AttributeChangedCallback( MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityInstanceNode::AfterDuplicateCallback( void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    V_EntityNodePtr::iterator itor = s_ShowNodes.begin();
    V_EntityNodePtr::iterator end  = s_ShowNodes.end();
    for( ; itor != end; ++itor )
    {
        (*itor)->Show();
    }

    itor = s_DupeNodes.begin();
    end  = s_DupeNodes.end();
    for( ; itor != end; ++itor )
    {
        (*itor)->Show();
        (*itor)->AddAttributeChangedCallback();
    }

    s_ShowNodes.clear();
    s_DupeNodes.clear();

    MAYA_FINISH_EXCEPTION_HANDLING();
}

static MObject EntityNodeParent( MDagPath& path )
{
    if( path.hasFn( MFn::kDagNode ) )
    {
        MDagPath parentPath;
        MFnDependencyNode nodeFn;

        while( path.pop( 1 ) != MS::kInvalidParameter )
        {        
            nodeFn.setObject( path.node() );
            if( nodeFn.typeId() == EntityInstanceNode::s_TypeID )    
            {
                return nodeFn.object();
            }
        }
    }

    return MObject::kNullObj;
}

void EntityInstanceNode::SelectionChangedCallback( void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    if( !s_ReplaceSelection )
    {
        return;
    }

    static bool inFunc = false;

    if( inFunc )
        return;
    else
        inFunc = true;

    MSelectionList list;
    MGlobal::getActiveSelectionList( list );

    MStatus stat;

    MSelectionList addList;
    bool added = false;
    u32 len = list.length();
    for( u32 i = 0; i < len; )
    {
        MDagPath path;
        list.getDagPath( i, path );

        MObject entityNode = EntityNodeParent( path );
        if( entityNode != MObject::kNullObj )
        {
            added = true;
            MFnDagNode nodeFn( entityNode );
            MDagPath instancePath;
            nodeFn.getPath( instancePath );
            addList.add( instancePath );
            list.remove( i );
        }
        else
        {
            ++i;
        }
    }

    if( added )
    {
        MGlobal::setActiveSelectionList( list, MGlobal::kReplaceList );
        MGlobal::setActiveSelectionList( addList, MGlobal::kAddToList );
    }

    inFunc = false;

    MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityInstanceNode::AllDagChangesCallback( MDagMessage::DagMessage dagMsg, MDagPath& child, MDagPath &parent, void* clientData )
{
    MAYA_START_EXCEPTION_HANDLING();

    MString message;
    switch( dagMsg )
    {
    case MDagMessage::kParentAdded:
        message += "kParentAdded --";
        break;
    case MDagMessage::kParentRemoved:
        message += "kParentRemoved --";
        break;
    case MDagMessage::kChildAdded:
        message += "kChildAdded --";
        break;
    case MDagMessage::kChildRemoved:
        message += "kChildRemoved --";
        break;
    case MDagMessage::kChildReordered:
        message += "kChildReordered --";
        break;
    case MDagMessage::kInstanceRemoved:
        message += "kInstanceRemoved --";
        break;
    default:
        message += "Some crap --";
    }

    message += " parent: ";
    message += parent.fullPathName();
    message += " child: ";
    message += child.fullPathName();
    MGlobal::displayInfo( message );

    MAYA_FINISH_EXCEPTION_HANDLING();
}

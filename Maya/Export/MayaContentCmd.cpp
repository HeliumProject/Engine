#include "Precompile.h"
#include "ExportAnimation.h"
#include "ExportPivotTransform.h"
#include "ExportEntity.h"
#include "ExportCurve.h"
#include "ExportJoint.h"
#include "ExportMesh.h"
#include "MayaContentCmd.h"

#include "Platform/Windows/Debug.h"
#include "Foundation/Container/BitArray.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Application/RCS/RCS.h"
#include "Application/Exception.h"
#include "Pipeline/Content/ContentVersion.h"
#include "Pipeline/Content/Nodes/Curve.h"
#include "Maya/Utils.h"

using namespace Reflect;
using namespace Content;
using namespace Symbol;
using namespace MayaContent;
using namespace Nocturnal;

static const char* ExportSelectionFlag = "-es";
static const char* ExportSelectionFlagLong  = "-exportSelection";

static const tchar* s_ProxyRigSuffix = TXT("_proxy");
static const tchar* s_MasterRigSuffix = TXT("_master");
M_string MayaContentCmd::m_UnloadedProxyFileRefNodes;

// #define DEBUG_MAYACONTENTCMD

//---------------------------------------------------------------------------
// MayaContentCmd constructor
//---------------------------------------------------------------------------
MayaContentCmd::MayaContentCmd()
{
    m_Abort = false;
}

//-----------------------------------------------------------------------------
// MayaContentCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax MayaContentCmd::newSyntax()
{
    MSyntax syntax;

    syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
    syntax.useSelectionAsDefault( true );

    syntax.addFlag( ExportSelectionFlag, ExportSelectionFlagLong );

    return syntax;
}

//---------------------------------------------------------------------------
// MayaContentCmd::doIt
//---------------------------------------------------------------------------
MStatus MayaContentCmd::doIt(const MArgList &args)
{
    MStatus status = MS::kSuccess;

    try
    {
        // parse the command line arguments using the declared syntax
        MArgDatabase argParser( syntax(), args, &status );

        if( argParser.isFlagSet( ExportSelectionFlag ) )
        {
            MSelectionList selList;
            MGlobal::getActiveSelectionList( selList );

            MObject object;
            u32 len = selList.length();
            for( u32 i = 0; i < len; ++i )
            {
                selList.getDependNode( i, object );
                m_ObjectsToExport.append( object );      
            }
            m_Data = kSelected;

            status = doIt();

            if (status == MS::kSuccess)
            {
                MGlobal::displayInfo("MayaContentCmd completed successfully");
            }
            else
            {
                MGlobal::displayError("MayaContentCmd encountered errors, see script editor");
            }
        }
    }
    catch ( Nocturnal::Exception& ex )
    {
        Debug::ProcessException( ex );

        if( MGlobal::mayaState() == MGlobal::kInteractive )
        {
            ::MessageBox( M3dView::applicationShell(), ex.What(), TXT("Critical Error"), MB_OK );
        }
        else
        {
            std::cerr << ex.What() << std::endl;
        }
    }

    return status;
}

//---------------------------------------------------------------------------
// MayaContentCmd::doIt
//---------------------------------------------------------------------------
MStatus MayaContentCmd::doIt()
{
    EXPORT_SCOPE_TIMER( ("") );

    MStatus status = MS::kSuccess;

    try
    {
        // gather the definitions for the export scene
        if ( DefineExportScene() )
        {
            // go ahead with the export
            ExportSceneData();
            if ( WriteExportedData() )
            {
                // export finished, cleanup the export scene
                m_ExportScene.Reset();
            }
            else
            {
                // failure while writing
                status = MS::kFailure;
            }
        }
        else
        {
            // export scene did not export successfully
            status = MS::kFailure;
        }

        m_QueuedNodes.clear();
    }
    catch ( Nocturnal::Exception& ex )
    {
        Debug::ProcessException( ex );

        if( MGlobal::mayaState() == MGlobal::kInteractive )
        {
            ::MessageBox( M3dView::applicationShell(), ex.What(), TXT("Critical Error"), MB_OK );
        }
        else
        {
            std::cerr << ex.What() << std::endl;
        }
    }

    return status;
}

//---------------------------------------------------------------------------
// MayaContentCmd::DefineExportScene
//---------------------------------------------------------------------------
bool MayaContentCmd::DefineExportScene()
{
    EXPORT_SCOPE_TIMER( ("") );

    bool         success = true;
    MObjectArray joints;

    if (m_Data == MayaContentCmd::kAnimation)
    {
        //
        // Export Animations
        //  loop over the various skeletons that are being exported and export an animation clip for each one
        //

        ExportAnimationClipPtr exportAnimClip = new ExportAnimationClip ();
        m_ExportScene.Add( exportAnimClip );
    }
    else
    {
        //
        // Setup export list
        //
        MItDag dagIter;
        MObjectArray objects;

        if( m_Data < kCommandDataCount )
        {
            u32 numObjects = m_ObjectsToExport.length();
            for( u32 i = 0; i < numObjects; ++i )
            {
                MObject& object = m_ObjectsToExport[i];
                dagIter.reset(object);
                Queue( dagIter, objects );
            }
        }

        success = ExportArray(objects);
    }

    return success;
}

//---------------------------------------------------------------------------
// MayaContentCmd::ExportSceneData
//---------------------------------------------------------------------------
void MayaContentCmd::ExportSceneData()
{
    EXPORT_SCOPE_TIMER( ("") );

    m_ExportScene.GatherMayaData();
    ExportAnimationBase::SampleMayaAnimationData();
    m_ExportScene.ProcessMayaData();
    m_ExportScene.ExportData();
}

//---------------------------------------------------------------------------
// MayaContentCmd::WriteExportedData
//---------------------------------------------------------------------------
bool MayaContentCmd::WriteExportedData()
{
    EXPORT_SCOPE_TIMER( ("") );

    bool success = true;

    if( m_Data == kSelected )
    {
        tstring xml;

        bool success;
        try
        {
            Reflect::ArchiveXML::ToString(m_ExportScene.m_Spool, xml);
            success = true;
        }
        catch ( Nocturnal::Exception& ex )
        {
            Log::Error( TXT("%s\n"), ex.What() );
            success = false;
        }

        if ( success )
        {
            // null term
            size_t size = (xml.size() * sizeof(tchar)) +1;

            // alloc zeroed global moveable mem
            HGLOBAL mem = GlobalAlloc(GHND, size);
            success = ( mem != NULL );

            if ( success )
            {
                // copy
                tchar* buf = (tchar*)GlobalLock(mem);
                {
                    _tcscpy(buf, xml.c_str());
                }
                GlobalUnlock(mem);

                // set cb
                OpenClipboard(M3dView::applicationShell());
                EmptyClipboard();
                SetClipboardData(CF_TEXT, buf);
                CloseClipboard();
            }
        }
    }
    else
    {
        ContentVersionPtr v = new ContentVersion (TXT("Maya"), MGlobal::mayaVersion().asTChar());

        RCS::File rcsFile( m_ContentFileName );
        rcsFile.GetInfo();

        if ( rcsFile.IsCheckedOutBySomeoneElse() )
        {
            throw Nocturnal::Exception( TXT("One (or more) of this asset's export files is checked out and locked by another user. %s"), rcsFile.m_LocalPath.c_str() );
        }

        rcsFile.Open( RCS::OpenFlags::Exclusive );

        try
        {
            Reflect::Archive::ToFile(m_ExportScene.m_Spool, m_ContentFileName, v);
        }
        catch ( Nocturnal::Exception& ex )
        {
            MString str ("Unable to save content file to: ");
            str += m_ContentFileName.c_str();
            str += ": ";
            str += ex.What();
            MGlobal::displayError( str );
        }
    }

    return success;
}

//---------------------------------------------------------------------------
// MayaContentCmd::ExportArray
//---------------------------------------------------------------------------
bool MayaContentCmd::ExportArray(MObjectArray objects)
{
    bool success = true;


    //
    // Export queued nodes
    //

    unsigned int numObjects = objects.length();
    for ( unsigned int i = 0; i < numObjects; ++i )
    {
        int percent = (int)((float)i/(float)objects.length()*100.0f);

        if ( !ExportObject( objects[i], percent ) )
        {
            success = false;
        }
    }

    return success;
}

//---------------------------------------------------------------------------
// MayaContentCmd::ExportObject
//---------------------------------------------------------------------------
bool MayaContentCmd::ExportObject(MObject object, int percent)
{
    ElementPtr e = NULL;
    MFnDependencyNode nodeFn (object);

    //
    // Check for exclusion
    //

    MStatus status;
    MDagPath path = MDagPath::getAPathTo( object, &status );
    if ( status != MS::kSuccess )
    {
        return true;
    }

    //
    // API Types
    //

    MFn::Type objType = object.apiType();

    switch ( objType )
    {
    case MFn::kPluginTransformNode:
        {
            switch( nodeFn.typeId().id() )
            {
            case IGL_ENTITYNODE_ID:
                {
                    m_ExportScene.Add( new ExportEntity( object, Maya::GetNodeID( object ) ) );
                    return true;
                }
            }
            break;
        }

    case MFn::kTransform:
        {
            m_ExportScene.Add( new ExportPivotTransform( object, Maya::GetNodeID( object ) ) );
            return true;
        }

    case MFn::kJoint:
        {
            m_ExportScene.Add( new ExportJoint( object, Maya::GetNodeID( object ) ) );
            return true;
        }

    case MFn::kMesh:
        {
            m_ExportScene.Add( new ExportMesh( object, Maya::GetNodeID( object ) ) );
            return true;
        }

    case MFn::kNurbsCurve:
        {
            m_ExportScene.Add( new ExportCurve ( object, Maya::GetNodeID( object ) ) );

            return true;
        }
    }


    return true;
}

//---------------------------------------------------------------------------
// MayaContentCmd::Queue
//---------------------------------------------------------------------------
void MayaContentCmd::Queue(MItDag& dagIter, MObjectArray& objects)
{
    //
    // Queue
    //

#ifdef DEBUG_MAYACONTENTCMD
    std::ostringstream str;
    str << std::endl << "Queuing relatives of " << dagIter.fullPathName().asTChar();
    MGlobal::displayInfo( str.str().c_str() );
#endif

    for (; !dagIter.isDone(); dagIter.next())
    {
        MFnDagNode nodeFn (dagIter.item());

        if (nodeFn.isIntermediateObject())
        {
            continue;
        }

        QueueNode(dagIter.item(), objects);
    }
}

//---------------------------------------------------------------------------
// MayaContentCmd::QueueNode
//---------------------------------------------------------------------------
void MayaContentCmd::QueueNode(const MObject node, MObjectArray& objects)
{
    MFnDagNode nodeFn (node);


    //
    // Check ID
    //


    if (Maya::GetNodeID( node ) == TUID::Null)
    {
        MGlobal::displayError("Unable to set TUID attribute on maya node!");
        return;
    }


    //
    // Early out if we are not to be exported or we are already queued
    //

    if ( IsQueued( node ) )
    {
        return;
    }


    //
    // Queue
    //

#ifdef DEBUG_MAYACONTENTCMD
    std::ostringstream str;
    str << " Queuing " << nodeFn.fullPathName().asTChar();
    MGlobal::displayInfo( str.str().c_str() );
#endif

    objects.append(node);
    SetQueued(node);


    //
    // Queue Parents
    //

    MFn::Type objType = node.apiType();

    switch (objType)
    {
    case MFn::kPluginLocatorNode:
        {
            // instances export thier parent transform as themself, so we can skip it
            if (!QueueParents(nodeFn.parent(0), objects))
            {
                return;
            }

            break;
        }

    default:
        {
            // export our parent transform nodes (group nodes)
            if (!QueueParents(node, objects))
            {
                return;
            }

            break;
        }
    }
}

//---------------------------------------------------------------------------
// MayaContentCmd::QueueParents
//---------------------------------------------------------------------------
bool MayaContentCmd::QueueParents(const MObject node, MObjectArray& objects)
{
    //
    // Queue all nodes up the hierarchy
    //

    MObject parent (MFnDagNode(node).parent(0));

    while (parent != MObject::kNullObj)
    {
        MFnDagNode parentFn (parent);

        //
        // Check for ID
        //


        if ( Maya::GetNodeID( parent ) == TUID::Null)
        {
            MGlobal::displayError("Unable to set TUID attribute on maya node!");
            return false;
        }


        //
        // Queue
        //

        MDagPath path;
        parentFn.getPath(path);

        if (!IsQueued(parent) && path.fullPathName().length() > 0)
        {
            objects.append(parent);
            SetQueued(parent);
        }

        parent = parentFn.parent(0);
    }

    return true;
}

//---------------------------------------------------------------------------
// MayaContentCmd::IsQueued
//---------------------------------------------------------------------------
bool MayaContentCmd::IsQueued(const MObject node)
{
    // insert it into the map of exported ids
    Maya::MObjectSet::iterator i = m_QueuedNodes.find(node);

    return i != m_QueuedNodes.end();
}

//---------------------------------------------------------------------------
// MayaContentCmd::SetQueued
//---------------------------------------------------------------------------
void MayaContentCmd::SetQueued(const MObject node)
{
    // insert it into the map of exported ids
    bool result = m_QueuedNodes.insert(node).second;

    NOC_ASSERT(result);
}

void MayaContentCmd::ExportCurrentScene(CommandData data)
{
    EXPORT_SCOPE_TIMER( ("") );

    UnloadProxyFileReferences();

    MGlobal::MSelectionMode selectionMode = MGlobal::selectionMode();

    tstring currentFile = MFileIO::currentFile().asTChar();

    // clean path returned by maya
    Nocturnal::Path currentPath( currentFile );
    Nocturnal::Path contentFileDir( currentPath.Directory() );
    contentFileDir.MakePath();

    try
    {
        RCS::File rcsContentFile( contentFileDir.Get() + TXT("...") );
        rcsContentFile.GetInfo();
        if ( rcsContentFile.ExistsInDepot() && !rcsContentFile.IsCheckedOutByMe() && !rcsContentFile.IsUpToDate() )
        {
            rcsContentFile.m_LocalPath += TXT("\\...");
            rcsContentFile.Sync();
        }
    }
    catch( const Nocturnal::Exception& )
    {
        // this is ok, the directory may not exist in perforce yet
    }

    tstring contentFile = currentFile;

    MayaContentCmd cmd;
    cmd.m_Data = data;
    cmd.m_SourceFileName = currentFile;
    cmd.m_ContentFileName = contentFile;
    cmd.doIt();

    ReloadProxyFileReferences();

    MGlobal::setSelectionMode( selectionMode );
}

bool MayaContentCmd::ExportSelectionToClipboard()
{
    MayaContentCmd cmd;

    MSelectionList selList;
    MGlobal::getActiveSelectionList( selList );

    MObject object;
    u32 len = selList.length();
    for( u32 i = 0; i < len; ++i )
    {
        selList.getDependNode( i, object );
        cmd.m_ObjectsToExport.append( object );      
    }
    cmd.m_Data = kSelected;

    return cmd.doIt() == MS::kSuccess;
}

MStatus MayaContentCmd::UnloadProxyFileReferences()
{
    MStatus status( MStatus::kSuccess );

    m_UnloadedProxyFileRefNodes.clear();

    Maya::MObjectSet refNodes;
    Maya::findNodesOfType( refNodes, MFn::kReference );

    Maya::MObjectSet::iterator itor = refNodes.begin();
    Maya::MObjectSet::iterator end  = refNodes.end();
    for( ; itor != end; ++itor )
    {
        MObject &object = *itor;   
        MFnDependencyNode nodeFn( object );

        // skip sharedReferenceNode
        tstring nodeName( nodeFn.name().asTChar() );
        if ( nodeName.find( TXT("sharedReferenceNode") ) != tstring::npos )
        {
            continue;
        }

        // get the current fileName attribute
        MString command( TXT("referenceQuery -filename ") );
        command += nodeFn.name();

        MString refFileName;
        status = MGlobal::executeCommand( command, refFileName );
        if ( !status )
        {
            continue;
        }

        // clean path returned by maya 
        tstring curFilePath = refFileName.asTChar();
        Nocturnal::Path::Normalize( curFilePath );

        tstring::size_type findProxyPos = curFilePath.rfind( s_ProxyRigSuffix );
        if ( findProxyPos != tstring::npos )
        {
            tstring masterFilePath = curFilePath;
            masterFilePath.erase( findProxyPos, _tcslen( s_ProxyRigSuffix ) );
            masterFilePath.insert( findProxyPos, s_MasterRigSuffix );

            if ( Nocturnal::Path( masterFilePath ).Exists() )
            {
                //file -loadReference "yourReferenceNodeHere" -type "mayaBinary" -options "v=0" "pathToNewReferenceFileHere";
                tstringstream command;
                command << "file -loadReference \"" << nodeFn.name().asTChar() << "\"";
                command << " -type \"mayaBinary\" -options \"v=0\"";
                command << " \"" << masterFilePath.c_str() << "\"";

                status = MGlobal::executeCommand( command.str().c_str(), false, false );

                if ( status )
                {
                    // add the node to the list to be reset later
                    m_UnloadedProxyFileRefNodes.insert( M_string::value_type( nodeFn.name().asTChar(), curFilePath ) );
                }
                else
                {
                    MGlobal::displayError( MString ( "Unable to unload proxy file for reference node: " ) + nodeFn.name() );
                }
            }
            else
            {
                // warn the user that they are exporting a proxy file that doesn't have a master
            }
        }
    }

    return status;
}

MStatus MayaContentCmd::ReloadProxyFileReferences()
{
    MStatus status( MStatus::kSuccess );

    for each ( const M_string::value_type& proxyNodePair in m_UnloadedProxyFileRefNodes )
    {
        //file -loadReference "yourReferenceNodeHere" -type "mayaBinary" -options "v=0" "pathToNewReferenceFileHere";
        tstringstream command;
        command << "file -loadReference \"" << proxyNodePair.first << "\"";
        command << " -type \"mayaBinary\" -options \"v=0\"";
        command << " \"" << proxyNodePair.second << "\"";

        status = MGlobal::executeCommand( command.str().c_str(), false, false );

        if ( status == MS::kFailure )
        {
            MGlobal::displayError( MString ( "Unable to reload proxy file for reference node: " ) + proxyNodePair.first.c_str() );
        }
    }

    return status;
}

void MayaContentCmd::ConvertMatrix(const MMatrix& matrix, Math::Matrix4& outMatrix)
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            outMatrix[i][j] = (float)matrix[i][j];
}

void MayaContentCmd::ConvertMatrix(const Math::Matrix4& matrix, MMatrix& outMatrix)
{
    for (int i=0; i<4; i++)
        for (int j=0; j<4; j++)
            outMatrix[i][j] = matrix[i][j];
}

void MayaContentCmd::GetDynamicAttributes( const MObject &node, const Component::ComponentCollectionPtr& element )
{
    MFnDependencyNode nodeFn (node);

    for (unsigned int i=0; i<nodeFn.attributeCount(); i++)
    {
        MObject attr (nodeFn.attribute(i));
        MFnAttribute attrFn (attr);

        if( attrFn.isDynamic() )
        {
            ConvertAttributeToComponent( nodeFn, attr, element );
        }
    }
}

void MayaContentCmd::ConvertAttributeToComponent( const MFnDependencyNode &nodeFn, const MFnAttribute &attrFn, const Component::ComponentCollectionPtr &element )
{
#pragma TODO("Restore old Components")
#if 0
    MStatus status;

    MObject attr = attrFn.object();
    switch (attr.apiType())
    {
    case MFn::kNumericAttribute:
        {
            MFnNumericAttribute numericAttr (attr);

            switch (numericAttr.unitType())
            {
            case MFnNumericData::kBoolean:
                {
                    bool value;
                    MPlug plug (nodeFn.findPlug(attr, &status));

                    if (status == MS::kSuccess)
                    {
                        plug.getValue(value);
                        element->SetComponent(numericAttr.name().asTChar(), Reflect::Serializer::Create<bool>( value ) );
                    }

                    break;
                }

            case MFnNumericData::kShort:
                {
                    short value;
                    MPlug plug (nodeFn.findPlug(attr, &status));

                    if (status == MS::kSuccess)
                    {
                        plug.getValue(value);
                        element->SetComponent(numericAttr.name().asTChar(), Reflect::Serializer::Create<i16>( value ) );
                    }

                    break;
                }

            case MFnNumericData::kInt:
                {
                    int value;
                    MPlug plug (nodeFn.findPlug(attr, &status));

                    if (status == MS::kSuccess)
                    {
                        plug.getValue(value);
                        element->SetComponent(numericAttr.name().asTChar(), Reflect::Serializer::Create<i32>((i32)value));
                    }

                    break;
                }

            case MFnNumericData::kFloat:
                {
                    float value;
                    MPlug plug (nodeFn.findPlug(attr, &status));

                    if (status == MS::kSuccess)
                    {
                        plug.getValue(value);
                        element->SetComponent(numericAttr.name().asTChar(), Reflect::Serializer::Create<f32>((f32)value));
                    }

                    break;
                }

            case MFnNumericData::kDouble:
                {
                    double value;
                    MPlug plug (nodeFn.findPlug(attr, &status));

                    if (status == MS::kSuccess)
                    {
                        plug.getValue(value);
                        element->SetComponent(numericAttr.name().asTChar(), Reflect::Serializer::Create<f32>((f32)value));
                    }

                    break;
                }
            }

            break;
        }

    case MFn::kTypedAttribute:
        {
            MFnTypedAttribute typedAttr (attr);

            switch (typedAttr.attrType())
            {

            case MFnData::kIntArray:
                {
                    std::vector< i32 > values;

                    MObject plugValues;
                    MPlug plug (nodeFn.findPlug(attr, &status));
                    plug.getValue( plugValues );

                    if ( !plugValues.hasFn( MFn::kIntArrayData ) )
                        break;

                    MFnIntArrayData valueList( plugValues );

                    unsigned int count = valueList.length();

                    for ( unsigned int j=0; j < count; ++j )
                        values.push_back( valueList[ j ] );

                    element->SetComponent(typedAttr.name().asTChar(), Reflect::Serializer::Create<std::vector< i32 >>(values));
                    values.clear();

                    break;
                }

            case MFnData::kString:
                {
                    if (!attrFn.isArray())
                    {
                        MString value;
                        MPlug plug (nodeFn.findPlug(attr, &status));

                        if (status == MS::kSuccess)
                        {
                            plug.getValue(value);
                            element->SetComponent(typedAttr.name().asTChar(), Reflect::Serializer::Create(tstring (value.asTChar())));
                        }

                        break;
                    }
                }

            case MFnData::kStringArray:
                {
                    std::vector< tstring > values;

                    MString value;
                    MPlug plug (nodeFn.findPlug(attr, &status));

                    unsigned int count = plug.numElements();

                    for (unsigned int j=0; j<count; j++)
                    {
                        // by PHYSICAL index
                        plug.elementByPhysicalIndex(j).getValue(value);
                        values.push_back(value.asTChar());
                    }

                    element->SetComponent(typedAttr.name().asTChar(), Reflect::Serializer::Create<std::vector< tstring >>(values));
                    values.clear();

                    break;
                }
            }

            break;
        }

    case MFn::kUnitAttribute:
        {
            // we should catch all of these below, I think UnitAttribute is just a utility class FnSet
            NOC_ASSERT(false);
            break;
        }

    case MFn::kFloatLinearAttribute:
    case MFn::kDoubleLinearAttribute:
        {
            MFnUnitAttribute unitAttr (attr);

            float value;
            MPlug plug (nodeFn.findPlug(attr, &status));

            if (status != MS::kSuccess)
                break;

            plug.getValue(value);

            // CM -> M
            value *= Math::CentimetersToMeters;

            element->SetComponent(unitAttr.name().asTChar(), Reflect::Serializer::Create<f32>((f32)value));

            break;
        }

    case MFn::kFloatAngleAttribute:
    case MFn::kDoubleAngleAttribute:
        {
            MFnUnitAttribute unitAttr (attr);

            float value;
            MPlug plug (nodeFn.findPlug(attr, &status));

            if (status != MS::kSuccess)
                break;

            plug.getValue(value);

            // RADIANS -> DEGREES
            value *= Math::RadToDeg;

            element->SetComponent(unitAttr.name().asTChar(), Reflect::Serializer::Create<f32>((f32)value));

            break;
        }
    }
#endif
}

#include "Precompile.h"
#include "Duplicate.h"
#include "Shaders.h"
#include "NodeTypes.h"

#include "Foundation/Reflect/Element.h"
#include "Foundation/Reflect/Archive.h"
#include "Foundation/Reflect/Serializers.h"

#include <maya/MDagPath.h>
#include <maya/MDagPathArray.h>
#include <maya/MFnArrayAttrsData.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnComponentListData.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MFnGenericAttribute.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnLatticeData.h>
#include <maya/MFnLayeredShader.h>
#include <maya/MFnLightDataAttribute.h>
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MFnNurbsCurveData.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnNurbsSurfaceData.h>
#include <maya/MFnPhongEShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnSphereData.h>
#include <maya/MFnStringArrayData.h>
#include <maya/MFnSubd.h>
#include <maya/MFnSubdData.h>
#include <maya/MFnTransform.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnSet.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MItDag.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MSelectionList.h>

#include <map>

MStatus appendIfUnique( MObjectArray & objArray, const MObject & obj )
{
    MStatus stat = MS::kSuccess;
    bool foundIt;
    int  len;

    foundIt = false;
    len = objArray.length();
    for (int i=0; !foundIt && i<len; i++)
    {
        foundIt = (obj==objArray[i]);
    }
    if (!foundIt)
    {
        // only append the object if it wasn't already found
        stat = objArray.append( obj );
    }
    return stat;
}

namespace Maya
{
    static MString attrName_instObjGroups(      "instObjGroups" );
    static MString attrName_dagSetMembers(      "dagSetMembers" );
    static MString attrName_message(            "message"       );
    static MString attrName_groupNodes(         "groupNodes"    );
    static MString attrName_objectGroupId(      "objectGroupId" );
    static MString attrName_nodeState(          "nodeState"     );
    static MString attrName_binMembership(      "binMembership" );
    static MString attrName_intermediateObject( "intermediateObject" );

    ////////////////////////////////////////////////////////////////////////////////
    //
    //   +---------------------------------------------+
    //   |  overview of igDuplicate.cpp file contents  |
    //   +---------------------------------------------+
    //
    // Three class definitions (all internal to this file):
    //        *
    //        * igMObjectPairArray    --  array of (copy source, copy destination)
    //        * DependencyDuplicator  --  dups depend. nodes, following upstream connections
    //        * DagDuplicator         --  dups/instances dag/dependency nodes
    //
    // A few extern functions defined via igDuplicate.h
    //        *
    //        * duplicate                 --  dups/instances dag nodes via DagDuplicator
    //        * duplicate                 --  dups/instances depend nodes and upstream
    //        * dagPathFromSingleInstance --  returns a dag path for singly instanced node
    //        * parentOfPath              --  returns the singly instanced parent path
    //        * retargetToSingleInstance  --  reinitializes MFnDagNode with the MDagPath
    //        * retargetToChildOfParent   --  reinitializes MFnDagNode with the MDagPath
    //        * childPath                 --  returns MDagPath representing MFnDagNode.child(i)
    //        * nameWithoutNamespace      --  returns a name without the prefix: namespace
    //
    // And, several internal static functions are defined
    //
    ////////////////////////////////////////////////////////////////////////////////


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray
    //
    // note that using operator[] to search through an object of this class is (at least)
    // an O(n) operation.  If efficiency becomes a concern, this could be recoded as O(log(n)).
    ////////////////////////////////////////////////////////////////////////////////

    class igMObjectPairArray
    {
    public:
        igMObjectPairArray()                            {}
        ~igMObjectPairArray()                           {}
        MStatus      setLength( unsigned int length );
        unsigned int length() const;
        MStatus      remove( unsigned int index );
        MStatus      insert( const MObject & sourceObj,
            const MObject & destinObj,
            unsigned int    index );
        MStatus      append( const MObject & sourceObj,
            const MObject & destinObj );
        MStatus      change( const MObject & sourceObj,
            const MObject & destinObj,
            unsigned int    index );
        MStatus      read( unsigned int index,
            MObject &    sourceObj,
            MObject &    destinObj ) const;
        MStatus      clear( );
        const MObject & operator[]( const MObject & sourceObj ) const;
        MObject &       operator[]( const MObject & sourceObj );
    protected:
        MObjectArray m_source;
        MObjectArray m_destin;
        int getIndex( const MObject & sourceObj ) const;
    };


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator
    //
    ////////////////////////////////////////////////////////////////////////////////
    class DependencyDuplicator
    {
    public:
        DependencyDuplicator( const MObject  & sourceObject,
            bool       duplicateUpstream,
            bool       duplicateDagReferences );
        ~DependencyDuplicator()               { }

        bool      dependencyDuplicate();      /// perform duplication of dependency nodes, and set m_resultPath
        MObject   resultObj()                 { return m_resultObj; }

    protected:
        const MObject &      m_sourceObj;         /// the source of the duplication
        MObject              m_resultObj;         /// the result
        bool                 m_duplicateUpstream; /// whether to follow dependency nodes leading into node
        bool                 m_duplicateDagNodes; /// whether to consider upstream dag references as copyable
        MDGModifier          m_connector;         /// used by replicateConnectionsToRecordedNodes for connecting plugs
        igMObjectPairArray   m_copyPairs;         /// pairs of the source for each copied node and the destination

        bool      dependencyNodeCopy( MFnDependencyNode & sourceNodeFn,
            MFnDependencyNode & destinNodeFn );
        bool      replicateConnectionsToRecordedNodes();
        bool      recordCopiedNode( MFnDependencyNode & sourceNodeFn,
            MFnDependencyNode & destinNodeFn );
        bool      copyAttributes( MFnDependencyNode & sourceNodeFn,
            MFnDependencyNode & destinNodeFn,
            bool                copyAll );
    private:
        bool      createNode( MFn::Type           apiType,
            MTypeId             sourceType,
            MFnDependencyNode & newNodeFn );
        bool      createLambertNode(       MFnDependencyNode & newNodeFn );
        bool      createBlinnNode(         MFnDependencyNode & newNodeFn );
        bool      createPhongNode(         MFnDependencyNode & newNodeFn );
        bool      createPhongENode(        MFnDependencyNode & newNodeFn );
        bool      createLayeredShaderNode( MFnDependencyNode & newNodeFn );
        bool      replicateConnections( MFnDependencyNode & origNodeFn,
            MFnDependencyNode & copyNodeFn );
        bool      copyPlugConnections( MPlug & origPlug,
            MPlug & copyPlug );
        bool      copyArrayPlugConnections( MPlug & origPlug,
            MPlug & copyPlug );
        bool      copyCompoundPlugConnections( MPlug & origPlug,
            MPlug & copyPlug );
        bool      copyScalarPlugConnections( MPlug & origPlug,
            MPlug & copyPlug );

    };


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator
    //
    ////////////////////////////////////////////////////////////////////////////////
    class DagDuplicator : public DependencyDuplicator
    {
    public:
        DagDuplicator( const MDagPath & sourcePath,
            const MDagPath & destinParent,
            bool       instanceGeometry,
            bool       instanceAppropriateTransforms,
            bool       allowShadersFromReferenceFile,
            bool       duplicateUpstream );
        ~DagDuplicator()                      { }

        bool      dagDuplicate();             /// perform duplication of dag nodes, and set m_resultPath
        MDagPath  resultPath()                { return m_resultPath; }

    protected:
        const MDagPath &     m_sourcePath;    /// the source of the duplication
        const MDagPath &     m_destinParent;  /// the parent node of the duplicated result
        MDagPath             m_resultPath;    /// a child of m_destinParent, containing the result
        bool                 m_instanceGeo;   /// geometry should be instanced
        bool                 m_instanceTrans; /// transforms should be instanced, if they parent only geometry and transforms
        bool                 m_refShaders;    /// whether to allow assigned shaders to be references

        bool      copyShaders( MFnDagNode & sourceNodeFn,
            MFnDagNode & destinNodeFn );
        bool      dagDuplicate( MFnDagNode & sourceNodeFn,
            MFnDagNode & destinParentFn,
            MFnDagNode & destinNodeFn,
            bool         isDupRoot );
        bool      dagDuplicate_instance( MFnDagNode & sourceNodeFn,
            MFnDagNode & destinParentFn,
            MFnDagNode & destinNodeFn );
        bool      dagDuplicate_copy( MFnDagNode & sourceNodeFn,
            MFnDagNode & destinParentFn,
            MFnDagNode & destinNodeFn );
        bool      dagNodeCopy( MFnDagNode & sourceNodeFn,
            MFnDagNode & destinParentFn,
            MFnDagNode & destinNodeFn );
        bool      isGeometry( MFnDagNode & nodeFn );
        bool      isInstancable( MFnDagNode & nodeFn );

    private:
        MObject   copyNode(             const MDagPath & sourcePath,
            MObject    destinParentObj );
        MObject   copyMeshNode(         const MDagPath & sourcePath,
            MObject    destinParentObj );
        MObject   copyNurbsCurveNode(   const MDagPath & sourcePath,
            MObject    destinParentObj );
        MObject   copyNurbsSurfaceNode( const MDagPath & sourcePath,
            MObject    destinParentObj );
        MObject   copySubdNode(         const MDagPath & sourcePath,
            MObject    destinParentObj );           
        bool      assignShaders( MObjectArray & potentialShaderArray,
            MFnDagNode   & sourceGeometryFn,
            MFnDagNode   & destinGeometryFn );
    };


    ////////////////////////////////////////////////////////////////////////////////
    //
    // local functions
    //
    ////////////////////////////////////////////////////////////////////////////////

    static bool    unsuitableForCopy( const MPlug & plug );
    static bool    copyPlugValue( MPlug & sourcePlug,
        MPlug & destinPlug );
    static bool    copyAttribute( MFnDependencyNode & sourceNodeFn,
        unsigned int        attrIndex,
        MFnDependencyNode & destinNodeFn,
        bool                copyExisting );
    static bool    setupMeshIterator( MFnDagNode & rootDagFn,
        MItDag & iterator );
    static bool    setupMeshFn( MItDag & iterator,
        MFnDagNode & meshFn );
    static bool    findPotentialShaders( MFnDependencyNode & sourceMeshFn,
        MObjectArray      & shaders );
    static MObject copyAttributeDef( MObject originalAttrObj );


    ////////////////////////////////////////////////////////////////////////////////
    //
    // duplicate
    //
    // the publicly available entry point to duplicating maya dependency nodes
    ////////////////////////////////////////////////////////////////////////////////
    bool duplicate( const MObject & sourceObj,
        MObject & resultObj,
        bool      duplicateUpstreamGraph,
        bool      duplicateDagReferences )
    {
        bool                 success;
        // initialize the DependencyDuplicator object
        DependencyDuplicator dup(sourceObj,
            duplicateUpstreamGraph,
            duplicateDagReferences);
        // attempt to duplicate the dependency nodes
        success = dup.dependencyDuplicate();
        if (success)
        {
            resultObj = dup.resultObj();
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // duplicate
    //
    // the publicly available entry point to duplicating maya dag subtrees with
    // attached insomniac shaders
    ////////////////////////////////////////////////////////////////////////////////
    bool duplicate( const MDagPath & sourcePath,
        const MDagPath & destinParent,
        MDagPath & resultPath,
        bool       instanceGeometry,
        bool       instanceAppropriateTransforms,
        bool       allowShadersFromReferenceFile,
        bool       duplicateUpstreamGraph )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - top IGDUPLICATE::duplicate " ));

        bool          success;
        // initialize the DagDuplicator object
        DagDuplicator dup(sourcePath,
            destinParent,
            instanceGeometry,
            instanceAppropriateTransforms,
            allowShadersFromReferenceFile,
            duplicateUpstreamGraph );
        // attempt to duplicate the dag nodes
        success = dup.dagDuplicate();
        if (success)
        {
            // get the result path
            resultPath = dup.resultPath();
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::setLength
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::setLength( unsigned int length )
    {
        MStatus sourceStat;
        MStatus destinStat;
        sourceStat = m_source.setLength( length );
        destinStat = m_destin.setLength( length );
        if (sourceStat==MS::kSuccess)
        {
            return destinStat;
        }
        return sourceStat;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::length
    //
    ////////////////////////////////////////////////////////////////////////////////
    unsigned int igMObjectPairArray::length() const
    {
        return m_source.length();
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::remove
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::remove( unsigned int index )
    {
        MStatus stat;
        stat = m_source.remove( index );
        if (stat==MS::kSuccess)
        {
            stat = m_destin.remove( index );
        }
        return stat;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::insert
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::insert( const MObject & sourceObj,
        const MObject & destinObj,
        unsigned int    index )
    {
        MStatus stat;
        stat = m_source.insert( sourceObj, index );
        if (stat==MS::kSuccess)
        {
            stat = m_destin.insert( destinObj, index );
        }
        return stat;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::append
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::append( const MObject & sourceObj,
        const MObject & destinObj )
    {
        MStatus stat;
        stat = m_source.append( sourceObj );
        if (stat==MS::kSuccess)
        {
            stat = m_destin.append( destinObj );
        }
        return stat;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::change
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::change( const MObject & sourceObj,
        const MObject & destinObj,
        unsigned int    index )
    {
        m_source[index] = sourceObj;
        m_destin[index] = destinObj;
        return MS::kSuccess;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::read
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::read( unsigned int index,
        MObject    & sourceObj,
        MObject    & destinObj ) const
    {
        sourceObj = m_source[index];
        destinObj = m_destin[index];
        return MS::kSuccess;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::clear
    //
    ////////////////////////////////////////////////////////////////////////////////
    MStatus igMObjectPairArray::clear( )
    {
        MStatus stat;
        stat = m_source.clear();
        if (stat==MS::kSuccess)
        {
            stat = m_destin.clear();
        }
        return stat;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::operator[]
    //
    ////////////////////////////////////////////////////////////////////////////////
    const MObject & igMObjectPairArray::operator[]( const MObject & sourceObj ) const
    {
        int index = getIndex( sourceObj );
        if (index<0)
        {
            return MObject::kNullObj;
        }
        return m_destin[index];
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::operator[]
    //
    ////////////////////////////////////////////////////////////////////////////////
    MObject & igMObjectPairArray::operator[]( const MObject & sourceObj )
    {
        int index = getIndex( sourceObj );
        if (index<0)
        {
            return MObject::kNullObj;
        }
        return m_destin[index];
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // igMObjectPairArray::getIndex
    //
    ////////////////////////////////////////////////////////////////////////////////
    int igMObjectPairArray::getIndex( const MObject & sourceObj ) const
    {
        int          index = -1;
        unsigned int length = m_source.length();
        //****************************************
        // WOAH!!!  an O(n) search begins here!!!!
        //****************************************
        for (unsigned int i=0; index<0 && i<length; i++)
        {
            if (sourceObj==m_source[i])
            {
                index = (int)(i);
            }
        }
        return index;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::DependencyDuplicator constructor
    //
    ////////////////////////////////////////////////////////////////////////////////
    DependencyDuplicator::DependencyDuplicator( const MObject  & sourceObject,
        bool       duplicateUpstream,
        bool       duplicateDagNodes )
        : m_sourceObj(         sourceObject ),
        m_duplicateUpstream( duplicateUpstream ),
        m_duplicateDagNodes( duplicateDagNodes )
    {
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::dependencyDuplicate
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::dependencyDuplicate()
    {
        MStatus    stat;
        bool       success = false;
        // build a dependency node function set for the source object member variable
        MFnDependencyNode sourceNodeFn( m_sourceObj, &stat );
        if ( stat==MS::kSuccess )
        {
            // attempt to duplicate the source
            // this will have the side effect of populating the m_copyPairs array
            MFnDependencyNode destinNodeFn;
            if ( dependencyNodeCopy( sourceNodeFn, destinNodeFn ) )
            {
                // retrieve the result object
                m_resultObj = destinNodeFn.object( &stat );
                if (stat==MS::kSuccess)
                {
                    // connect the new object
                    // (and optionally follow upstream connections, creating new nodes)
                    success = replicateConnectionsToRecordedNodes();
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::dependencyNodeCopy
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::dependencyNodeCopy( MFnDependencyNode & sourceNodeFn,
        MFnDependencyNode & destinNodeFn )
    {
        MStatus      stat;
        bool         success = false;
        // find the type of the source
        MTypeId sourceType = sourceNodeFn.typeId( &stat );
        if (stat==MS::kSuccess)
        {
            // try to make a node of the same type
            if ( createNode( sourceNodeFn.object().apiType(), sourceType, destinNodeFn ) )
            {
                // remember this copied node
                if ( recordCopiedNode( sourceNodeFn, destinNodeFn ) )
                {
                    // copy all of the attributes from the old to the new
                    success = copyAttributes( sourceNodeFn, destinNodeFn, true );
                    if ( success )
                    {
                        // just to be clean, give the copied node a name something like the source
                        destinNodeFn.setName( nameWithoutNamespace(sourceNodeFn) );
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::createNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::createNode( MFn::Type           apiType,
        MTypeId             sourceType,
        MFnDependencyNode & newNodeFn )
    {
        MStatus stat;
        bool    success = false;
        switch (apiType)
        {
        case MFn::kLambert       :
            success = createLambertNode( newNodeFn );
            break;
        case MFn::kBlinn         :
            success = createBlinnNode( newNodeFn );
            break;
        case MFn::kPhong         :
            success = createPhongNode( newNodeFn );
            break;
        case MFn::kPhongExplorer :
            success = createPhongENode( newNodeFn );
            break;
        case MFn::kLayeredShader :
            success = createLayeredShaderNode( newNodeFn );
            break;
        default                  :
            newNodeFn.create( sourceType, &stat );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::createLambertNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::createLambertNode( MFnDependencyNode & newNodeFn )
    {
        MStatus          stat;
        bool             success;
        MFnLambertShader lambertShaderFn;
        MObject          lambertShaderObj = lambertShaderFn.create( true, &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            newNodeFn.setObject( lambertShaderObj );
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::createBlinnNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::createBlinnNode( MFnDependencyNode & newNodeFn )
    {
        MStatus          stat;
        bool             success;
        MFnBlinnShader   blinnShaderFn;
        MObject          blinnShaderObj = blinnShaderFn.create( true, &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            newNodeFn.setObject( blinnShaderObj );
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::createPhongNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::createPhongNode( MFnDependencyNode & newNodeFn )
    {
        MStatus          stat;
        bool             success;
        MFnPhongShader   phongShaderFn;
        MObject          phongShaderObj = phongShaderFn.create( true, &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            newNodeFn.setObject( phongShaderObj );
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::createPhongENode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::createPhongENode( MFnDependencyNode & newNodeFn )
    {
        MStatus          stat;
        bool             success;
        MFnPhongEShader  phongEShaderFn;
        MObject          phongEShaderObj = phongEShaderFn.create( true, &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            newNodeFn.setObject( phongEShaderObj );
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::createLayeredShaderNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::createLayeredShaderNode( MFnDependencyNode & newNodeFn )
    {
        MStatus          stat;
        bool             success;
        MFnLayeredShader layeredShaderFn;
        MObject          layeredShaderObj = layeredShaderFn.create( true, &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            newNodeFn.setObject( layeredShaderObj );
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::recordCopiedNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::recordCopiedNode( MFnDependencyNode & sourceNodeFn,
        MFnDependencyNode & destinNodeFn )
    {
        MStatus stat;
        bool    success = false;
        MObject sourceObj;
        MObject destinObj;
        sourceObj = sourceNodeFn.object( &stat );
        if (stat==MS::kSuccess)
        {
            destinObj = destinNodeFn.object( &stat );
            if (stat==MS::kSuccess)
            {
                MObject & oldDestinObj = m_copyPairs[sourceObj];
                if (oldDestinObj!=MObject::kNullObj)
                {
                    // already had this node, be sure it is correct
                    success = (oldDestinObj==destinObj);
                }
                else
                {
                    // didn't have this node, so append it
                    stat = m_copyPairs.append( sourceObj, destinObj );
                    success = (stat==MS::kSuccess);
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::replicateConnectionsToRecordedNodes
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::replicateConnectionsToRecordedNodes()
    {
        MStatus           stat;
        bool              success = true;
        MObject           origObj, copyObj;
        MFnDependencyNode origNodeFn, copyNodeFn;
        MPlugArray        connections;
        // loop through each recorded node
        // (the array may grow during processing, as upstream nodes are copied)
        for (unsigned int i=0; success && i<m_copyPairs.length(); i++)
        {
            // read recorded node i, which is a pair of original and copied paths
            stat = m_copyPairs.read(i, origObj, copyObj);
            success = (stat==MS::kSuccess);
            if (success)
            {
                // setup a reused dag function set to the i'th original
                stat = origNodeFn.setObject( origObj );
                success = (stat==MS::kSuccess);
                if (success)
                {
                    // setup a reused dag function set to the i'th copy
                    stat = copyNodeFn.setObject( copyObj );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // replicate all the original connections onto the copy
                        success = replicateConnections( origNodeFn, copyNodeFn );
                    }
                }
            }
        }
        if (success) 
        {
            // make all the requested connections
            m_connector.doIt();
        }
        else
        {
            // do not make the requested connections
            m_connector.undoIt();
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::replicateConnections
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::replicateConnections( MFnDependencyNode & origNodeFn,
        MFnDependencyNode & copyNodeFn )
    {
        MStatus      stat;
        MPlugArray   connections;
        bool         success = true;
        unsigned int i;
        unsigned int attrCount = origNodeFn.attributeCount( &stat );
        if (stat==MS::kSuccess)
        {
            // consider each attribute of the original node
            for (i=0; success && i<attrCount; i++)
            {
                // get the i'th attribute object
                MObject origAttrObj = origNodeFn.attribute( i, &stat );
                if (stat==MS::kSuccess)
                {
                    // get the i'th attribute plug
                    MPlug origAttrPlug = origNodeFn.findPlug( origAttrObj, true, &stat );
                    if (stat==MS::kSuccess)
                    {
                        // get the attribute function set for this attribute
                        MFnAttribute origAttrFn( origAttrObj, &stat );
                        success = (stat==MS::kSuccess);
                        if (success)
                        {
                            // make sure this is a top level attribute without a parent
                            MObject origAttrParent( origAttrFn.parent() );
                            if (origAttrParent==MObject::kNullObj)
                            {
                                // find the corresponding attribute in the copied node
                                MString attrName( origAttrFn.name() );
                                MPlug copyAttrPlug = copyNodeFn.findPlug( attrName, true, &stat );
                                if (stat==MS::kSuccess)
                                {
                                    // copy inputs from the original node plug into the copied node plug
                                    success = copyPlugConnections( origAttrPlug, copyAttrPlug );
                                }
                            }
                        }
                    }
                }
            }
        }
        return true;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::copyPlugConnections
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::copyPlugConnections( MPlug & origPlug,
        MPlug & copyPlug )
    {
        MStatus stat;
        bool    success = false;
        bool    isArray;
        bool    isCompound;
        // get the attribute function set for the original plug
        MObject origAttributeObj( origPlug.attribute(&stat) );
        if (stat==MS::kSuccess)
        {
            MFnAttribute origAttributeFn( origAttributeObj, &stat );
            if (stat==MS::kSuccess)
            {
                MString attrName( origAttributeFn.name() );
                if ( !origAttributeFn.isConnectable()  ||
                    attrName==attrName_instObjGroups ||
                    attrName==attrName_dagSetMembers ||
                    attrName==attrName_message       ||
                    attrName==attrName_groupNodes    ||
                    attrName==attrName_objectGroupId )
                {
                    // unconnectable plugs, just don't do anything
                    success = true;
                }
                else
                {
                    // is the original an array plug?
                    isArray = origPlug.isArray( &stat );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // does the isArray flag of the copy match the original?
                        if ((isArray==copyPlug.isArray(&stat)) && (stat==MS::kSuccess))
                        {
                            if (isArray)
                            {
                                // handle array plugs
                                success = copyArrayPlugConnections( origPlug, copyPlug );
                            }
                            // is the original a compound plug?
                            isCompound = origPlug.isCompound( &stat );
                            success = (stat==MS::kSuccess);
                            if (success)
                            {
                                // does the isCompound flag of the copy match the original?
                                if ((isCompound==copyPlug.isCompound(&stat)) && (stat==MS::kSuccess))
                                {
                                    if (isCompound)
                                    {
                                        // handle compound plugs
                                        success = copyCompoundPlugConnections( origPlug, copyPlug );
                                    }
                                    else
                                    {
                                        // handle plugs that are not array nor compound
                                        copyScalarPlugConnections( origPlug, copyPlug );
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::copyArrayPlugConnections
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::copyArrayPlugConnections( MPlug & origPlug,
        MPlug & copyPlug )
    {
        MStatus   stat;
        bool      success;
        MIntArray indices;
        unsigned int elementCount = origPlug.getExistingArrayAttributeIndices( indices, &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            // inspect every logical index of the original plug (end early if failure occurs)
            for (unsigned int i=0; success && i<elementCount; i++)
            {
                // get the original plug element
                unsigned int index = indices[i];
                MPlug origElementPlug = origPlug.elementByLogicalIndex( index, &stat );
                success = (stat==MS::kSuccess);
                if (success)
                {
                    // get the copied plug element (this will create it if not already there)
                    MPlug copyElementPlug = copyPlug.elementByLogicalIndex( index, &stat );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // copy the source plug value to the destination
                        success = copyPlugConnections( origElementPlug, copyElementPlug );
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::copyCompoundPlugConnections
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::copyCompoundPlugConnections( MPlug & origPlug,
        MPlug & copyPlug )
    {
        MStatus   stat;
        bool      success;
        unsigned int childCount = origPlug.numChildren( &stat );
        success = (stat==MS::kSuccess);
        if (success)
        {
            // inspect every logical index of the original plug (end early if failure occurs)
            for (unsigned int i=0; success && i<childCount; i++)
            {
                // get the original plug child
                MPlug origChildPlug = origPlug.child( i, &stat );
                success = (stat==MS::kSuccess);
                if (success)
                {
                    // get the copied plug child
                    MPlug copyChildPlug = copyPlug.child( i, &stat );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // copy the source plug value to the destination
                        success = copyPlugConnections( origChildPlug, copyChildPlug );
                    }
                }
            }
        }
        if (success)
        {
            // make any connections to the entire compound plug
            success = copyScalarPlugConnections( origPlug, copyPlug );
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::copyScalarPlugConnections
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::copyScalarPlugConnections( MPlug & origPlug,
        MPlug & copyPlug )
    {
        MStatus    stat;
        bool       success = true;
        MPlugArray connections;
        if ( origPlug.connectedTo(connections, true, false, &stat) && stat==MS::kSuccess)
        {
            unsigned int connectionCount = connections.length();
            if (connectionCount>1)
            {
                // how can there be more than one connection with this plug as its destination?
                success = false;
            }
            else
            {
                if (connectionCount==0)
                {
                    // no connections is still a success
                    success = true;
                }
                else
                {
                    MPlug fromPlug = connections[0];
                    // the connection from fromPlug to origPlug requires copying
                    MObject fromNodeObj = fromPlug.node();
                    // see if the connection came from a copied node
                    MObject copiedFromNodeObj = m_copyPairs[fromNodeObj];
                    if (copiedFromNodeObj==MObject::kNullObj)
                    {
                        // the connection didn't come from a copied node, check if it should be copied
                        if (m_duplicateUpstream)
                        {
                            // only copy upstream dag nodes if requested
                            MFnDagNode dagNodeFn( fromNodeObj, &stat );
                            if ( (stat==MS::kSuccess && m_duplicateDagNodes) ||
                                (stat!=MS::kSuccess) )
                            {
                                // isn't a dag node, try to attach a dependency function set
                                MFnDependencyNode depNodeFn( fromNodeObj, &stat );
                                if (stat==MS::kSuccess)
                                {
                                    // get the attribute object of the from plug
                                    MObject fromAttrObj( fromPlug.attribute(&stat) );
                                    if (stat==MS::kSuccess)
                                    {
                                        // attach a function set to the from plug
                                        MFnAttribute fromAttrFn( fromAttrObj, &stat );
                                        if (stat==MS::kSuccess)
                                        {
                                            // copy the upstream node
                                            // this will have a side effect of adding an element to m_copyPairs
                                            MFnDependencyNode upstreamNode;
                                            if ( dependencyNodeCopy( depNodeFn, upstreamNode ) )
                                            {
                                                // find the corresponding plug on the upstream copy
                                                MString fromAttrName( fromAttrFn.name() );
                                                MPlug actualFromPlug = upstreamNode.findPlug( fromAttrName, true, &stat );
                                                if (stat==MS::kSuccess)
                                                {
                                                    // found the actual 'from plug' on the copied node
                                                    // use this from plug rather than the one from the original node
                                                    fromPlug = actualFromPlug;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // the connection did come from a copied node, so get the actual from plug
                        // get the attribute object of the from plug
                        MObject fromAttrObj( fromPlug.attribute(&stat) );
                        if (stat==MS::kSuccess)
                        {
                            // attach a function set to the from plug
                            MFnAttribute fromAttrFn( fromAttrObj, &stat );
                            if (stat==MS::kSuccess)
                            {
                                // attach a function set to the copied node
                                MFnDependencyNode fromNodeFn( copiedFromNodeObj, &stat );
                                if (stat==MS::kSuccess)
                                {
                                    MString fromAttrName( fromAttrFn.name() );
                                    MPlug actualFromPlug = fromNodeFn.findPlug( fromAttrName, true, &stat );
                                    if (stat==MS::kSuccess)
                                    {
                                        // found the actual 'from plug' on the copied node
                                        // use this from plug rather than the one from the original node
                                        fromPlug = actualFromPlug;
                                    }
                                }
                            }
                        }
                    }
                    // connect to the copied plug
                    m_connector.connect( fromPlug, copyPlug );
                }
            }
        }
        return success;
    }

    /*
    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::copyAttributes
    //
    /// \param sourceNodeFn   the source for copying attributes
    /// \param destinNodeFn   the destination for copying attributes
    /// \param copyAll        copy all attributes or just the new ones
    /// \result               true if the attribute copy worked
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::copyAttributes( MFnDependencyNode & sourceNodeFn,
    MFnDependencyNode & destinNodeFn,
    bool                copyAll )
    {
    RF_SCOPE_TIMER(( __FUNCTION__ " - top DependencyDuplicator::copyAttributes " ));
    MStatus stat;
    bool    success = false;
    // look for attributes of the source
    unsigned int attributeCount = sourceNodeFn.attributeCount( &stat );
    if (stat==MS::kSuccess)
    {
    success = true; // hope for the best
    for (unsigned int i=0; success && i<attributeCount; i++)
    {
    success = copyAttribute( sourceNodeFn, i, destinNodeFn, copyAll );
    }
    }
    return success;
    }
    */

    ////////////////////////////////////////////////////////////////////////////////
    //
    // DependencyDuplicator::copyAttributes
    //
    /// \param sourceNodeFn   the source for copying attributes
    /// \param destinNodeFn   the destination for copying attributes
    /// \param copyAll        copy all attributes or just the new ones
    /// \result               true if the attribute copy worked
    ////////////////////////////////////////////////////////////////////////////////
    bool DependencyDuplicator::copyAttributes( MFnDependencyNode & sourceNodeFn,
        MFnDependencyNode & destinNodeFn,
        bool                copyAll )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - top DependencyDuplicator::copyAttributes " ));
        MStatus stat;
        bool    success = false;
        // look for attributes of the source
        unsigned int attributeCount = sourceNodeFn.attributeCount( &stat );
        if (stat==MS::kSuccess)
        {
            success = true; // hope for the best
            for (unsigned int i=0; success && i<attributeCount; i++)
            {
                MAYA_SCOPE_TIMER(( __FUNCTION__ " - copyAttribute " ));
                bool    hadAttribute = false;
                bool    hasAttribute = false;
                success = false;
                // find the attribute object for the specified index
                MObject attrObj = sourceNodeFn.attribute( i, &stat );
                if (stat==MS::kSuccess)
                {
                    // get the class of this attribute
                    MFnDependencyNode::MAttrClass attrClass = sourceNodeFn.attributeClass( attrObj, &stat );
                    if (stat==MS::kSuccess)
                    {
                        // create a function set to look into attribute's data
                        MFnAttribute attrFn( attrObj, &stat );
                        if (stat==MS::kSuccess)
                        {
                            // look for attributes without a parent, only copy/add top level attributes
                            MObject attrParent( attrFn.parent(&stat) );
                            if (stat==MS::kSuccess && attrParent!=MObject::kNullObj)
                            {
                                // for attributes that have a parent, assume the copy has already occured
                                success = true;
                            }
                            else
                            {
                                // get the name of the attribute
                                MString attrName( attrFn.name(&stat) );
                                if (stat==MS::kSuccess)
                                {
                                    // see if the destination has an attribute of this name
                                    hasAttribute = destinNodeFn.hasAttribute( attrName );
                                    // remember whether the destination had the attribute before it was (potentially) added
                                    hadAttribute = hasAttribute;
                                    if (!hasAttribute && attrClass==MFnDependencyNode::kLocalDynamicAttr)
                                    {
                                        // actually have to copy the attribute (grrr...) 
                                        // failure to make a copy can lead to internal maya failure when original is deleted
                                        MObject copiedAttrObj = copyAttributeDef( attrObj );
                                        if (copiedAttrObj!=MObject::kNullObj)
                                        {
                                            stat = destinNodeFn.addAttribute( copiedAttrObj, attrClass );
                                            hasAttribute = (stat==MS::kSuccess);
                                        }
                                    }
                                }
                            }
                            // be sure that the attribute now exists for the destination
                            if (hasAttribute)
                            {
                                if (!copyAll && hadAttribute)
                                {
                                    // don't recopy existing attributes if the copyExisting flag is off
                                    success = true;
                                }
                                else
                                {
                                    // try to copy the value 
                                    // look for the source plug
                                    MPlug   sourcePlug = sourceNodeFn.findPlug( attrObj, true, &stat );
                                    if (stat==MS::kSuccess)
                                    {
                                        if (unsuitableForCopy(sourcePlug))
                                        {
                                            // some standard attributes are not appropriate for copying
                                            success = true;
                                        }
                                        else
                                        {
                                            MPlug destinPlug = destinNodeFn.findPlug( attrObj, true, &stat );
                                            if (stat==MS::kSuccess)
                                            {
                                                MAYA_SCOPE_TIMER(( __FUNCTION__ " - copyPlugValue " ));
                                                success = copyPlugValue( sourcePlug, destinPlug );
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::DagDuplicator constructor
    //
    ////////////////////////////////////////////////////////////////////////////////
    DagDuplicator::DagDuplicator( const MDagPath & sourcePath,
        const MDagPath & destinParent,
        bool       instanceGeometry,
        bool       instanceAppropriateTransforms,
        bool       allowShadersFromReferenceFile,
        bool       duplicateUpstreamGraph )
        : DependencyDuplicator( sourcePath.node(), duplicateUpstreamGraph, false ),
        m_sourcePath(    sourcePath ),
        m_destinParent(  destinParent ),
        m_instanceGeo(   instanceGeometry ),
        m_instanceTrans( instanceAppropriateTransforms ),
        m_refShaders(    allowShadersFromReferenceFile )
    {
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::copyShaders
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::copyShaders( MFnDagNode & sourceNodeFn,
        MFnDagNode & destinNodeFn )
    {
        MStatus      stat;
        bool         success = true;
        MObjectArray potentialShaders;
        // look for shaders from this source
        if ( findPotentialShaders( sourceNodeFn, potentialShaders ) )
        {
            // source shaders seem to be found, copy the shaders
            success = assignShaders( potentialShaders, sourceNodeFn, destinNodeFn );
        }
        return success;
    }




    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::assignShaders
    //
    // given original geometry and given the corresponding copied geometry,
    // assign shaders to the copy so as to match those of the original
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::assignShaders( MObjectArray & potentialShaderArray,
        MFnDagNode   & sourceGeometryFn,
        MFnDagNode   & destinGeometryFn )
    {
        MStatus        stat;
        MSelectionList shaderMembers;
        MDagPath       memberDagPath;
        MObject        memberComponent;
        MDagPath       sourceDagPath;
        MDagPath       destinDagPath;
        MObject        sourceMeshNode = sourceGeometryFn.object();
        bool           reuseSourceShader;
        bool           success = false;
        // there should be one unique path to the source mesh
        stat = sourceGeometryFn.getPath( sourceDagPath );
        if (stat==MS::kSuccess)
        {
            // and, there should be one unique path to the destination mesh
            stat = destinGeometryFn.getPath( destinDagPath );
            if (stat==MS::kSuccess)
            {
                success = true;
                int shaderCount = potentialShaderArray.length();
                for (int i=0; success && i<shaderCount; i++)
                {
                    // inspect the i'th potential shader
                    MObject sourceShaderObj( potentialShaderArray[i] );
                    MFnSet sourceShader(sourceShaderObj, &stat);
                    if (stat==MS::kSuccess)
                    {
                        MObject destinShaderObj;
                        if (m_refShaders)
                        {
                            // if allowing shaders from a reference file then whatever shader we have is good
                            destinShaderObj = sourceShaderObj;
                        }
                        else
                        {
                            // not allowing shaders from a reference file, so be sure this isn't one
                            destinShaderObj = Maya::nonReferencedShadingEngine(sourceShaderObj);
                        }
                        // only one function set is needed for the shading engine if we reuse the source
                        reuseSourceShader = (destinShaderObj==sourceShaderObj);
                        if (reuseSourceShader)
                        {
                            destinShaderObj = MObject::kNullObj;
                        }
                        // when reuseSourceShader is false this will create a function set for the destination
                        MFnSet destinShader(destinShaderObj, &stat);
                        if (stat==MS::kSuccess || reuseSourceShader)
                        {
                            // see what elements the potential source shader applies to
                            stat = sourceShader.getMembers( shaderMembers, false );
                            if (stat==MS::kSuccess)
                            {
                                int memberCount = shaderMembers.length();
                                for (int j=0; j<memberCount; ++j)
                                {
                                    // inspect the j'th member of the shader
                                    stat = shaderMembers.getDagPath( j, memberDagPath, memberComponent );
                                    if (stat==MS::kSuccess)
                                    {
                                        if (memberDagPath==sourceDagPath)
                                        {
                                            // the source geometry is a member of the shader, so add the destination
                                            if (reuseSourceShader)
                                            {
                                                stat = sourceShader.addMember( destinDagPath, memberComponent );
                                            }
                                            else
                                            {
                                                stat = destinShader.addMember( destinDagPath, memberComponent );
                                            }
                                            if (stat!=MS::kSuccess)
                                            {
                                                MGlobal::displayWarning(destinShader.name()+" shading engine would not accept a surface.");
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::dagDuplicate
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::dagDuplicate()
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - DagDuplicator::dagDuplicate " ));
        MStatus    stat;
        bool       success = false;
        // build a dag node function set for the source path member variable
        MFnDagNode sourceNodeFn( m_sourcePath, &stat );
        if ( stat==MS::kSuccess &&
            !sourceNodeFn.inUnderWorld() &&
            !sourceNodeFn.isIntermediateObject() )
        {
            // build a dag node function set for the destination parent member variable
            MFnDagNode destinParentFn( m_destinParent, &stat );
            if ( stat==MS::kSuccess &&
                !destinParentFn.inUnderWorld() &&
                !destinParentFn.isIntermediateObject() )
            {
                // attempt to duplicate the source and place under destination parent
                // this will have the side effect of populating the m_copyPairs array
                MFnDagNode destinNodeFn;
                if ( dagDuplicate( sourceNodeFn, destinParentFn, destinNodeFn, true ) )
                {
                    // retrieve the path of the duplication result
                    m_resultPath = destinNodeFn.dagPath( &stat );
                    if ( stat==MS::kSuccess &&
                        !destinNodeFn.inUnderWorld() &&
                        !destinNodeFn.isIntermediateObject() )
                    {
                        m_resultObj = m_resultPath.node();
                        success = replicateConnectionsToRecordedNodes();
                    }
                }
                else
                {
                    // if dagDuplicate failed then cleanup any mess left behind
                    success = false;
                    MObject destinNodeObj( destinNodeFn.object(&stat) );
                    if (stat==MS::kSuccess && destinNodeObj!=MObject::kNullObj)
                    {
                        MGlobal::deleteNode( destinNodeObj );
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::dagDuplicate
    //
    /// \param sourceNodeFn   the source for duplication
    /// \param destinParentFn the parent where duplicated results will go
    /// \param destinNodeFn   set to the copied destination, child of destinParent
    /// \result               true if the duplicate worked
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::dagDuplicate( MFnDagNode & sourceNodeFn,
        MFnDagNode & destinParentFn,
        MFnDagNode & destinNodeFn,
        bool         isDupRoot )
    {
        bool success;
        /*
        if ( (m_instanceGeo   && isGeometry(sourceNodeFn))                  ||
        (m_instanceTrans && isInstancable(sourceNodeFn) && !isDupRoot)    )
        {
        // instance the current node
        success = dagDuplicate_instance( sourceNodeFn,
        destinParentFn,
        destinNodeFn );
        }
        else
        {
        // copy the current node
        success = dagDuplicate_copy( sourceNodeFn,
        destinParentFn,
        destinNodeFn );
        }
        */
        success = dagDuplicate_instance( sourceNodeFn,
            destinParentFn,
            destinNodeFn );
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::dagDuplicate_instance
    //
    /// \param sourceNodeFn   the source for duplication
    /// \param destinParentFn the parent where duplicated results will go
    /// \param destinNodeFn   set to the instanced destination, child of destinParent
    /// \result               true if the instancing worked
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::dagDuplicate_instance( MFnDagNode & sourceNodeFn,
        MFnDagNode & destinParentFn,
        MFnDagNode & destinNodeFn )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - DagDuplicator::dagDuplicate_instance " ));
        MStatus stat;
        bool    success = false;
        MObject sourceNodeObj( sourceNodeFn.object(&stat) );
        if (stat==MS::kSuccess)
        {
            // make an instance of the source node and place under the destination parent
            stat = destinParentFn.addChild( sourceNodeObj, MFnDagNode::kNextPos, true );
            success = (stat==MS::kSuccess);
            if (!success)
            {
                MGlobal::displayError( MString("could not create an instance of ") +
                    sourceNodeFn.partialPathName() +
                    " beneath the node " +
                    destinParentFn.partialPathName());
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::dagDuplicate_copy
    //
    /// \param sourceNodeFn   the source for duplication
    /// \param destinParentFn the parent where duplicated results will go
    /// \param destinNodeFn   set to the copied destination, child of destinParent
    /// \result               true if the duplicate worked
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::dagDuplicate_copy( MFnDagNode & sourceNodeFn,
        MFnDagNode & destinParentFn,
        MFnDagNode & destinNodeFn )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - DagDuplicator::dagDuplicate_copy " ));
        MStatus    stat;
        MFnDagNode sourceChildFn;
        MFnDagNode destinChildFn;
        bool       success = false;
        // copy the current node
        if ( dagNodeCopy(sourceNodeFn, destinParentFn, destinNodeFn) )
        {
            // rename the copy to match the source
            destinNodeFn.setName( nameWithoutNamespace(sourceNodeFn) );
            // record the copy locally
            recordCopiedNode( sourceNodeFn, destinNodeFn );
            // prepare to copy children
            int childCount = sourceNodeFn.childCount( &stat );
            success = (stat==MS::kSuccess);
            {
                MAYA_SCOPE_TIMER(( __FUNCTION__ " - DagDuplicator::dagDuplicate_copy children " ));

                for (int i=0; success && i<childCount; i++)
                {
                    // identify the source of the i'th child
                    MDagPath sourceChildPath = childPath( sourceNodeFn, i, &stat );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // get a function set for the child
                        stat = sourceChildFn.setObject( sourceChildPath );
                        success = (stat==MS::kSuccess);
                        // don't try to copy intermediate nodes as they will fail
                        if (success && !sourceChildFn.isIntermediateObject())
                        {
                            // recursive duplication occurs here
                            success = dagDuplicate( sourceChildFn, destinNodeFn, destinChildFn, false );
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::isGeometry
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::isGeometry( MFnDagNode & nodeFn )
    {
        MStatus stat;
        bool    foundGeometry = false;
        MObject nodeObj = nodeFn.object( &stat );
        if (stat==MS::kSuccess)
        {
            foundGeometry = nodeObj.hasFn( MFn::kGeometric );
        }
        return foundGeometry;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::isInstancable
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::isInstancable( MFnDagNode & nodeFn )
    {
        MStatus stat;
        bool    instancable = false;
        MObject nodeObj = nodeFn.object( &stat );
        if (stat==MS::kSuccess)
        {
            unsigned int numChildren = nodeFn.childCount( &stat );
            if (stat==MS::kSuccess)
            {
                // node itself must be instancable
                instancable = nodeObj.hasFn( MFn::kTransform ) ||
                    nodeObj.hasFn( MFn::kGeometric );
                for (unsigned int i=0; instancable && i<numChildren; i++)
                {
                    // all children of node must be instancable
                    MDagPath childNodePath = childPath( nodeFn, i, &stat );
                    instancable = (stat==MS::kSuccess);
                    if (instancable)
                    {
                        MFnDagNode childNodeFn( childNodePath, &stat );
                        instancable = (stat==MS::kSuccess);
                        if (instancable)
                        {
                            // recurse to evaluate whether children are instancable
                            instancable = isInstancable( childNodeFn );
                        }
                    }
                }
            }
        }
        return instancable;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::dagNodeCopy
    //
    // dagNodeCopy is the top level of the work done by DagDuplicator.  This method is
    // called internally by dagDuplicate for each node that is to be duplicated
    //
    /// \param sourceNodeFn   the source for node copy
    /// \param destinParentFn the parent where copied node will go
    /// \param destinNodeFn   set to the copied node, child of destinParent
    /// \result               true if the node copy worked
    ////////////////////////////////////////////////////////////////////////////////
    bool DagDuplicator::dagNodeCopy( MFnDagNode & sourceNodeFn,
        MFnDagNode & destinParentFn,
        MFnDagNode & destinNodeFn )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - DagDuplicator::dagNodeCopy " ));
        MStatus      stat;
        bool         success = false;
        // only copy nodes of real interest
        bool intermediateNode = sourceNodeFn.isIntermediateObject(&stat);
        if (stat==MS::kSuccess && !intermediateNode)
        {
            // find the source node
            MDagPath sourcePath = sourceNodeFn.dagPath( &stat );
            if (stat==MS::kSuccess)
            {
                // find the parent object of the duplication
                MDagPath destinParentPath = destinParentFn.dagPath( &stat );
                if (stat==MS::kSuccess)
                {
                    MObject destinParentObj = destinParentPath.node( &stat );
                    if (stat==MS::kSuccess)
                    {
                        // try to copy the source node and parent under the destination parent
                        MObject destinObj;
                        bool    copyAllAttributes = false;
                        bool    copyNoAttributes = false;
                        destinObj = copyNode( sourcePath, destinParentObj );
                        if (destinObj==MObject::kNullObj)
                        {
                            // since copy didn't succeed, all attributes will need copying into newly created node
                            copyAllAttributes = true;
                            // determine the type of the source node
                            MTypeId sourceType = sourceNodeFn.typeId( &stat );
                            if (stat==MS::kSuccess)
                            {
                                // create a new node with same type as source and parented under destination parent
                                MFnDagNode createDagFn;
                                // use the type of the source node
                                destinObj = createDagFn.create( sourceType, destinParentObj );
                            }
                        }
                        // the newly copied node or newly created node now needs its copied attributes
                        if (destinObj!=MObject::kNullObj)
                        {
                            destinNodeFn.setObject( destinObj );
                            if (stat==MS::kSuccess && retargetToSingleInstance(destinNodeFn))
                            {
                                if (copyNoAttributes)
                                {
                                    // if not copying attributes then this node is through copying
                                    success = true;
                                }
                                else
                                {
                                    // try to copy some or all of the node attributes
                                    success = copyAttributes( sourceNodeFn, destinNodeFn, copyAllAttributes );
                                }
                            }
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::copyNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    MObject DagDuplicator::copyNode( const MDagPath & sourcePath,
        MObject    destinParentObj )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - DagDuplicator::copyNode " ));

        MStatus stat;
        MObject newObj;
        // be sure this is a leaf, not a transform above a leaf
        MFnDagNode sourceNodeFn( sourcePath, &stat );
        if (stat==MS::kSuccess && sourceNodeFn.childCount()==0)
        {
            // possibly a mesh node
            newObj = copyMeshNode( sourcePath, destinParentObj );
            if (newObj==MObject::kNullObj)
            {
                // possibly a nurbs curve
                newObj = copyNurbsCurveNode( sourcePath, destinParentObj );
                if (newObj==MObject::kNullObj)
                {
                    // possibly a nurbs surface
                    newObj = copyNurbsSurfaceNode( sourcePath, destinParentObj );
                    if (newObj==MObject::kNullObj)
                    {
                        // possibly a subdivision surface
                        newObj = copySubdNode( sourcePath, destinParentObj );
                        if (newObj==MObject::kNullObj)
                        {
                            // no copy was performed; MObject::kNullObj is the return value
                        }
                    }
                }
            }
        }
        return newObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::copyMeshNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    MObject DagDuplicator::copyMeshNode( const MDagPath & sourcePath,
        MObject    destinParentObj )
    {
        MStatus stat;
        MObject newObj( MObject::kNullObj );
        // try setting up a mesh function set
        MFnMesh meshFn( sourcePath, &stat );
        if (stat==MS::kSuccess)
        {
            // this source is a mesh, so copy it
            MFnMesh newMeshFn;
            newObj = newMeshFn.copy( sourcePath.node(), destinParentObj, &stat );
            if (stat==MS::kSuccess)
            {
                // copy worked, so hook up appropriate shaders to the new mesh
                copyShaders( meshFn, newMeshFn );
            }
            else
            {
                MGlobal::displayError(stat.errorString());
                MGlobal::displayError(MString("unable to copy mesh ")+sourcePath.partialPathName());
            }
        }
        return newObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::copyNurbsCurveNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    MObject DagDuplicator::copyNurbsCurveNode( const MDagPath & sourcePath,
        MObject    destinParentObj )
    {
        MStatus stat;
        MObject newObj( MObject::kNullObj );
        // try setting up a nurbs curve function set
        MFnNurbsCurve nurbsCurveFn( sourcePath, &stat );
        if (stat==MS::kSuccess)
        {
            // this source is a nurbs curve, so copy it
            MFnNurbsCurve newNurbsCurveFn;
            newObj = newNurbsCurveFn.copy( sourcePath.node(), destinParentObj, &stat );
            if (stat!=MS::kSuccess)
            {
                MGlobal::displayError(stat.errorString());
                MGlobal::displayError(MString("unable to copy nurbs curve ")+sourcePath.partialPathName());
            }
        }
        return newObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::copyNurbsSurfaceNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    MObject DagDuplicator::copyNurbsSurfaceNode( const MDagPath & sourcePath,
        MObject    destinParentObj )
    {
        MStatus stat;
        MObject newObj( MObject::kNullObj );
        // try setting up a nurbs surface function set
        MFnNurbsSurface nurbsSurfaceFn( sourcePath, &stat );
        if (stat==MS::kSuccess)
        {
            // this source is a nurbs surface, so copy it
            MFnNurbsSurface newNurbsSurfaceFn;
            newObj = newNurbsSurfaceFn.copy( sourcePath.node(), destinParentObj, &stat );
            if (stat==MS::kSuccess)
            {
                // copy worked, so hook up appropriate shaders to the new nurbs surface
                copyShaders( nurbsSurfaceFn, newNurbsSurfaceFn );
            }
            else
            {
                MGlobal::displayError(stat.errorString());
                MGlobal::displayError(MString("unable to copy nurbs surface ")+sourcePath.partialPathName());
            }
        }
        return newObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DagDuplicator::copySubdNode
    //
    ////////////////////////////////////////////////////////////////////////////////
    MObject DagDuplicator::copySubdNode( const MDagPath & sourcePath,
        MObject    destinParentObj )
    {
        MStatus stat;
        MObject newObj( MObject::kNullObj );
        // try setting up a subdivision surface function set
        MFnSubd subdFn( sourcePath, &stat );
        if (stat==MS::kSuccess)
        {
            // this source is a subdivision surface, so copy it
            MFnSubd newSubdFn;
            newObj = newSubdFn.copy( sourcePath.node(), destinParentObj, &stat );
            if (stat==MS::kSuccess)
            {
                // copy worked, so hook up appropriate shaders to the new subdivision surface
                copyShaders( subdFn, newSubdFn );
            }
            else
            {
                MGlobal::displayError(stat.errorString());
                MGlobal::displayError(MString("unable to copy subd ")+sourcePath.partialPathName());
            }
        }
        return newObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // unsuitableForCopy
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool unsuitableForCopy( const MPlug & plug )
    {
        MStatus stat;
        bool    unsuitable = false;
        MObject attrObj = plug.attribute( &stat );
        if (stat==MS::kSuccess)
        {
            MFnAttribute attrFn( attrObj, &stat );
            if (stat==MS::kSuccess)
            {
                if (!attrFn.isReadable() || !attrFn.isWritable())
                {
                    // if the attribute can't be read or written, it can't be copied
                    unsuitable = true;
                }
                else
                {
                    MFnMessageAttribute msgAttrFn( attrObj, &stat );
                    if (stat==MS::kSuccess)
                    {
                        // message attributes are unsuitable for copying
                        unsuitable = true;
                    }
                    else
                    {
                        MFnGenericAttribute genAttrFn( attrObj, &stat );
                        if (stat==MS::kSuccess && !plug.isArray() && !plug.isCompound())
                        {
                            // generic scalars are unsuitable for copying
                            unsuitable = true;
                        }
                        else
                        {
                            // some named attributes are unsuitable for copying
                            MString attrName( attrFn.name(&stat) );
                            if (stat==MS::kSuccess)
                            {
                                unsuitable = (  attrName==attrName_instObjGroups      ||
                                    attrName==attrName_dagSetMembers      ||
                                    attrName==attrName_message            ||
                                    attrName==attrName_groupNodes         ||
                                    attrName==attrName_nodeState          ||
                                    attrName==attrName_binMembership      ||
                                    attrName==attrName_intermediateObject );
                            }
                        }
                    }
                }
            }
        }
        return unsuitable;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyAttributeValue
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static bool copyAttributeValue( MFnDependencyNode & sourceNodeFn,
        MFnDependencyNode & destinNodeFn,
        MObject             attrObj )
    {
        MStatus stat;
        bool    success = false;
        // look for the source plug
        MPlug   sourcePlug = sourceNodeFn.findPlug( attrObj, true, &stat );
        if (stat==MS::kSuccess)
        {
            if (unsuitableForCopy(sourcePlug))
            {
                // some standard attributes are not appropriate for copying
                success = true;
            }
            else
            {
                MPlug destinPlug = destinNodeFn.findPlug( attrObj, true, &stat );
                if (stat==MS::kSuccess)
                {
                    success = copyPlugValue( sourcePlug, destinPlug );
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyAttribute
    //
    /// \param sourceNodeFn  function set for the source node
    /// \param attrIndex     index of the attribute to copy from the source node
    /// \param destinNodeFn  function set for the destination node
    /// \param copyExisting  whether to copy if the attribute already exists in the destination
    /// \return              success of this function, including true when copyExisting=false prevented a copy
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyAttribute( MFnDependencyNode & sourceNodeFn,
        unsigned int        attrIndex,
        MFnDependencyNode & destinNodeFn,
        bool                copyExisting )
    {
        MStatus stat;
        bool    hadAttribute = false;
        bool    hasAttribute = false;
        bool    success = false;
        // find the attribute object for the specified index
        MObject attrObj = sourceNodeFn.attribute( attrIndex, &stat );
        if (stat==MS::kSuccess)
        {
            // get the class of this attribute
            MFnDependencyNode::MAttrClass attrClass = sourceNodeFn.attributeClass( attrObj, &stat );
            if (stat==MS::kSuccess)
            {
                // create a function set to look into attribute's data
                MFnAttribute attrFn( attrObj, &stat );
                if (stat==MS::kSuccess)
                {
                    // look for attributes without a parent, only copy/add top level attributes
                    MObject attrParent( attrFn.parent(&stat) );
                    if (stat==MS::kSuccess && attrParent!=MObject::kNullObj)
                    {
                        // for attributes that have a parent, assume the copy has already occured
                        success = true;
                    }
                    else
                    {
                        // get the name of the attribute
                        MString attrName( attrFn.name(&stat) );
                        if (stat==MS::kSuccess)
                        {
                            // see if the destination has an attribute of this name
                            hasAttribute = destinNodeFn.hasAttribute( attrName );
                            // remember whether the destination had the attribute before it was (potentially) added
                            hadAttribute = hasAttribute;
                            if (!hasAttribute && attrClass==MFnDependencyNode::kLocalDynamicAttr)
                            {
                                // actually have to copy the attribute (grrr...) 
                                // failure to make a copy can lead to internal maya failure when original is deleted
                                MObject copiedAttrObj = copyAttributeDef( attrObj );
                                if (copiedAttrObj!=MObject::kNullObj)
                                {
                                    stat = destinNodeFn.addAttribute( copiedAttrObj, attrClass );
                                    hasAttribute = (stat==MS::kSuccess);
                                }
                            }
                        }
                    }
                    // be sure that the attribute now exists for the destination
                    if (hasAttribute)
                    {
                        if (!copyExisting && hadAttribute)
                        {
                            // don't recopy existing attributes if the copyExisting flag is off
                            success = true;
                        }
                        else
                        {
                            // try to copy the value 
                            success = copyAttributeValue( sourceNodeFn, destinNodeFn, attrObj );
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyFailedMessage
    //
    ////////////////////////////////////////////////////////////////////////////////
    static void copyFailedMessage( MPlug & sourcePlug, tchar* copyPhase )
    {
        MString sourceName( sourcePlug.name() );
        MString failureMessage( MString(copyPhase) + " copy failed " + sourceName );
        MGlobal::displayError( failureMessage );
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // nonCompoundFailedMessage
    //
    ////////////////////////////////////////////////////////////////////////////////
    static void nonCompoundFailedMessage( tchar* attrType )
    {
        MString failureMessage( MString("Attempted to copy a ") +
            attrType +
            + " that was not a compound attribute." );
        MGlobal::displayError( failureMessage );
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_kBoolean
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_kBoolean( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        bool    value;
        // get the value from the source
        stat = sourcePlug.getValue( value );
        if (stat==MS::kSuccess)
        {
            // write it to the destination
            stat = destinPlug.setValue( value );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_kByte
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_kByte( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        char    value;
        // get the value from the source
        stat = sourcePlug.getValue( value );
        if (stat==MS::kSuccess)
        {
            // write it to the destination
            stat = destinPlug.setValue( value );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_kShort
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_kShort( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        short   value;
        // get the value from the source
        stat = sourcePlug.getValue( value );
        if (stat==MS::kSuccess)
        {
            // write it to the destination
            stat = destinPlug.setValue( value );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k2Short
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k2Short( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k2Short"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k3Short
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k3Short( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k3Short"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_kInt
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_kInt( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        int     value;
        // get the value from the source
        stat = sourcePlug.getValue( value );
        if (stat==MS::kSuccess)
        {
            // write it to the destination
            stat = destinPlug.setValue( value );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k2Int
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k2Int( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k2Int"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k3Int
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k3Int( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k3Int"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_kFloat
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_kFloat( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        float   value;
        // get the value from the source
        stat = sourcePlug.getValue( value );
        if (stat==MS::kSuccess)
        {
            // write it to the destination
            stat = destinPlug.setValue( value );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k2Float
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k2Float( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k2Float"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k3Float
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k3Float( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k3Float"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_kDouble
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_kDouble( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        double  value;
        // get the value from the source
        stat = sourcePlug.getValue( value );
        if (stat==MS::kSuccess)
        {
            // write it to the destination
            stat = destinPlug.setValue( value );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k2Double
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k2Double( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k2Double"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData_k3Double
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData_k3Double( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        nonCompoundFailedMessage(TXT("k3Double"));
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNumericData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus              stat;
        MFnNumericData::Type unitType     = MFnNumericData::kInvalid;
        tchar              * dataTypeName = TXT("numeric");
        bool                 success      = false;
        // get the attribute object
        MObject attrObj = sourcePlug.attribute( &stat );
        if (stat==MS::kSuccess)
        {
            // create an attribute function set
            MFnNumericAttribute attrNumFn( attrObj, &stat );
            if (stat==MS::kSuccess)
            {
                // it is a numeric attribute, so retrieve its type
                unitType = attrNumFn.unitType();
            }
            else
            {
                // numeric failed, so try enum attribute
                MFnEnumAttribute attrEnumFn( attrObj, &stat );
                if (stat==MS::kSuccess)
                {
                    // it's an enum attribute, so copy as though an int
                    unitType = MFnNumericData::kInt;
                }
                else
                {
                    // numeric failed and enum failed, so try unit attribute
                    MFnUnitAttribute attrUnitFn( attrObj, &stat );
                    if (stat==MS::kSuccess)
                    {
                        // it's a unit attribute, so copy as though a double
                        unitType = MFnNumericData::kDouble;
                    }
                }
            }
            switch (unitType)
            {
            case MFnNumericData::kBoolean:
                dataTypeName = TXT( "kBoolean");
                success = copyNumericData_kBoolean(sourcePlug, destinPlug);
                break;
            case MFnNumericData::kByte:
            case MFnNumericData::kChar:
                dataTypeName = TXT( "kByte");
                success = copyNumericData_kByte(sourcePlug, destinPlug);
                break;
            case MFnNumericData::kShort:
                dataTypeName = TXT( "kShort");
                success = copyNumericData_kShort(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k2Short:
                dataTypeName = TXT( "k2Short");
                success = copyNumericData_k2Short(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k3Short:
                dataTypeName = TXT( "k3Short");
                success = copyNumericData_k3Short(sourcePlug, destinPlug);
                break;
            case MFnNumericData::kInt:
                dataTypeName = TXT( "kInt");
                success = copyNumericData_kInt(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k2Int:
                dataTypeName = TXT( "k2Int");
                success = copyNumericData_k2Int(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k3Int:
                dataTypeName = TXT( "k3Int");
                success = copyNumericData_k3Int(sourcePlug, destinPlug);
                break;
            case MFnNumericData::kFloat:
                dataTypeName = TXT( "kFloat");
                success = copyNumericData_kFloat(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k2Float:
                dataTypeName = TXT( "k2Float");
                success = copyNumericData_k2Float(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k3Float:
                dataTypeName = TXT( "k3Float");
                success = copyNumericData_k3Float(sourcePlug, destinPlug);
                break;
            case MFnNumericData::kDouble:
                dataTypeName = TXT( "kDouble");
                success = copyNumericData_kDouble(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k2Double:
                dataTypeName = TXT( "k2Double");
                success = copyNumericData_k2Double(sourcePlug, destinPlug);
                break;
            case MFnNumericData::k3Double:
                dataTypeName = TXT( "k3Double");
                success = copyNumericData_k3Double(sourcePlug, destinPlug);
                break;
            default:
                dataTypeName = TXT( "unknown numeric");
                success = false;
                break;
            }
        }
        if (!success)
        {
            if (stat!=MS::kSuccess)
            {
                MGlobal::displayError(stat.errorString());
            }
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, dataTypeName);
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyPluginData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyPluginData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        bool success = false;
        MGlobal::displayWarning(TXT("unimplemented attribute copy"));
        if (!success)
        {
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("plugin"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyPluginGeometryData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyPluginGeometryData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        bool success = false;
        MGlobal::displayWarning(TXT("unimplemented attribute copy"));
        if (!success)
        {
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("plugin geometry"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyStringData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyStringData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MString stringValue;
        // get the string from the source
        stat = sourcePlug.getValue( stringValue );
        if (stat==MS::kSuccess)
        {
            // write the string to the destination
            stat = destinPlug.setValue( stringValue );
            success = (stat==MS::kSuccess);
        }
        if (!success)
        {
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("string"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyMatrixData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyMatrixData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject matrixObj;
        // get the object representing the matrix data
        stat = sourcePlug.getValue( matrixObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty matrix (bizarre?)
            success = true;
        }
        else
        {
            // try to setup a function set on the matrix data
            MFnMatrixData matrixDataFn( matrixObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( matrixObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("matrix"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyStringArrayData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyStringArrayData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject stringArrayObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( stringArrayObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty array
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnStringArrayData stringArrayDataFn( stringArrayObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( stringArrayObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("string array"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyDoubleArrayData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyDoubleArrayData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject doubleArrayObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( doubleArrayObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty array
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnDoubleArrayData doubleArrayDataFn( doubleArrayObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( doubleArrayObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("double array"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyIntArrayData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyIntArrayData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject intArrayObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( intArrayObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty array
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnIntArrayData intArrayDataFn( intArrayObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( intArrayObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("int array"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyPointArrayData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyPointArrayData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject pointArrayObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( pointArrayObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty array
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnPointArrayData pointArrayDataFn( pointArrayObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( pointArrayObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("point array"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyVectorArrayData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyVectorArrayData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject vectorArrayObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( vectorArrayObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty array
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnVectorArrayData vectorArrayDataFn( vectorArrayObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( vectorArrayObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("vector array"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyComponentListData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyComponentListData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject componentListObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( componentListObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty array
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnComponentListData componentListDataFn( componentListObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( componentListObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("component list"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyMeshData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyMeshData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject meshObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( meshObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty mesh
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnMeshData meshDataFn( meshObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( meshObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("mesh"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyLatticeData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyLatticeData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject latticeObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( latticeObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty lattice
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnLatticeData latticeDataFn( latticeObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( latticeObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("lattice"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNurbsCurveData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNurbsCurveData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject nurbsCurveObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( nurbsCurveObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty nurbsCurve
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnNurbsCurveData nurbsCurveDataFn( nurbsCurveObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( nurbsCurveObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("nurbs curve"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNurbsSurfaceData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyNurbsSurfaceData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject nurbsSurfaceObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( nurbsSurfaceObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty nurbsSurface
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnNurbsSurfaceData nurbsSurfaceDataFn( nurbsSurfaceObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( nurbsSurfaceObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("nurbs surface"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copySphereData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copySphereData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject sphereObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( sphereObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty sphere
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnSphereData sphereDataFn( sphereObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( sphereObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("sphere"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyDynArrayAttrsData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyDynArrayAttrsData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject arrayAttrsObj;
        MGlobal::displayWarning(TXT("Untested Insomniac Games function invoked: copyDynArrayAttrsData"));
        // get the object representing the array data
        stat = sourcePlug.getValue( arrayAttrsObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty arrayAttrs
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnArrayAttrsData arrayAttrsDataFn( arrayAttrsObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( arrayAttrsObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("multiple arrays of attributes"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyDynSweptGeometryData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyDynSweptGeometryData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        //  *** REMOVED BECAUSE build_tools WAS NOT LINKING CORRECTLY WITH MFnDynSweptGeometryData ***
        //  MObject dynSweptGeometryObj;
        //  // get the object representing the array data
        //  stat = sourcePlug.getValue( dynSweptGeometryObj );
        //  if (stat!=MS::kSuccess)
        //  {
        //    // silently ignore failure, this could be an empty dynSweptGeometry
        //    success = true;
        //  }
        //  else
        //  {
        //    // try to setup a function set on the array data
        //    MFnDynSweptGeometryData dynSweptGeometryDataFn( dynSweptGeometryObj, &stat );
        //    if (stat==MS::kSuccess)
        //    {
        //      // this data looks valid, write it to the destination
        //      stat = destinPlug.setValue( dynSweptGeometryObj );
        //      success = (stat==MS::kSuccess);
        //    }
        //  }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("swept geometry"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copySubdSurfaceData
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copySubdSurfaceData( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        MObject subdSurfaceObj;
        // get the object representing the array data
        stat = sourcePlug.getValue( subdSurfaceObj );
        if (stat!=MS::kSuccess)
        {
            // silently ignore failure, this could be an empty subdSurface
            success = true;
        }
        else
        {
            // try to setup a function set on the array data
            MFnSubdData subdSurfaceDataFn( subdSurfaceObj, &stat );
            if (stat==MS::kSuccess)
            {
                // this data looks valid, write it to the destination
                stat = destinPlug.setValue( subdSurfaceObj );
                success = (stat==MS::kSuccess);
            }
        }
        if (!success)
        {
            MGlobal::displayError(stat.errorString());
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("subdivision surface"));
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyScalarPlugValue
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static bool copyScalarPlugValue( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MAYA_SCOPE_TIMER(( __FUNCTION__ " - copyScalarPlugValue " ));
        typedef bool copyDataFn( MPlug & sourcePlug, MPlug & destinPlug );
        struct CopyFuncAssignment {
            MFnData::Type type;
            copyDataFn  * fn;
        };
        enum { copyFuncCount = 18 };
        static CopyFuncAssignment copyFunc[copyFuncCount] = {
            // kIntArray and similar numerics need to be listed BEFORE kNumeric
            // kString must be listed BEFORE kNumeric
            {MFnData::kDoubleArray,      &copyDoubleArrayData},
            {MFnData::kIntArray,         &copyIntArrayData},
            {MFnData::kPointArray,       &copyPointArrayData},
            {MFnData::kVectorArray,      &copyVectorArrayData},
            {MFnData::kMatrix,           &copyMatrixData},
            {MFnData::kStringArray,      &copyStringArrayData},
            {MFnData::kString,           &copyStringData},
            {MFnData::kPlugin,           &copyPluginData},
            {MFnData::kPluginGeometry,   &copyPluginGeometryData},
            {MFnData::kComponentList,    &copyComponentListData},
            {MFnData::kMesh,             &copyMeshData},
            {MFnData::kLattice,          &copyLatticeData},
            {MFnData::kNurbsCurve,       &copyNurbsCurveData},
            {MFnData::kNurbsSurface,     &copyNurbsSurfaceData},
            {MFnData::kSphere,           &copySphereData},
            {MFnData::kDynArrayAttrs,    &copyDynArrayAttrsData},
            //{MFnData::kDynSweptGeometry, &copyDynSweptGeometryData},
            {MFnData::kSubdSurface,      &copySubdSurfaceData},
            {MFnData::kNumeric,          &copyNumericData}
        };

        MStatus stat;
        bool    success = true;
        MObject attrObj = destinPlug.attribute( &stat );
        if (stat==MS::kSuccess)
        {
            MFn::Type attrType = attrObj.apiType();
            MFnAttribute attrFn( attrObj, &stat );
            if (stat==MS::kSuccess)
            {
                // search for a copyData function for this attribute
                copyDataFn * copyData = 0;
                for (unsigned int i=0; copyData==0 && i<copyFuncCount; i++)
                {
                    // see if this attribute accepts a particular data type
                    if ( attrFn.accepts(copyFunc[i].type) )
                    {
                        // record the copy function, which terminates the search
                        copyData = copyFunc[i].fn;
                    }
                }
                if (copyData!=0)
                {
                    // call the copy data function via the function pointer
                    success = copyData( sourcePlug, destinPlug );
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyArrayPlugValue
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static bool copyArrayPlugValue( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus      stat;
        bool         success = false;
        MIntArray    indices;
        // get all of the logical indices for elements connected or with value set
        unsigned int indexCount = sourcePlug.getExistingArrayAttributeIndices( indices, &stat );
        if (stat==MS::kSuccess)
        {
            // inspect every logical index of the source plug (end early if failure occurs)
            success = true;
            for (unsigned int i=0; success && i<indexCount; i++)
            {
                // get the source plug element
                unsigned int index = indices[i];
                MPlug sourceElementPlug = sourcePlug.elementByLogicalIndex( index, &stat );
                success = (stat==MS::kSuccess);
                if (success)
                {
                    // get the destination plug element (this will create it if not already there)
                    MPlug destinElementPlug = destinPlug.elementByLogicalIndex( index, &stat );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // copy the source plug value to the destination
                        success = copyPlugValue( sourceElementPlug, destinElementPlug );
                    }
                }
            }
        }
        if (!success)
        {
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("array attribute"));
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyCompoundPlugValue
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static bool copyCompoundPlugValue( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus  stat;
        bool     success = false;
        unsigned int sourceChildCount, destinChildCount, i;
        // get the number of source children
        sourceChildCount = sourcePlug.numChildren( &stat );
        if (stat==MS::kSuccess)
        {
            // get the number of destination children (it should match the source)
            destinChildCount = destinPlug.numChildren( &stat );
            if (stat==MS::kSuccess && sourceChildCount==destinChildCount)
            {
                // copy each child
                success = true;
                for (i=0; success && i<sourceChildCount; i++)
                {
                    // get the i'th source child
                    MPlug sourceChildPlug = sourcePlug.child( i, &stat );
                    success = (stat==MS::kSuccess);
                    if (success)
                    {
                        // get the i'th destination child
                        MPlug destinChildPlug = destinPlug.child( i, &stat );
                        success = (stat=MS::kSuccess);
                        if (success)
                        {
                            // copy the source child to the destination child
                            success = copyPlugValue( sourceChildPlug, destinChildPlug );
                        }
                    }
                }
            }
        }
        if (!success)
        {
            // explain a failure if it occurs
            copyFailedMessage(sourcePlug, TXT("compound attribute"));
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // determinePlugFlags
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static bool determinePlugFlags( MPlug & plug,
        bool  & isArray,
        bool  & isCompound,
        bool  & isProcedural )
    {
        MStatus stat;
        bool    success = false;
        isArray = plug.isArray( &stat );
        if (stat==MS::kSuccess)
        {
            isCompound = plug.isCompound( &stat );
            if (stat==MS::kSuccess)
            {
                isProcedural = plug.isProcedural( &stat );
                success = (stat==MS::kSuccess);
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyPlugValue
    //
    ////////////////////////////////////////////////////////////////////////////////
    static bool copyPlugValue( MPlug & sourcePlug,
        MPlug & destinPlug )
    {
        MStatus stat;
        bool    success = false;
        bool    sourceIsArray,      destinIsArray;
        bool    sourceIsCompound,   destinIsCompound;
        bool    sourceIsProcedural, destinIsProcedural;
        if ( determinePlugFlags( sourcePlug,
            sourceIsArray,
            sourceIsCompound,
            sourceIsProcedural ) )
        {
            if ( determinePlugFlags( destinPlug,
                destinIsArray,
                destinIsCompound,
                destinIsProcedural ) )
            {
                if ( sourceIsArray      == destinIsArray &&
                    sourceIsCompound   == destinIsCompound &&
                    sourceIsProcedural == destinIsProcedural )
                {
                    if (sourceIsProcedural ||
                        destinPlug.isFreeToChange(true, false)!=MPlug::kFreeToChange)
                    {
                        // don't try to copy procedural plugs
                        // nor try to change plugs not free to change
                        success = true;
                    }
                    else
                    {
                        if (sourceIsArray)
                        {
                            // array plugs
                            success = copyArrayPlugValue( sourcePlug, destinPlug );
                        }
                        else
                        {
                            if (sourceIsCompound)
                            {
                                // compound plugs
                                success = copyCompoundPlugValue( sourcePlug, destinPlug );
                            }
                            else
                            {
                                // plugs with actual values
                                success = copyScalarPlugValue( sourcePlug, destinPlug );
                            }
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // dagPathFromSingleInstance
    //
    ////////////////////////////////////////////////////////////////////////////////
    MDagPath dagPathFromSingleInstance( MObject obj )
    {
        MStatus    stat;
        MDagPath   dagPath;  // start with an invalid path as default return value
        MFnDagNode dagNodeFn( obj, &stat );
        if (stat==MS::kSuccess)
        {
            if ( retargetToSingleInstance( dagNodeFn ) )
            {
                dagPath = dagNodeFn.dagPath();
            }
        }
        return dagPath;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // parentOfPath
    //
    ////////////////////////////////////////////////////////////////////////////////
    MDagPath parentOfPath( const MDagPath & childDagPath )
    {
        MStatus    stat;
        MDagPath   dagPath;  // start with an invalid path as default return value
        MString    childName = childDagPath.fullPathName();
        MFnDagNode childDagNodeFn( childDagPath, &stat );
        if (stat==MS::kSuccess)
        {
            unsigned int parentCount = childDagNodeFn.parentCount( &stat );
            if (stat==MS::kSuccess)
            {
                bool foundPath = false;
                for (unsigned int i=0; !foundPath && i<parentCount; i++)
                {
                    MObject parentObj = childDagNodeFn.parent( i, &stat );
                    if (stat==MS::kSuccess)
                    {
                        MFnDagNode parentNodeFn( parentObj, &stat );
                        if (stat==MS::kSuccess && retargetToSingleInstance(parentNodeFn))
                        {
                            MString parentName = parentNodeFn.fullPathName(&stat);
                            unsigned int parentNameLength = parentName.length();
                            if (stat==MS::kSuccess && parentNameLength > 2)
                            {
                                // see if the parent full path name is the prefix of the child path name
                                MString childSubName( childName.substring(0, parentNameLength-1) );
                                if (childSubName==parentName)
                                {
                                    foundPath = true;
                                    dagPath = parentNodeFn.dagPath();
                                }
                            }
                        }
                    }
                }
                if (!foundPath && parentCount>0)
                {
                    // something went wrong so just make something up
                    MFnDagNode parentNodeFn( childDagNodeFn.parent(0) );
                    parentNodeFn.getPath( dagPath );
                }
            }
        }
        return dagPath;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // retargetToSingleInstance
    //
    // utility function that returns true if it is able to set the target object
    // of the dagNodeFn to the MDagPath representing the solitary instance of the node
    ////////////////////////////////////////////////////////////////////////////////
    bool retargetToSingleInstance( MFnDagNode & dagNodeFn )
    {
        MStatus       stat;
        MDagPathArray allDagPaths;
        bool          success = false;

        stat = dagNodeFn.getAllPaths( allDagPaths );
        if (stat==MS::kSuccess && allDagPaths.length()==1)
        {
            stat = dagNodeFn.setObject( allDagPaths[0] );
            success = (stat==MS::kSuccess);
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // retargetToChildOfParent
    //
    // utility function that returns true if it is able to set the target object
    // to the instance which is owned by the specified parent
    ////////////////////////////////////////////////////////////////////////////////
    bool retargetToChildOfParent( MFnDagNode & dagNodeFn,
        MFnDagNode & parentFn )
    {
        MStatus       stat;
        bool          success = false;
        MDagPath      parentPath;
        MString       parentPathStr;
        MDagPathArray allDagPaths;
        MDagPath      nodePath;
        MString       nodePathStr;

        // be sure that the parent is attached to a dag path
        parentPath = parentFn.dagPath( &stat );
        if (stat==MS::kSuccess || retargetToSingleInstance( parentFn ))
        {
            parentPath = parentFn.dagPath( &stat );
            if (stat==MS::kSuccess)
            {
                // recover the MString representing the name of the parent
                parentPathStr = parentFn.fullPathName( &stat );
                if (stat==MS::kSuccess)
                {
                    int lastPrefixChar = parentPathStr.length()-1;
                    // get all the paths leading to the node
                    stat = dagNodeFn.getAllPaths( allDagPaths );
                    if (stat==MS::kSuccess)
                    {
                        // look through the paths for one prefixed by the parent
                        unsigned int pathCount = allDagPaths.length();
                        for (unsigned int i=0; !success && i<pathCount; i++)
                        {
                            MString nodePathStr( allDagPaths[i].fullPathName(&stat) );
                            if (stat==MS::kSuccess)
                            {
                                // perhaps allDagPaths[i] is the correct child?  
                                MString prefix( nodePathStr.substring(0, lastPrefixChar) );
                                if (parentPathStr==prefix)
                                {
                                    // prefix matches the parent, so retarget the function set
                                    stat = dagNodeFn.setObject( allDagPaths[i] );
                                    success = (stat==MS::kSuccess);
                                }
                            }
                        }
                    }
                }
            }
        }
        return success;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // childPath
    //
    /// \param nodeFn       function set for the dag node being queried
    /// \param i            index of the i'th child requested from nodeFn
    /// \param returnStatus error code for any failure, otherwise MS::kSuccess
    /// \return             the path name for the i'th child of nodeFn
    ////////////////////////////////////////////////////////////////////////////////
    MDagPath childPath( MFnDagNode & nodeFn,
        unsigned int i, 
        MStatus    * returnStatus )
    {
        MStatus  stat;
        MDagPath resultPath;
        // find the child as an MObject
        MObject  childObj = nodeFn.child( i, &stat );
        if (stat==MS::kSuccess)
        {
            // build a function set for the child
            MFnDagNode childFn( childObj, &stat );
            if (stat==MS::kSuccess)
            {
                // make sure we have the right dag path instance for the child
                if (retargetToChildOfParent(childFn, nodeFn))
                {
                    // grab the dag path for the result
                    resultPath = childFn.dagPath( &stat );
                    if (stat!=MS::kSuccess)
                    {
                        // this shouldn't happen, try to get a good return value
                        stat = childFn.getPath( resultPath );
                    }
                }
            }
        }
        if (returnStatus!=NULL)
        {
            // return the MStatus result, if requested
            *returnStatus = stat;
        }
        return resultPath;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // nameWithoutNamespace
    //
    /// \param nodeFn       function set for the dag node being queried
    /// \return             the name of the node without any prefix: namespace
    ////////////////////////////////////////////////////////////////////////////////
    MString nameWithoutNamespace( MFnDependencyNode & nodeFn )
    {
        MStatus stat;
        MString name( nodeFn.name( &stat ) );
        if (stat!=MS::kSuccess)
        {
            // if the source has no name, then use a generic name "node" (should never happen)
            name.set("node");
        }
        else
        {
            // name is already okay, but let's try to remove any prefix
            MStringArray splitName;
            stat = name.split( ':', splitName );
            if (stat==MS::kSuccess)
            {
                int splitSections = splitName.length();
                if (splitSections>0)
                {
                    // the section after the final ':' is the unprefixed name
                    name = splitName[splitSections-1];
                }
            }
        }
        return name;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // setupMeshIterator
    //
    // given a root node setup an existing iterator to iterate over meshes under that root
    ////////////////////////////////////////////////////////////////////////////////
    bool setupMeshIterator( MFnDagNode & rootDagFn,
        MItDag     & iterator   )
    {
        MStatus stat;
        bool    success = false;
        MObject rootObj( rootDagFn.object(&stat) );
        if (stat==MS::kSuccess)
        {
            stat = iterator.reset( rootObj, MItDag::kBreadthFirst, MFn::kMesh );
            success = (stat==MS::kSuccess);
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // setupMeshFn
    //
    // attempt to setup a MFnDagNode on the current item of an iterator.  Only 
    // returns true if the result is mesh geometry and is not an intermeiate object
    ////////////////////////////////////////////////////////////////////////////////
    bool setupMeshFn( MItDag     & iterator,
        MFnDagNode & meshFn )
    {
        MStatus   stat;
        bool      success = false;
        MObject   meshObj = iterator.item(&stat);

        if (stat==MS::kSuccess)
        {
            if (meshObj.apiType()==MFn::kMesh)
            {
                // found a mesh, not a transform above a mesh
                MDagPath meshDagPath;
                stat = iterator.getPath( meshDagPath );
                if (stat==MS::kSuccess)
                {
                    stat = meshFn.setObject( meshDagPath );
                    if (stat==MS::kSuccess)
                    {
                        // make sure this isn't some historical gobbledygook
                        success = !meshFn.isIntermediateObject();
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // findPotentialShaders
    //
    ////////////////////////////////////////////////////////////////////////////////
    bool findPotentialShaders(  MFnDependencyNode & sourceMeshFn,
        MObjectArray      & shaders )
    {
        MStatus stat;
        bool    success = false;
        // start with no potential shaders found
        shaders.clear();
        // shaders are connected via instObjGroups
        MPlug instObjGroupsArray = sourceMeshFn.findPlug( attrName_instObjGroups, true, &stat );
        if (stat==MS::kSuccess && instObjGroupsArray.isArray())
        {
            int instances = instObjGroupsArray.numElements( &stat );
            if (stat==MS::kSuccess && instances>0)
            {
                // even if nothing is found, we've made it far enough that zero shaders is a valid return
                success = true;
                for (int i=0; i<instances; i++)
                {
                    // for the plug instObjGroups[i]
                    MPlug instObjGroups = instObjGroupsArray.elementByPhysicalIndex(i, &stat);
                    if (stat==MS::kSuccess)
                    {
                        // look for destination plugs connected to instObjGroups[i]
                        MPlugArray dagSetMembersArray;
                        if (instObjGroups.connectedTo(dagSetMembersArray, false, true, &stat) && stat==MS::kSuccess)
                        {
                            int shaderCount = dagSetMembersArray.length();
                            for (int j=0; j<shaderCount; j++)
                            {
                                // might be another shader
                                appendIfUnique( shaders, dagSetMembersArray[j].node() );
                            }
                        }
                        // look at the plug instObjGroups[i].objectGroups
                        MPlug objectGroupsArray = instObjGroups.child(0, &stat);
                        if (stat==MS::kSuccess && objectGroupsArray.isArray())
                        {
                            MIntArray objectGroupsIndices;
                            if ( objectGroupsArray.getExistingArrayAttributeIndices( objectGroupsIndices, &stat ) &&
                                stat==MS::kSuccess )
                            {
                                int objectGroupsIndexCount = objectGroupsIndices.length();
                                for (int j=0; j<objectGroupsIndexCount; j++)
                                {
                                    // for the plug instObjGroups[i].objectGroups[j]
                                    MPlug objectGroups = objectGroupsArray.elementByLogicalIndex(objectGroupsIndices[j], &stat);
                                    if (stat==MS::kSuccess)
                                    {
                                        MPlugArray dagSetMembersArray;
                                        if (objectGroups.connectedTo(dagSetMembersArray, false, true, &stat) && stat==MS::kSuccess)
                                        {
                                            int shaderCount = dagSetMembersArray.length();
                                            for (int j=0; j<shaderCount; j++)
                                            {
                                                // could be a shader for a face
                                                appendIfUnique( shaders, dagSetMembersArray[j].node() );
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return success;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyCompoundAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyCompoundAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject              copyAttrObj( origAttrObj );
        MFnCompoundAttribute copyAttrFn;
        MFnCompoundAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            copyAttrObj = copyAttrFn.create( fullName, briefName, &stat );
            if (stat==MS::kSuccess)
            {
                unsigned int numChildren = origAttrFn.numChildren();
                for (unsigned int i=0; i<numChildren; i++)
                {
                    MObject origChildObj = origAttrFn.child( i, &stat );
                    if (stat==MS::kSuccess)
                    {
                        copyAttrFn.addChild( copyAttributeDef( origChildObj ) );
                    }
                }
            }
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyEnumAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyEnumAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject          copyAttrObj( origAttrObj );
        MFnEnumAttribute copyAttrFn;
        MFnEnumAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            short int defaultValue, minValue, maxValue;
            MString   fullName(  origAttrFn.name() );
            MString   briefName( origAttrFn.shortName() );
            origAttrFn.getDefault( defaultValue );
            if (origAttrFn.getMin( minValue )!=MS::kSuccess)
            {
                minValue = 0;
            }
            if (origAttrFn.getMax( maxValue )!=MS::kSuccess)
            {
                maxValue = 0;
            }
            copyAttrObj = copyAttrFn.create( fullName, briefName, 0, &stat );
            if (stat==MS::kSuccess)
            {
                // copy each of the defined fields
                for (short int i=minValue; i<=maxValue; i++)
                {
                    MString fieldName( origAttrFn.fieldName( i, &stat ) );
                    if (stat==MS::kSuccess)
                    {
                        copyAttrFn.addField( fieldName, i );
                    }
                }
                // set the default value
                copyAttrFn.setDefault(defaultValue);
            }
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyGenericAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyGenericAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject             copyAttrObj( origAttrObj );
        MFnGenericAttribute copyAttrFn;
        MFnGenericAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            copyAttrObj = copyAttrFn.create( fullName, briefName, &stat );
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyLightDataAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyLightDataAttributeDef( MObject origAttrObj )
    {
        enum { childCount=8 };
        MStatus stat;
        MObject               copyAttrObj( origAttrObj );
        MFnLightDataAttribute copyAttrFn;
        MFnLightDataAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            unsigned int i;
            MObject      origChildObj[childCount];
            MObject      copyChildObj[childCount];
            MString      fullName(  origAttrFn.name() );
            MString      briefName( origAttrFn.shortName() );
            for (i=0; (stat==MS::kSuccess) && i<childCount; i++)
            {
                // get the original child attribute defs
                origChildObj[i] = origAttrFn.child( i, &stat );
            }
            if (stat==MS::kSuccess)
            {
                for (i=0; i<childCount; i++)
                {
                    // create new child attribute defs by copying the old
                    copyChildObj[i] = copyAttributeDef( origChildObj[i] );
                }
                // create the new parent attribute def
                copyAttrObj = copyAttrFn.create( fullName, briefName,
                    copyChildObj[0], copyChildObj[1],
                    copyChildObj[2], copyChildObj[3],
                    copyChildObj[4], copyChildObj[5],
                    copyChildObj[6], copyChildObj[7],
                    &stat );
            }
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyMatrixAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyMatrixAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject            copyAttrObj( origAttrObj );
        MFnMatrixAttribute copyAttrFn;
        MFnMatrixAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            // assume a double matrix is desired because I don't know how to detect a float matrix
            copyAttrObj = copyAttrFn.create( fullName, briefName, MFnMatrixAttribute::kDouble, &stat );
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyMessageAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyMessageAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject             copyAttrObj( origAttrObj );
        MFnMessageAttribute copyAttrFn;
        MFnMessageAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            copyAttrObj = copyAttrFn.create( fullName, briefName, &stat );
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyNumericAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyNumericAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject             copyAttrObj( origAttrObj );
        MFnNumericAttribute copyAttrFn;
        MFnNumericAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            double  defaultValue;
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            origAttrFn.getDefault( defaultValue );
            // create the numeric attribute
            copyAttrObj = copyAttrFn.create( fullName, briefName,
                origAttrFn.unitType(), defaultValue, &stat );
            if (stat==MS::kSuccess)
            {
                // set any limits
                double valueLimit;
                if (origAttrFn.getMin(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setMin(valueLimit);
                }
                if (origAttrFn.getMax(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setMax(valueLimit);
                }
                if (origAttrFn.getSoftMin(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setSoftMin(valueLimit);
                }
                if (origAttrFn.getSoftMax(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setSoftMax(valueLimit);
                }
            }
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyTypedAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyTypedAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject           copyAttrObj( origAttrObj );
        MFnTypedAttribute copyAttrFn;
        MFnTypedAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            MObject defaultData;
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            // unsure if the following getDefault call requires an initialized parameter?
            origAttrFn.getDefault( defaultData );
            copyAttrObj = copyAttrFn.create( fullName, briefName,
                origAttrFn.attrType(), defaultData, &stat );
        }
        return copyAttrObj;
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyUnitAttributeDef
    //
    ////////////////////////////////////////////////////////////////////////////////
    inline static MObject copyUnitAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject          copyAttrObj( origAttrObj );
        MFnUnitAttribute copyAttrFn;
        MFnUnitAttribute origAttrFn( origAttrObj, &stat );
        if (stat==MS::kSuccess)
        {
            double  defaultValue;
            MString fullName(  origAttrFn.name() );
            MString briefName( origAttrFn.shortName() );
            origAttrFn.getDefault( defaultValue );
            // create the unit attribute def
            copyAttrObj = copyAttrFn.create( fullName, briefName,
                origAttrFn.unitType(), defaultValue, &stat );
            if (stat==MS::kSuccess)
            {
                // set any limits
                double valueLimit;
                if (origAttrFn.getMin(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setMin(valueLimit);
                }
                if (origAttrFn.getMax(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setMax(valueLimit);
                }
                if (origAttrFn.getSoftMin(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setSoftMin(valueLimit);
                }
                if (origAttrFn.getSoftMax(valueLimit) == MS::kSuccess)
                {
                    copyAttrFn.setSoftMax(valueLimit);
                }
            }
        }
        return copyAttrObj;
    }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // copyAttributeDef
    //
    // It is annoying but true that if the attribute definition isn't copied, and
    // is used to define an attribute of a new node, that maya will crash if the
    // old node is deleted before returning to the maya command line.  Curiously,
    // the crash doesn't occur until after the Attribute Editor focuses on the new node.
    // Also, the crash doesn't occur if the old node is not deleted, or if the 
    // deletion occurs after returning to the command line.  Ahhh.. Maya... you lovely
    // piece of software.  This copyAttributeDef makes a duplicate copy of
    // an existing dynamic local attribute definition so that this error is prevented.
    ////////////////////////////////////////////////////////////////////////////////
    static MObject copyAttributeDef( MObject origAttrObj )
    {
        MStatus stat;
        MObject copyAttrObj;
        // retrieve attribute type
        MFn::Type origAttrType = origAttrObj.apiType();
        switch (origAttrType) {
    case MFn::kCompoundAttribute  :
        copyAttrObj = copyCompoundAttributeDef( origAttrObj );
        break;
    case MFn::kEnumAttribute      :
        copyAttrObj = copyEnumAttributeDef( origAttrObj );
        break;
    case MFn::kGenericAttribute   :
        copyAttrObj = copyGenericAttributeDef( origAttrObj );
        break;
    case MFn::kLightDataAttribute :
        copyAttrObj = copyLightDataAttributeDef( origAttrObj );
        break;
    case MFn::kMatrixAttribute    :
        copyAttrObj = copyMatrixAttributeDef( origAttrObj );
        break;
    case MFn::kMessageAttribute   :
        copyAttrObj = copyMessageAttributeDef( origAttrObj );
        break;
    case MFn::kNumericAttribute   :
        copyAttrObj = copyNumericAttributeDef( origAttrObj );
        break;
    case MFn::kTypedAttribute     :
        copyAttrObj = copyTypedAttributeDef( origAttrObj );
        break;
    case MFn::kUnitAttribute      :
        copyAttrObj = copyUnitAttributeDef( origAttrObj );
        break;
    default :
        copyAttrObj = MObject::kNullObj;
        }
        if (copyAttrObj==MObject::kNullObj)
        {
            // this situation shouldn't happen.
            // If it does anyway, then the following may OCCASSIONALLY cause maya to later crash.
            // It's better than just failing, I suppose.
            copyAttrObj = origAttrObj;
        }
        else
        {
            MFnAttribute origAttrFn( origAttrObj, &stat );
            if (stat==MS::kSuccess)
            {
                MFnAttribute copyAttrFn( copyAttrObj, &stat );
                if (stat==MS::kSuccess)
                {
                    // attribute has been copied, set the general flags
                    copyAttrFn.setReadable(          origAttrFn.isReadable()          );
                    copyAttrFn.setWritable(          origAttrFn.isWritable()          );
                    copyAttrFn.setConnectable(       origAttrFn.isConnectable()       );
                    copyAttrFn.setStorable(          origAttrFn.isStorable()          );
                    copyAttrFn.setCached(            origAttrFn.isCached()            );
                    copyAttrFn.setArray(             origAttrFn.isArray()             );
                    copyAttrFn.setIndexMatters(      origAttrFn.indexMatters()        );
                    copyAttrFn.setKeyable(           origAttrFn.isKeyable()           );
                    copyAttrFn.setChannelBox(        origAttrFn.isChannelBoxFlagSet() );
                    copyAttrFn.setHidden(            origAttrFn.isHidden()            );
                    copyAttrFn.setUsedAsColor(       origAttrFn.isUsedAsColor()       );
                    copyAttrFn.setIndeterminant(     origAttrFn.isIndeterminant()     );
                    copyAttrFn.setRenderSource(      origAttrFn.isRenderSource()      );
                    copyAttrFn.setWorldSpace(        origAttrFn.isWorldSpace()        );
                    copyAttrFn.setAffectsWorldSpace( origAttrFn.isAffectsWorldSpace() );
                }
            }
        }

        return copyAttrObj;
    }
}

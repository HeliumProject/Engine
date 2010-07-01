#pragma once

#include <string>
#include <strstream>
#include <maya/MPxCommand.h>
#include <maya/MObjectArray.h>
#include <maya/MMatrix.h>
#include <maya/MItDag.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnDependencyNode.h>

#include "Foundation/Reflect/Version.h"

#include "Maya/Utils.h"
#include "Maya/UniqueID.h"
#include "Maya/NodeTypes.h"
#include "Maya/ErrorHelpers.h"

#include "API.h"
#include "ExportScene.h"
#include "Platform/Types.h"

//
// Typedefs
//

typedef std::map<int, int> M_types;
typedef std::map<tstring, MObject> M_object;
typedef std::map<tstring, tstring> M_string;


//
// Export Insomniac Resources from Maya Scene to Reflect File
//  Created 1/21/03 by Geoff Evans
//

namespace MayaContent
{
  //
  // Exporter class definition
  //

  class MAYA_API MayaContentCmd : public MPxCommand
  {
  public:
    //
    // Operation Data
    //

    enum CommandData
    {
      kAnimation = 0,

      // general export types
      kRiggedMesh,
      kStaticMesh,

      // special cased types for special group nodes
      kCollision,
      kPathfinding,
      kGlue,

      // cinematics types
      kCineScene,

      // selected objects for export
      kSelected,

      // anything under here is not mappable to a filespec
      kCommandDataCount,   
    };

    typedef std::map< CommandData, MObjectArray > M_DataObjects;

  private:
    // this sets the data set we want to process
    CommandData m_Data;

    // the file name of the source file we are dealing with
    tstring m_SourceFileName;
    tstring m_ContentFileName;
    tstring m_FragmentName;

    // the action abort flag
    bool m_Abort;

    // this tracks objects are queued to be exported, so we don't export a node twice
    Maya::S_MObject m_QueuedNodes;

    // the scene full of export objects to do the export
    ExportScene m_ExportScene;

    // the objects sorted by export type
    MObjectArray m_ObjectsToExport[kCommandDataCount];

    // the root to use for searching
    MObject m_Root;

    static M_string m_UnloadedProxyFileRefNodes;

    //
    // Implementation
    //

  public:
    MayaContentCmd ();

    static void* creator ()
    {
      return new MayaContentCmd ();
    }

    virtual bool isUndoable()
    {
      return false;
    }

    virtual bool hasSyntax()
    {
      return true;
    }

    static MSyntax newSyntax();

    // MEL entry point
    MStatus doIt (const MArgList &args);

    // Common code for MEL and Library entry points
    MStatus doIt ();

    // Guts of the export process
    bool DefineExportScene();
    void ExportSceneData();
    bool WriteExportedData();


    //
    // Member Functions
    //

    void GetSelected (MObjectArray& objects);
    bool GetFile ();

    //
    // Export routines
    //

    // exports objects
    bool ExportArray (MObjectArray objects);

    // export maya class object
    bool ExportObject(MObject object, int percent = -1);


    //
    // Export queue and touched tracking
    //

    void Queue (MItDag& dagItr, MObjectArray& objects);
    void QueueNode (const MObject node, MObjectArray& objects);
    bool QueueParents (const MObject node, MObjectArray& objects);

    bool IsQueued (const MObject node);
    void SetQueued (const MObject node);

    //
    // Gets the data types exported
    //
    void DetermineExportedTypes( BitArray& types );

    //
    // This is the primary interface to the exporter
    //
    static void ExportCurrentScene( MObject root = MObject::kNullObj, tstring& sourceFile = tstring(), tstring& fragmentName = tstring() );

    //
    // Export whatever is selected to the clipboard as XML
    //
    static bool ExportSelectionToClipboard();

    //
    // Proxy File References
    //
    static MStatus UnloadProxyFileReferences();
    static MStatus ReloadProxyFileReferences();

    //
    // Utils
    //

    static void ConvertMatrix (const MMatrix& matrix, Math::Matrix4& outMatrix);
    static void ConvertMatrix (const Math::Matrix4& outMatrix, MMatrix& matrix);

    static void GetDynamicAttributes( const MObject& node, const Component::ComponentCollectionPtr& element );
    static void ConvertAttributeToComponent( const MFnDependencyNode &nodeFn, const MFnAttribute &attrFn, const Component::ComponentCollectionPtr &element );
  };
}

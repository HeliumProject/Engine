#include "Precompile.h"

#include "Callbacks.h"
#include "ExportNode.h"
#include "ExportNodeSet.h"
#include "Maya/NodeTypes.h"

MIntArray g_CallbackIDs; 

MStatus CallbacksCreate() 
{
    MStatus stat;

    g_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kBeforeImport, ExportNodeSet::PreImportCB, NULL, &stat));
    if (!stat)
        MGlobal::displayError("Unable to add Before Import callback for ExportNodeSet.\n");  

    g_CallbackIDs.append((int)MSceneMessage::addCallback( MSceneMessage::kAfterImport, ExportNodeSet::PostImportCB, NULL, &stat));
    if (!stat)
        MGlobal::displayError("Unable to add Before Import callback for ExportNodeSet.\n");  

    g_CallbackIDs.append((int)MDGMessage::addNodeAddedCallback ( ExportNode::NodeAddedCB, ExportNode::s_TypeName, NULL, &stat));
    if (!stat)
        MGlobal::displayError("Unable to add Node Added callback for ExportNode.\n");  

    return MS::kSuccess; 
}

MStatus CallbacksDelete()
{
    MStatus stat;

    // Remove all the callbacks
    stat = MMessage::removeCallbacks(g_CallbackIDs);
    if (!stat)
    {
        MGlobal::displayError("Unable to delete callbacks");  
    }

    return MS::kSuccess;
}

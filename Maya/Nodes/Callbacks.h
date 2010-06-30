#pragma once

#include <maya/MDGMessage.h>
#include <maya/MSceneMessage.h>
#include <maya/MEventMessage.h>
#include <maya/MModelMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MFnMessageAttribute.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MSyntax.h>

MStatus CallbacksCreate();
MStatus CallbacksDelete();

#include "SceneGraphPch.h"
#include "SceneGraphInit.h"

#include "Application/InitializerStack.h"
#include "Reflect/Registry.h"

#include "SceneGraph/SettingsManager.h"
#include "SceneGraph/SceneSettings.h"
#include "SceneGraph/Tool.h"
#include "SceneGraph/CreateTool.h"
#include "SceneGraph/DuplicateTool.h"
#include "SceneGraph/ScaleManipulator.h"
#include "SceneGraph/RotateManipulator.h"
#include "SceneGraph/TranslateManipulator.h"
#include "SceneGraph/JointTransform.h"
#include "SceneGraph/PivotTransform.h"
#include "SceneGraph/Layer.h"
#include "SceneGraph/Curve.h"
#include "SceneGraph/CurveControlPoint.h"
#include "SceneGraph/CurveCreateTool.h"
#include "SceneGraph/CurveEditTool.h"
#include "SceneGraph/Instance.h"
#include "SceneGraph/Locator.h"
#include "SceneGraph/LocatorCreateTool.h"
#include "SceneGraph/ViewportSettings.h"
#include "SceneGraph/SceneSettings.h"
#include "SceneGraph/SceneManifest.h"
#include "SceneGraph/GridSettings.h"
#include "SceneGraph/Primitive.h"

using namespace Helium;
using namespace Helium::SceneGraph;

static Helium::InitializerStack g_SceneGraphInitStack;

void SceneGraph::Initialize()
{
	if ( g_SceneGraphInitStack.Increment() == 1 )
	{
		// core library initiailization
		g_SceneGraphInitStack.Push( Reflect::Initialize, Reflect::Cleanup );
		g_SceneGraphInitStack.Push( PropertiesGenerator::Initialize, PropertiesGenerator::Cleanup );
	}
}

void SceneGraph::Cleanup()
{
	g_SceneGraphInitStack.Decrement();
}

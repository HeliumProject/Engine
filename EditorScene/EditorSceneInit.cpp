#include "EditorScenePch.h"
#include "EditorSceneInit.h"

#include "Application/InitializerStack.h"
#include "Reflect/Registry.h"

#include "EditorScene/SettingsManager.h"
#include "EditorScene/SceneSettings.h"
#include "EditorScene/Tool.h"
#include "EditorScene/CreateTool.h"
#include "EditorScene/DuplicateTool.h"
#include "EditorScene/ScaleManipulator.h"
#include "EditorScene/RotateManipulator.h"
#include "EditorScene/TranslateManipulator.h"
#include "EditorScene/PivotTransform.h"
#include "EditorScene/Layer.h"
#include "EditorScene/Curve.h"
#include "EditorScene/CurveControlPoint.h"
#include "EditorScene/CurveCreateTool.h"
#include "EditorScene/CurveEditTool.h"
#include "EditorScene/Instance.h"
#include "EditorScene/Locator.h"
#include "EditorScene/LocatorCreateTool.h"
#include "EditorScene/ViewportSettings.h"
#include "EditorScene/SceneSettings.h"
#include "EditorScene/SceneManifest.h"
#include "EditorScene/GridSettings.h"
#include "EditorScene/Primitive.h"

using namespace Helium;
using namespace Helium::Editor;

static Helium::InitializerStack g_EditorSceneInitStack;

void Editor::Initialize()
{
	if ( g_EditorSceneInitStack.Increment() == 1 )
	{
		// core library initiailization
		g_EditorSceneInitStack.Push( Reflect::Initialize, Reflect::Cleanup );
		g_EditorSceneInitStack.Push( PropertiesGenerator::Initialize, PropertiesGenerator::Cleanup );
	}
}

void Editor::Cleanup()
{
	g_EditorSceneInitStack.Decrement();
}

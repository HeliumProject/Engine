#include "SceneGraphPch.h"
#include "SceneGraphInit.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/SettingsManager.h"
#include "Foundation/Reflect/Registry.h"

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
#include "SceneGraph/Shader.h"
#include "SceneGraph/Mesh.h"
#include "SceneGraph/Skin.h"
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
#include "SceneGraph/Project.h"

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

        g_SceneGraphInitStack.Push( Primitive::InitializeType, Primitive::CleanupType );

        g_SceneGraphInitStack.Push( Tool::InitializeType, Tool::CleanupType );
        g_SceneGraphInitStack.Push( CreateTool::InitializeType, CreateTool::CleanupType );
        g_SceneGraphInitStack.Push( DuplicateTool::InitializeType, DuplicateTool::CleanupType );

        g_SceneGraphInitStack.Push( TransformManipulator::InitializeType, TransformManipulator::CleanupType );
        g_SceneGraphInitStack.Push( TranslateManipulator::InitializeType, TranslateManipulator::CleanupType );
        g_SceneGraphInitStack.Push( RotateManipulator::InitializeType, RotateManipulator::CleanupType );
        g_SceneGraphInitStack.Push( ScaleManipulator::InitializeType, ScaleManipulator::CleanupType );

        g_SceneGraphInitStack.Push( Graph::InitializeType, Graph::CleanupType );
        g_SceneGraphInitStack.Push( SceneNode::InitializeType, SceneNode::CleanupType );
        g_SceneGraphInitStack.Push( HierarchyNode::InitializeType, HierarchyNode::CleanupType );
        g_SceneGraphInitStack.Push( Transform::InitializeType, Transform::CleanupType );
        g_SceneGraphInitStack.Push( JointTransform::InitializeType, JointTransform::CleanupType );
        g_SceneGraphInitStack.Push( PivotTransform::InitializeType, PivotTransform::CleanupType );
        g_SceneGraphInitStack.Push( Layer::InitializeType, Layer::CleanupType );

        g_SceneGraphInitStack.Push( Shader::InitializeType, Shader::CleanupType );
        g_SceneGraphInitStack.Push( Mesh::InitializeType, Mesh::CleanupType );
        g_SceneGraphInitStack.Push( Skin::InitializeType, Skin::CleanupType );

        g_SceneGraphInitStack.Push( CurveControlPoint::InitializeType, CurveControlPoint::CleanupType );
        g_SceneGraphInitStack.Push( Curve::InitializeType, Curve::CleanupType );
        g_SceneGraphInitStack.Push( CurveCreateTool::InitializeType, CurveCreateTool::CleanupType );
        g_SceneGraphInitStack.Push( CurveEditTool::InitializeType, CurveEditTool::CleanupType );

        g_SceneGraphInitStack.Push( Instance::InitializeType, Instance::CleanupType );

        g_SceneGraphInitStack.Push( Locator::InitializeType, Locator::CleanupType );
        g_SceneGraphInitStack.Push( LocatorCreateTool::InitializeType, LocatorCreateTool::CleanupType );
    }
}

void SceneGraph::Cleanup()
{
    g_SceneGraphInitStack.Decrement();
}

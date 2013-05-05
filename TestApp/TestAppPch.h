#pragma once

#include "TestApp/TestApp.h"

#include "Platform/System.h"
#include "Platform/Socket.h"
#include "Platform/Trace.h"
#include "Platform/Timer.h"
#include "Engine/FileLocations.h"
#include "Foundation/FilePath.h"
#include "Foundation/FileStream.h"
#include "Engine/AsyncLoader.h"
#include "Foundation/Map.h"
#include "Foundation/SortedMap.h"
#include "Foundation/SortedSet.h"
#include "MathSimd/Matrix44.h"
#include "MathSimd/Quat.h"
#include "MathSimd/Matrix44Soa.h"
#include "MathSimd/QuatSoa.h"
#include "MathSimd/AaBox.h"
#include "Math/Float16.h"
#include "Engine/AssetType.h"
#include "Engine/Package.h"
#include "Engine/JobManager.h"
#include "Engine/JobContext.h"
#include "Engine/Config.h"
#include "Engine/CacheManager.h"
#include "EngineJobs/EngineJobsInterface.h"
#include "PcSupport/ConfigPc.h"
#include "TestJobs/TestJobsInterface.h"
#include "Rendering/RRenderCommandProxy.h"
#include "Rendering/RRenderContext.h"
#include "Rendering/RSurface.h"
#include "Graphics/Animation.h"
#include "Graphics/DynamicDrawer.h"
#include "Graphics/Font.h"
#include "Graphics/GraphicsConfig.h"
#include "Graphics/Material.h"
#include "Graphics/RenderResourceManager.h"
#include "GraphicsJobs/GraphicsJobs.h"
#include "Framework/Slice.h"
#include "Graphics/Mesh.h"
#include "Framework/World.h"
#include "Framework/WorldManager.h"
#include "RenderingD3D9/D3D9Renderer.h"

#if HELIUM_TOOLS
#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/LooseAssetLoader.h"
#include "EditorSupport/FontResourceHandler.h"
#include "PreprocessingPc/PcPreprocessor.h"
#endif

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_sort.h"

#include <tchar.h>
#include <algorithm>

#include "TestApp/gtest.h"
#include "TestApp/GTest_Globals.h"

namespace Helium
{
    class SkeletalMeshEntity;
    typedef Helium::StrongPtr< SkeletalMeshEntity > SkeletalMeshEntityPtr;
    typedef Helium::StrongPtr< const SkeletalMeshEntity > ConstSkeletalMeshEntityPtr;

	class StaticMeshEntity;
    typedef Helium::StrongPtr< StaticMeshEntity > StaticMeshEntityPtr;
    typedef Helium::StrongPtr< const StaticMeshEntity > ConstStaticMeshEntityPtr;

    HELIUM_DECLARE_RPTR( RRenderContext );
    HELIUM_DECLARE_RPTR( RSurface );
}

#pragma once

#include "TestApp/TestApp.h"

#include "Platform/Platform.h"
#include "Platform/Socket.h"
#include "Platform/Trace.h"
#include "Platform/Timer.h"
#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Foundation/Stream/FileStream.h"
#include "Foundation/Stream/BufferedStream.h"
#include "Foundation/AsyncLoader.h"
#include "Foundation/Container/Map.h"
#include "Foundation/Container/SortedMap.h"
#include "Foundation/Container/SortedSet.h"
#include "Foundation/Math/SimdMatrix44.h"
#include "Foundation/Math/SimdQuat.h"
#include "Foundation/Math/SimdMatrix44Soa.h"
#include "Foundation/Math/SimdQuatSoa.h"
#include "Foundation/Math/SimdAaBox.h"
#include "Foundation/Math/Float16.h"
#include "Engine/GameObjectType.h"
#include "Engine/Package.h"
#include "Engine/JobManager.h"
#include "Engine/JobContext.h"
#include "Engine/Config.h"
#include "Engine/CacheManager.h"
#include "EngineJobs/EngineJobsInterface.h"
#include "PcSupport/PcCacheObjectLoader.h"
#include "PcSupport/XmlSerializer.h"
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
#include "Graphics/GraphicsCustomTypeRegistration.h"
#include "GraphicsJobs/GraphicsJobs.h"
#include "Framework/Camera.h"
#include "Framework/Layer.h"
#include "Framework/Mesh.h"
#include "Framework/SkeletalMeshEntity.h"
#include "Framework/StaticMeshEntity.h"
#include "Framework/World.h"
#include "Framework/WorldManager.h"
#include "RenderingD3D9/D3D9Renderer.h"

#if HELIUM_TOOLS
#include "PcSupport/ObjectPreprocessor.h"
#include "EditorSupport/EditorObjectLoader.h"
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
    HELIUM_DECLARE_PTR( Camera );
    HELIUM_DECLARE_PTR( SkeletalMeshEntity );
    HELIUM_DECLARE_PTR( StaticMeshEntity );

    HELIUM_DECLARE_RPTR( RRenderContext );
    HELIUM_DECLARE_RPTR( RSurface );
}

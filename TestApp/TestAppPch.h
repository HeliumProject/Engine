#pragma once

#include "TestApp/TestApp.h"

#include "Platform/Platform.h"
#include "Platform/Socket.h"
#include "Platform/Trace.h"
#include "Platform/Timer.h"
#include "Foundation/File.h"
#include "Foundation/FilePath.h"
#include "Foundation/FileStream.h"
#include "Foundation/BufferedStream.h"
#include "Engine/AsyncLoader.h"
#include "Foundation/Map.h"
#include "Foundation/SortedMap.h"
#include "Foundation/SortedSet.h"
#include "Math/SimdMatrix44.h"
#include "Math/SimdQuat.h"
#include "Math/SimdMatrix44Soa.h"
#include "Math/SimdQuatSoa.h"
#include "Math/SimdAaBox.h"
#include "Math/Float16.h"
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
    class Camera;
    typedef Helium::StrongPtr< Camera > CameraPtr;
    typedef Helium::StrongPtr< const Camera > ConstCameraPtr;

    class SkeletalMeshEntity;
    typedef Helium::StrongPtr< SkeletalMeshEntity > SkeletalMeshEntityPtr;
    typedef Helium::StrongPtr< const SkeletalMeshEntity > ConstSkeletalMeshEntityPtr;

	class StaticMeshEntity;
    typedef Helium::StrongPtr< StaticMeshEntity > StaticMeshEntityPtr;
    typedef Helium::StrongPtr< const StaticMeshEntity > ConstStaticMeshEntityPtr;

    HELIUM_DECLARE_RPTR( RRenderContext );
    HELIUM_DECLARE_RPTR( RSurface );
}

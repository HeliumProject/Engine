// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "Platform/Trace.h"
#include "Foundation/File/File.h"
#include "Foundation/File/Path.h"
#include "Foundation/Stream/FileStream.h"
#include "Foundation/Stream/BufferedStream.h"
#include "Foundation/AsyncLoader.h"
#include "Foundation/Container/Map.h"
#include "Foundation/Container/SortedMap.h"
#include "Foundation/Container/SortedSet.h"
#include "Platform/Timer.h"
#include "Platform/Math/Simd/Matrix44.h"
#include "Platform/Math/Simd/Quat.h"
#include "Platform/Math/Simd/Matrix44Soa.h"
#include "Platform/Math/Simd/QuatSoa.h"
#include "Platform/Math/Simd/AaBox.h"
#include "Platform/Math/Float16.h"
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
#include "Graphics/GraphicsEnumRegistration.h"
#include "GraphicsJobs/GraphicsJobs.h"
#include "Framework/Camera.h"
#include "Framework/Layer.h"
#include "Framework/Mesh.h"
#include "Framework/SkeletalMeshEntity.h"
#include "Framework/StaticMeshEntity.h"
#include "Framework/World.h"
#include "Framework/WorldManager.h"
#include "D3D9Rendering/D3D9Renderer.h"

#if L_EDITOR
#include "PcSupport/ObjectPreprocessor.h"
#include "EditorSupport/EditorObjectLoader.h"
#include "EditorSupport/FontResourceHandler.h"
#include "PreprocessingPc/PcPreprocessor.h"
#endif

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_sort.h"

#include <tchar.h>
#include <algorithm>

namespace Lunar
{
    HELIUM_DECLARE_PTR( Camera );
    HELIUM_DECLARE_PTR( SkeletalMeshEntity );
    HELIUM_DECLARE_PTR( StaticMeshEntity );

    L_DECLARE_RPTR( RRenderContext );
    L_DECLARE_RPTR( RSurface );
}

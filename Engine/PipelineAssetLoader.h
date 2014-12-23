#pragma once

#include "Foundation/IPCTCP.h"

#include "Engine/AssetLoader.h"
#include "Engine/PipelinePackageLoaderMap.h"

namespace Helium
{
	class PipelinePackageLoader;

	/// Asset loader for loading objects from pipeline server.
	class HELIUM_ENGINE_API PipelineAssetLoader : public AssetLoader
	{
	public:
		/// @name Construction/Destruction
		//@{
		PipelineAssetLoader();
		virtual ~PipelineAssetLoader();
		//@}

		/// @name Static Initialization
		//@{
		static bool InitializeStaticInstance();
		//@}

		static PipelineAssetLoader *GetPipelineStaticInstance()
		{
			return sm_pPipelineInstance;
		}

		PipelinePackageLoader *GetPipelinePackageLoader( AssetPath path );

	private:
		PipelinePackageLoaderMap m_packageLoaderMap;

		/// @name Loading Implementation
		//@{
		virtual PackageLoader* GetPackageLoader( AssetPath path );
		virtual void TickPackageLoaders();
		//@}

		static PipelineAssetLoader* sm_pPipelineInstance;
		Helium::IPC::TCPConnection m_Connection;
	};
}

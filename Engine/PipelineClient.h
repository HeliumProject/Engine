#pragma once

#include "Foundation/IPCTCP.h"

#include "Engine/AssetLoader.h"
#include "Engine/PipelinePackageLoaderMap.h"

namespace Helium
{
	class PipelinePackageLoader;

	/// Asset loader for loading objects from pipeline server.
	class HELIUM_ENGINE_API PipelineClient
	{
	public:
		PipelineClient();
		virtual ~PipelineClient();

		static bool InitializeStaticInstance();
		static void DestroyStaticInstance();

		static PipelineClient* GetStaticInstance()
		{
			return sm_pInstance;
		}

		void SubscribePackage( AssetPath path );
		void UnsubscribePackage( AssetPath path );

		size_t BeginRequestAssetData( AssetPath path );
		bool TryFinishRequestAssetData( size_t requestIndex );

		void Tick();

	private:
		/// Load request pool block size.
		static const size_t MAX_CONCURRENT_DATA_REQUESTS = 16;

		struct SubscriptionState
		{
			uint64_t m_MostRecentUpdateTimestamp;
			bool m_IsCurrentlySubscribed;
			bool m_IsWantingToBeSubscribed;
		};
		Map< AssetPath, SubscriptionState > m_Subscriptions;

		struct AssetDataRequest
		{
			//size_t m_RequestId;
			AssetPath m_Path;
			String m_AssetData;
			uint64_t m_Timestamp;
			bool m_DataReceived;
		};

		/// Pending load requests.
		SparseArray< AssetDataRequest* > m_DataRequests;
		/// Load request pool.
		ObjectPool< AssetDataRequest > m_DataRequestPool;

		/// Singleton instance.
		static PipelineClient* sm_pInstance;

		Helium::IPC::TCPConnection m_Connection;
	};
}

#pragma once

#include "Application/InitializerStack.h"
#include "Foundation/Map.h"
#include "Engine/AssetPath.h"

namespace Helium
{
	namespace Pipeline
	{
		class PipelineEngine
		{
		public:
			static bool Init();
			static void Cleanup();

			static void Tick();

		private:
			static Helium::InitializerStack m_InitializerStack;
			static Helium::CallbackThread m_Thread;

			struct SubscriptionState
			{
				uint64_t m_MostRecentUpdateTimestamp;
				bool m_IsCurrentlySubscribed;
				bool m_TocSent;
			};

			struct DataRequest
			{
				size_t m_RequestId;
				AssetPath m_Path;
				size_t m_AsyncLoadRequest;
			};

			struct ClientState
			{
				Map< AssetPath, SubscriptionState > m_Subscriptions;
				DynamicArray< DataRequest > m_Requests;
			};
			static ClientState m_ClientState;
			


		};
	}
}

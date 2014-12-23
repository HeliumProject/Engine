
#include "PipelinePch.h"
#include "PipelineEngine.h"

#include "Foundation/MemoryStream.h"
#include "Foundation/IPCTCP.h"

#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

#include "Persist/ArchiveJson.h"

#include "Engine/FileLocations.h"
#include "Engine/AsyncLoader.h"
#include "Engine/AssetLoader.h"
#include "Engine/CacheManager.h"
#include "Engine/Config.h"
#include "Engine/Asset.h"
#include "Engine/PipelineMessages.h"
#include "Engine/PackageLoader.h"

#include "PcSupport/AssetPreprocessor.h"
#include "PcSupport/ConfigPc.h"
#include "PcSupport/LooseAssetLoader.h"
#include "PcSupport/PlatformPreprocessor.h"

#include "PreprocessingPc/PcPreprocessor.h"

#include "Graphics/Shader.h"

using namespace Helium;
using namespace Helium::Pipeline;

Helium::InitializerStack PipelineEngine::m_InitializerStack;
Helium::CallbackThread Helium::Pipeline::PipelineEngine::m_Thread;
Helium::Pipeline::PipelineEngine::ClientState Helium::Pipeline::PipelineEngine::m_ClientState;



Helium::IPC::TCPConnection g_ConnectionServer;

void ThreadFn(void *)
{
	while (true)
	{
		PipelineEngine::Tick();
	}
}

bool PipelineEngine::Init()
{
	// Register shutdown for general systems.
	m_InitializerStack.Push( FileLocations::Shutdown );
	m_InitializerStack.Push( Name::Shutdown );
	m_InitializerStack.Push( AssetPath::Shutdown );

	// Async I/O.
	AsyncLoader& asyncLoader = AsyncLoader::GetStaticInstance();
	HELIUM_VERIFY( asyncLoader.Initialize() );
	m_InitializerStack.Push( AsyncLoader::DestroyStaticInstance );

	// Asset cache management.
	FilePath baseDirectory;
	if ( !FileLocations::GetBaseDirectory( baseDirectory ) )
	{
		HELIUM_TRACE( TraceLevels::Error, TXT( "Could not get base directory." ) );
		return false;
	}

	HELIUM_VERIFY( CacheManager::InitializeStaticInstance( baseDirectory ) );
	m_InitializerStack.Push( CacheManager::DestroyStaticInstance );

	m_InitializerStack.Push( Reflect::ObjectRefCountSupport::Shutdown );
	m_InitializerStack.Push( Asset::Shutdown );
	m_InitializerStack.Push( AssetType::Shutdown );
	m_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );

	// Asset loader and preprocessor.
	HELIUM_VERIFY( LooseAssetLoader::InitializeStaticInstance() );
	m_InitializerStack.Push( LooseAssetLoader::DestroyStaticInstance );

	AssetLoader* pAssetLoader = AssetLoader::GetStaticInstance();
	HELIUM_ASSERT( pAssetLoader );

	AssetPreprocessor* pAssetPreprocessor = AssetPreprocessor::CreateStaticInstance();
	HELIUM_ASSERT( pAssetPreprocessor );
	PlatformPreprocessor* pPlatformPreprocessor = new PcPreprocessor;
	HELIUM_ASSERT( pPlatformPreprocessor );
	pAssetPreprocessor->SetPlatformPreprocessor( Cache::PLATFORM_PC, pPlatformPreprocessor );

	m_InitializerStack.Push( AssetPreprocessor::DestroyStaticInstance );
	m_InitializerStack.Push( AssetTracker::DestroyStaticInstance );

	g_ConnectionServer.Initialize(true, "Pipeline", "0.0.0.0", 22222);

	// start read thread
	if (!m_Thread.Create( ThreadFn, NULL, TXT("IPC Read Thread")))
	{
		HELIUM_BREAK();
		return false;
	}

	return true;
}

void PipelineEngine::Cleanup()
{
	m_InitializerStack.Cleanup();
}

void PipelineEngine::Tick()
{
	IPC::Message *pMsgIn = NULL;
	while ( g_ConnectionServer.Receive( &pMsgIn, false ) == IPC::ConnectionState::Active 
		&& pMsgIn)
	{
		StaticMemoryStream stream( pMsgIn->GetData(), pMsgIn->GetSize() );
		Reflect::ObjectPtr object;
		Persist::ArchiveReaderJson::ReadFromStream( stream, object );

		if (object)
		{
			if (object->GetMetaClass() == MsgSubscriptionChange::s_MetaClass)
			{
				MsgSubscriptionChange &msg = *Reflect::AssertCast<MsgSubscriptionChange>(object.Get());
				AssetPath path(*msg.m_Path);

				// Received a subscription message!
				HELIUM_TRACE( TraceLevels::Info, "Received a subscription message for %s.\n", *msg.m_Path );

				Map< AssetPath, SubscriptionState >::Iterator iter = m_ClientState.m_Subscriptions.Find( path );

				if ( HELIUM_VERIFY( iter == m_ClientState.m_Subscriptions.End() ) )
				{
					m_ClientState.m_Subscriptions.Insert( iter, Map< AssetPath, SubscriptionState >::ValueType( path, SubscriptionState() ) );
					iter->Second().m_MostRecentUpdateTimestamp = msg.m_Timestamp;
					iter->Second().m_IsCurrentlySubscribed = true;
					iter->Second().m_IsCurrentlySubscribed = false;
				}
			}
			else if (object->GetMetaClass() == MsgRequestAssetData::s_MetaClass)
			{
				MsgRequestAssetData &msg = *Reflect::AssertCast<MsgRequestAssetData>(object.Get());
				AssetPath path(*msg.m_Path);

				// Received a subscription message!
				HELIUM_TRACE( TraceLevels::Info, "Received a data request message for %s.\n", *msg.m_Path );

				DataRequest *pRequest = m_ClientState.m_Requests.New();
				pRequest->m_Path = path;
				pRequest->m_RequestId = msg.m_RequestId;

				AssetLoader::GetStaticInstance()

				//AssetLoader::GetStaticInstance()->LoadObject(path, object);




				//AsyncLoader::GetStaticInstance().QueueRequest()
				//AsyncLoader::GetStaticInstance().TrySyncRequest()
			}
		}
	}

	for ( Map< AssetPath, SubscriptionState >::Iterator iter = m_ClientState.m_Subscriptions.Begin(); iter != m_ClientState.m_Subscriptions.End(); ++iter )
	{
		const AssetPath &path = iter->First();
		SubscriptionState &state = iter->Second();

		if (!state.m_TocSent)
		{
			Reflect::ObjectPtr object;
			AssetLoader::GetStaticInstance()->LoadObject(path, object);

			Package *pPackage = Reflect::SafeCast<Package>(object);

			MsgPackageTOCPtr tocMessagePtr(new MsgPackageTOC());
			tocMessagePtr->m_Path = path.ToString();

			if (pPackage)
			{
				PackageLoader *pLoader = pPackage->GetLoader();
				DynamicArray<AssetPath> paths;
				pLoader->EnumerateChildPackages(paths);

				for (DynamicArray<AssetPath>::Iterator assetIter = paths.Begin();
					assetIter != paths.End(); ++assetIter)
				{
					tocMessagePtr->m_ChildPackages.Push( assetIter->ToString() );
				}

				size_t assetCount = pLoader->GetAssetCount();

				for ( size_t assetIndex = 0; assetIndex < assetCount; ++assetIndex )
				{
					AssetPath path = pLoader->GetAssetPath( assetIndex );
					Name typeName = pLoader->GetAssetTypeName( assetIndex );
					AssetPath templatePath = pLoader->GetAssetTemplatePath( assetIndex );
					uint64_t timestamp = pLoader->GetAssetFileSystemTimestamp( assetIndex );

					PipelineAssetInfo *pInfo = tocMessagePtr->m_Assets.New();
					pInfo->m_Path = path.ToString();
					pInfo->m_Type = *typeName;
					pInfo->m_Template = templatePath.ToString();
					pInfo->m_Size = 0; // TODO: Populate this
					pInfo->m_Timestamp = timestamp;
				}
			}
			else
			{
				HELIUM_TRACE( TraceLevels::Warning, "Received a subscription message for a non package (%s)!.\n", *path.ToString() );
			}

			SendPipelineMessage(g_ConnectionServer, tocMessagePtr);

			state.m_TocSent = true;
		}
	}
}

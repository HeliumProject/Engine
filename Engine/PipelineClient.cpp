#include "EnginePch.h"
#include "PipelineClient.h"
#include "PipelineMessages.h"

#include "Platform/File.h"

#include "Reflect/Object.h"

#include "Persist/ArchiveJson.h"

#include "Engine/FileLocations.h"
#include "Engine/Config.h"
#include "Engine/Resource.h"
#include "Engine/PipelinePackageLoader.h"
#include "Engine/PipelineAssetLoader.h"

#include "Foundation/DirectoryIterator.h"
#include "Foundation/FilePath.h"
#include "Foundation/IPCTCP.h"
#include "Foundation/MemoryStream.h"


using namespace Helium;
using namespace Helium::Pipeline;

PipelineClient* Helium::PipelineClient::sm_pInstance;


/// Constructor.
PipelineClient::PipelineClient()
: m_DataRequestPool( MAX_CONCURRENT_DATA_REQUESTS )
{
	m_Connection.Initialize(false, "Pipeline", "127.0.0.1", 22222);
}

/// Destructor.
PipelineClient::~PipelineClient()
{
}

/// Initialize the static object loader instance as an PipelineClient.
///
/// @return  True if the loader was initialized successfully, false if not or another object loader instance already
///          exists.
bool PipelineClient::InitializeStaticInstance()
{
	if( sm_pInstance )
	{
		return false;
	}

	sm_pInstance = new PipelineClient;
	HELIUM_ASSERT( sm_pInstance );

	return true;
}

void Helium::PipelineClient::DestroyStaticInstance()
{
	delete sm_pInstance;
	sm_pInstance = NULL;
}

void Helium::PipelineClient::SubscribePackage( AssetPath path )
{
	Map< AssetPath, SubscriptionState >::Iterator iter = m_Subscriptions.Find( path );

	if ( HELIUM_VERIFY( iter == m_Subscriptions.End() ) )
	{
		m_Subscriptions.Insert( iter, Map< AssetPath, SubscriptionState >::ValueType( path, SubscriptionState() ) );
		iter->Second().m_MostRecentUpdateTimestamp = 0;
		iter->Second().m_IsCurrentlySubscribed = false;
		iter->Second().m_IsWantingToBeSubscribed = true;
	}
}

void Helium::PipelineClient::UnsubscribePackage( AssetPath path )
{
	Map< AssetPath, SubscriptionState >::Iterator iter = m_Subscriptions.Find( path );

	if ( HELIUM_VERIFY( iter != m_Subscriptions.End() ) )
	{
		m_Subscriptions.Remove( iter );
	}
}

void Helium::PipelineClient::Tick()
{
	IPC::Message *pMsgIn = NULL;
	while ( m_Connection.Receive( &pMsgIn, false ) == IPC::ConnectionState::Active 
		&& pMsgIn)
	{
		StaticMemoryStream stream( pMsgIn->GetData(), pMsgIn->GetSize() );
		Reflect::ObjectPtr object;
		Persist::ArchiveReaderJson::ReadFromStream( stream, object );

		if (object)
		{
			if (object->GetMetaClass() == MsgPackageTOC::s_MetaClass)
			{
				MsgPackageTOC &msg = *Reflect::AssertCast<MsgPackageTOC>(object.Get());
				AssetPath path(*msg.m_Path);

				// Received a subscription message!
				HELIUM_TRACE( TraceLevels::Info, "Received a TOC message for %s.\n", *msg.m_Path );

				PipelineAssetLoader *pAssetLoader = PipelineAssetLoader::GetPipelineStaticInstance();
				PipelinePackageLoader *pPackageLoader = pAssetLoader->GetPipelinePackageLoader(path);
				
				if (pPackageLoader)
				{
					pPackageLoader->UpdateChildPackages( msg.m_ChildPackages );

					for ( DynamicArray< Pipeline::PipelineAssetInfo >::Iterator iter = msg.m_Assets.Begin();
						iter != msg.m_Assets.End(); ++iter )
					{
						pPackageLoader->UpdateAssetInfo(*iter);
					}

					pPackageLoader->UpdateAssetInfoComplete();
				}
			}
			else if (object->GetMetaClass() == MsgAssetData::s_MetaClass)
			{
				MsgAssetData &msg = *Reflect::AssertCast<MsgAssetData>(object.Get());
				AssetPath path(*msg.m_Path);

				// Received a subscription message!
				HELIUM_TRACE( TraceLevels::Info, "Received a data message for %s.\n", *msg.m_Path );

				AssetDataRequest *pRequest = m_DataRequests.GetElement( msg.m_RequestId );
				if ( !pRequest )
				{
					HELIUM_TRACE( TraceLevels::Warning, "Received a data message %s with request ID %d that was not expected.\n", *msg.m_Path, msg.m_RequestId );
				}
				else if ( pRequest->m_Path != path )
				{
					HELIUM_TRACE( TraceLevels::Warning, "Received a data message %s with request ID %d but was expecting data for %s.\n", *msg.m_Path, msg.m_RequestId, pRequest->m_Path );
				}
				else
				{
					pRequest->m_AssetData = msg.m_AssetData;
					pRequest->m_Timestamp = msg.m_Timestamp;
					pRequest->m_DataReceived = true;
				}
			}
		}
	}

	for ( Map< AssetPath, SubscriptionState >::Iterator iter = m_Subscriptions.Begin(); iter != m_Subscriptions.End(); ++iter )
	{
		const AssetPath &path = iter->First();
		SubscriptionState &state = iter->Second();

		// If the connection is not good, then consider the subscription inactive
		if ( m_Connection.GetState() != IPC::ConnectionState::Active )
		{
			state.m_IsCurrentlySubscribed = false;
		}
		// We are connected, so if our current state doesn't match the desired state, then deal with it
		else if ( state.m_IsCurrentlySubscribed != state.m_IsWantingToBeSubscribed )
		{
			MsgSubscribeToPackagePtr pMessageOut( new MsgSubscriptionChange() );
			pMessageOut->m_Path = path.ToString();
			pMessageOut->m_Subscribe = state.m_IsWantingToBeSubscribed;
			pMessageOut->m_Timestamp = state.m_MostRecentUpdateTimestamp;
			Pipeline::SendPipelineMessage( m_Connection, pMessageOut );

			state.m_IsCurrentlySubscribed = state.m_IsWantingToBeSubscribed;
		}
	}
}

size_t Helium::PipelineClient::BeginRequestAssetData( AssetPath path )
{
	AssetDataRequest *pRequest = m_DataRequestPool.Allocate();
	size_t requestId = m_DataRequests.Add( pRequest );

	pRequest->m_Path = path;
	pRequest->m_DataReceived = false;
	pRequest->m_Timestamp = 0;
	pRequest->m_AssetData.Clear();

	MsgRequestAssetDataPtr pMessageOut( new MsgRequestAssetData() );
	pMessageOut->m_Path = path.ToString();
	pMessageOut->m_RequestId = requestId;

	Pipeline::SendPipelineMessage( m_Connection, pMessageOut );
	return requestId;
}

bool Helium::PipelineClient::TryFinishRequestAssetData( size_t requestIndex )
{
	AssetDataRequest *pRequest = m_DataRequests.GetElement( requestIndex );
	if ( !pRequest )
	{
		return false;
	}

	if ( !pRequest->m_DataReceived )
	{
		return false;
	}

	pRequest->m_DataReceived = false;
	pRequest->m_Path.Clear();
	pRequest->m_Timestamp = 0;
	pRequest->m_AssetData.Clear();

	m_DataRequests.Remove( requestIndex );
	m_DataRequestPool.Release( pRequest );

	return true;
}

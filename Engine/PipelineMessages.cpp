#include "EnginePch.h"
#include "PipelineMessages.h"

#include "Platform/File.h"

#include "Foundation/FilePath.h"
#include "Foundation/DirectoryIterator.h"
#include "Foundation/MemoryStream.h"
#include "Foundation/IPC.h"

#include "Engine/FileLocations.h"
#include "Engine/Config.h"
#include "Engine/Resource.h"
#include "Engine/PipelinePackageLoader.h"

#include "Persist/ArchiveJson.h"

using namespace Helium;
using namespace Helium::Pipeline;

HELIUM_DEFINE_CLASS( Helium::Pipeline::MsgSubscriptionChange );

void MsgSubscriptionChange::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &MsgSubscriptionChange::m_Path,			TXT( "m_Path" ) );
	comp.AddField( &MsgSubscriptionChange::m_Subscribe,		TXT( "m_Subscribe" ) );
	comp.AddField( &MsgSubscriptionChange::m_Timestamp,		TXT( "m_Timestamp" ) );
}

HELIUM_DEFINE_BASE_STRUCT( Helium::Pipeline::PipelineAssetInfo );

void Helium::Pipeline::PipelineAssetInfo::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &PipelineAssetInfo::m_Path,			TXT( "m_Path" ) );
	comp.AddField( &PipelineAssetInfo::m_Template,		TXT( "m_Template" ) );
	comp.AddField( &PipelineAssetInfo::m_Type,			TXT( "m_Type" ) );
	comp.AddField( &PipelineAssetInfo::m_Timestamp,		TXT( "m_Timestamp" ) );
	comp.AddField( &PipelineAssetInfo::m_Size,			TXT( "m_Size" ) );
}

bool PipelineAssetInfo::operator==( const PipelineAssetInfo& _rhs ) const
{
	return ( 
		m_Path == _rhs.m_Path &&
		m_Template == _rhs.m_Template &&
		m_Type == _rhs.m_Type &&
		m_Timestamp == _rhs.m_Timestamp &&
		m_Size == _rhs.m_Size
		);
}

bool PipelineAssetInfo::operator!=( const PipelineAssetInfo& _rhs ) const
{
	return !( *this == _rhs );
}     

HELIUM_DEFINE_CLASS( Helium::Pipeline::MsgPackageTOC );

void Helium::Pipeline::MsgPackageTOC::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &MsgPackageTOC::m_Path,			TXT( "m_Path" ) );
	comp.AddField( &MsgPackageTOC::m_Assets,		TXT( "m_Assets" ) );
	comp.AddField( &MsgPackageTOC::m_ChildPackages,	TXT( "m_ChildPackages" ) );
}

HELIUM_DEFINE_CLASS( Helium::Pipeline::MsgRequestAssetData );

void MsgRequestAssetData::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &MsgRequestAssetData::m_Path,		TXT( "m_Path" ) );
	comp.AddField( &MsgRequestAssetData::m_RequestId,	TXT( "m_RequestId" ) );
}

HELIUM_DEFINE_CLASS( Helium::Pipeline::MsgAssetData );

void MsgAssetData::PopulateMetaType( Reflect::MetaStruct& comp )
{
	comp.AddField( &MsgAssetData::m_Path,		TXT( "m_Path" ) );
	comp.AddField( &MsgAssetData::m_RequestId,	TXT( "m_RequestId" ) );
	comp.AddField( &MsgAssetData::m_Timestamp,	TXT( "m_Timestamp" ) );
	comp.AddField( &MsgAssetData::m_AssetData,	TXT( "m_AssetData" ) );
}

bool Helium::Pipeline::SendPipelineMessage(IPC::Connection &connection, Reflect::ObjectPtr object)
{
	DynamicArray<uint8_t> buffer;
	DynamicMemoryStream stream ( &buffer );
	Persist::ArchiveWriterJson::WriteToStream( object, stream );

	IPC::Message *pMsgOut = connection.CreateMessage( 0, static_cast<int>( buffer.GetSize() ) );
	memcpy( pMsgOut->GetData(), buffer.GetData(), buffer.GetSize() );

	if ( connection.Send( pMsgOut ) != IPC::ConnectionStates::Active )
	{
		delete pMsgOut;
		return false;
	}

	return true;
}
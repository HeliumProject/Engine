#pragma once

#include "Engine.h"
#include "Reflect/Object.h"

namespace Helium
{
	class PipelinePackageLoader;

	namespace IPC
	{
		class Connection;
	}

	namespace Pipeline
	{
		// Sent when the game wants to subscribe/unsubscribe to status of a particular package
		struct HELIUM_ENGINE_API MsgSubscriptionChange : Reflect::Object
		{
			HELIUM_DECLARE_CLASS( Helium::Pipeline::MsgSubscriptionChange, Reflect::Object );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			String m_Path;          // Path to the package in question
			bool m_Subscribe;		// True if subscribing, false if unsubscribing
			uint64_t m_Timestamp;	// Timestamp of last received data
		};
		typedef Helium::StrongPtr< MsgSubscriptionChange > MsgSubscribeToPackagePtr;

		struct HELIUM_ENGINE_API PipelineAssetInfo : Reflect::Struct
		{
			HELIUM_DECLARE_BASE_STRUCT(Helium::Pipeline::PipelineAssetInfo);
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			inline bool operator==( const PipelineAssetInfo& _rhs ) const;
			inline bool operator!=( const PipelineAssetInfo& _rhs ) const;

			String m_Path;
			String m_Template;
			String m_Type;
			uint64_t m_Timestamp;
			uint64_t m_Size;
		};

		// Sent when pipeline receives a subscribe request
		struct HELIUM_ENGINE_API MsgPackageTOC : Reflect::Object
		{
			HELIUM_DECLARE_CLASS( Helium::Pipeline::MsgPackageTOC, Reflect::Object );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			String m_Path;								// Package path
			DynamicArray<PipelineAssetInfo> m_Assets;   // One entry per asset in the array
			DynamicArray<String> m_ChildPackages;
		};
		typedef Helium::StrongPtr< MsgPackageTOC > MsgPackageTOCPtr;

		// Sent when pipeline receives a subscribe request
		struct HELIUM_ENGINE_API MsgRequestAssetData : Reflect::Object
		{
			HELIUM_DECLARE_CLASS( Helium::Pipeline::MsgRequestAssetData, Reflect::Object );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			String m_Path;
			size_t m_RequestId;
		};
		typedef Helium::StrongPtr< MsgRequestAssetData > MsgRequestAssetDataPtr;

		// Sent when pipeline receives a subscribe request
		struct HELIUM_ENGINE_API MsgAssetData : Reflect::Object
		{
			HELIUM_DECLARE_CLASS( Helium::Pipeline::MsgAssetData, Reflect::Object );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

			String m_Path;
			size_t m_RequestId;
			uint64_t m_Timestamp;
			String m_AssetData;

		};
		typedef Helium::StrongPtr< MsgAssetData > MsgAssetDataPtr;

		// Utility function
		HELIUM_ENGINE_API bool SendPipelineMessage(Helium::IPC::Connection &connection, Reflect::ObjectPtr object);
	}
}

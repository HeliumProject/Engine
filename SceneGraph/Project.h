#pragma once

#include "Foundation/Attribute.h"
#include "Application/DocumentManager.h"

#include "Reflect/Object.h"
#include "Reflect/TranslatorDeduction.h"

#include "SceneGraph/API.h"

namespace Helium
{
	class HELIUM_SCENE_GRAPH_API Project : public Reflect::Object
	{
	public:
		Project( const FilePath& path = FilePath( "" ) );

		const FilePath& GetPath();
		void SetPath( const FilePath& path );

		const std::set< FilePath >& GetPaths();
		void AddPath( const FilePath& path );
		void RemovePath( const FilePath& path );

		void ConnectDocument( Document* document );
		void DisconnectDocument( const Document* document );

		void OnDocumentOpened( const DocumentEventArgs& args );
		void OnDocumenClosed( const DocumentEventArgs& args );
		void OnDocumentSave( const DocumentEventArgs& args );
		void OnDocumentPathChanged( const DocumentPathChangedArgs& args );
		void OnChildDocumentPathChanged( const DocumentPathChangedArgs& args );

		bool Serialize() const;

	public:
		Helium::Event< const FilePath& > e_PathAdded;
		Helium::Event< const FilePath& > e_PathRemoved;

		mutable DocumentObjectChangedSignature::Event e_HasChanged;

	protected:
		FilePath             m_Path;
		std::set< FilePath > m_Paths;

	public:
		HELIUM_DECLARE_CLASS( Project, Reflect::Object );
		static void PopulateMetaType( Reflect::MetaStruct& comp );
	};

	typedef Helium::StrongPtr<Project> ProjectPtr;
}
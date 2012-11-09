#pragma once

#include "Platform/Types.h"
#include "Platform/File.h"

#include "Foundation/API.h"
#include "Foundation/Event.h"
#include "Foundation/FilePath.h"

namespace Helium
{
	namespace DirectoryFlags
	{
		enum Type
		{
			Default			= 0,
			SkipFiles       = 1 << 0,          // Skip files
			SkipDirectories = 1 << 1,          // Skip directories
			RelativePath    = 1 << 2,          // Don't preped each file with the root path
		};
	}

	struct HELIUM_FOUNDATION_API DirectoryIteratorItem
	{
		DirectoryIteratorItem()
			: m_CreateTime ( 0x0 )
			, m_ModTime ( 0x0 )
			, m_Size( 0x0 )
		{

		}

		void Clear()
		{
			m_Path.Clear();
			m_CreateTime = 0x0;
			m_ModTime = 0x0;
			m_Size = 0x0;
		}

		Path      m_Path;
		uint64_t  m_CreateTime;
		uint64_t  m_ModTime;
		uint64_t  m_Size;
	};

	class HELIUM_FOUNDATION_API DirectoryIterator
	{
	public:
		DirectoryIterator();
		DirectoryIterator( const Path& path, uint32_t flags = DirectoryFlags::Default );
		~DirectoryIterator();

		bool IsDone();
		bool Next();
		const DirectoryIteratorItem& GetItem();

		void Reset();
		bool Open(const Path& path, uint32_t flags = DirectoryFlags::Default);

		static void GetFiles( const Path& path, std::set< Helium::Path >& paths, bool recursive = false );
		void GetFiles( std::set< Helium::Path >& paths, bool recursive = false );

	private:
		bool Find();
		void Close();

		Path					m_Path;
		uint32_t				m_Flags;
		Directory				m_Directory;
		DirectoryIteratorItem	m_Item;
		bool					m_Done;
	};

	typedef Helium::Signature< const DirectoryIteratorItem& > DirectoryItemSignature;

	HELIUM_FOUNDATION_API void RecurseDirectories( DirectoryItemSignature::Delegate delegate, const Path& path, uint32_t flags = DirectoryFlags::Default);
}
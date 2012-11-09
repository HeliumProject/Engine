#pragma once

#include "Platform/API.h"
#include "Platform/File.h"
#include "Platform/Status.h"
#include "Platform/Types.h"
#include "Platform/Utility.h"

namespace Helium
{
	class HELIUM_PLATFORM_API DirectoryEntry
	{
	public:
		DirectoryEntry( const tstring& name = TXT( "" ) );

		tstring	m_Name;
		Status	m_Stat;
	};

	class HELIUM_PLATFORM_API Directory : NonCopyable
	{
	public:
		Directory( const tstring& path = TXT( "" ) );
		~Directory();

		bool IsOpen();
		bool FindFirst( DirectoryEntry& entry );
		bool FindNext( DirectoryEntry& entry );
		bool Close();

		tstring	m_Path;

	private:
#if HELIUM_OS_WIN
		void*	m_Handle;
#else
#error Implement Directory for this platform.
#endif
	};
}
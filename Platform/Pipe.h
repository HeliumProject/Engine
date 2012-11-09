#pragma once

#include "Platform/Condition.h"

const static uint32_t IPC_PIPE_BUFFER_SIZE = 8192;

namespace Helium
{
	class HELIUM_PLATFORM_API Pipe
	{
	public:
		Pipe();
		~Pipe();

		bool Create(const tchar_t* name);
		bool Open(const tchar_t* name);
		void Close();

		bool Connect(Condition& terminate);
		void Disconnect();

		bool Read(void* buffer, uint32_t bytes, uint32_t& read, Condition& terminate);
		bool Write(void* buffer, uint32_t bytes, uint32_t& wrote, Condition& terminate);

	private:
#ifdef HELIUM_OS_WIN
		typedef void* Handle;
		struct Overlapped
		{
			uint32_t* Internal;
			uint32_t* InternalHigh;
			union
			{
				struct
				{
					uint32_t Offset;
					uint32_t OffsetHigh;
				};

				void* Pointer;
			};
			void* hEvent;
		} m_Overlapped;
#else
#error Implement Pipe for this platform.
#endif
		Handle m_Handle;
	};

	HELIUM_PLATFORM_API bool InitializePipes();
	HELIUM_PLATFORM_API void CleanupPipes();
}

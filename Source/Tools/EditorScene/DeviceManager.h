#pragma once

#include "Foundation/Event.h"
#include "Rendering/Renderer.h"
#include "EditorScene/API.h"

#define MAX_DEVICE_COUNT 32

namespace Helium
{
	HELIUM_DECLARE_RPTR( RRenderContext );

	namespace DeviceStates
	{
		enum DeviceState
		{
			Found,
			Lost
		};
	}
	typedef DeviceStates::DeviceState DeviceState;

	struct DeviceStateArgs
	{
		DeviceState m_DeviceState;

		DeviceStateArgs( DeviceState state )
			: m_DeviceState( state )
		{
		}
	};
	typedef Helium::Signature< const DeviceStateArgs& > DeviceStateSignature;

	// all rendering classes should be derived this
	class HELIUM_EDITOR_SCENE_API DeviceManager : NonCopyable
	{
	public:
		DeviceManager();
		virtual ~DeviceManager();

		static void SetUnique(); // call before init
		bool Init( void* hwnd, uint32_t back_buffer_width, uint32_t back_buffer_height, uint32_t init_flags = 0 );

	private:
		bool ResizeSwapChain( uint32_t width, uint32_t height );
		bool ResizeDevice( uint32_t width, uint32_t height );
	public:
		bool Resize( uint32_t width, uint32_t height );
		bool Swap();

		bool TestDeviceReady();

		inline uint32_t GetWidth()
		{
			return m_Width;
		}

		inline uint32_t GetHeight()
		{
			return m_Height;
		}

	private:
		/// Window to which rendering is performed.
		void*                                   m_hWnd;
		/// Render context.
		Helium::RRenderContextPtr               m_spRenderContext;

		bool                                    m_UsingSwapchain;
		uint32_t                                m_Width;
		uint32_t                                m_Height;

		/// Main render context.
		static Helium::RRenderContextPtr        sm_spMainRenderContext;
		/// Main render context window handle.
		static void*                            sm_hMainRenderContextWnd;
		/// Main render context width.
		static uint32_t                         sm_mainRenderContextWidth;
		/// Main render context heigth.
		static uint32_t                         sm_mainRenderContextHeight;

		static bool                             m_Unique;
		static uint32_t                         m_InitCount;
		static DeviceManager*                   m_Clients[MAX_DEVICE_COUNT];
	};
}
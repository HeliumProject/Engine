require "Base"

configuration {}

defines
{
	"PLATFORM_DLL=1",
	"FOUNDATION_DLL=1",
	"PIPELINE_DLL=1",
	"WXUSINGDLL=1",
	"wxNO_EXPAT_LIB=1",
	"wxNO_JPEG_LIB=1",
	"wxNO_PNG_LIB=1",
	"wxNO_TIFF_LIB=1",
	"wxNO_ZLIB_LIB=1",
}

flags
{
	"FatalWarnings",
}

includedirs
{
	".",
	"Dependencies/boost",
	"Dependencies/Expat",
	"Dependencies/nvtt/src",
	"Dependencies/png",
	"Dependencies/tbb/include",
	"Dependencies/zlib",
	"Integrations/FBX/include",
}

libdirs
{
	"Integrations/FBX/lib/vs2008",
}

if haveGranny then
	includedirs
	{
		"Integrations/Granny",
		"Integrations/Granny/granny_sdk/include",
	}

	defines
	{
		"L_HAVE_GRANNY=1",
	}
else
	defines
	{
		"L_HAVE_GRANNY=0",
	}
end

--[[
We build monolithic wx, so ignore all the legacy non-monolithic
#pragma comment directives (on windows only)
--]]
configuration "windows"
	linkoptions
	{
		"/NODEFAULTLIB:wxbase29ud",
		"/NODEFAULTLIB:wxbase29d",
		"/NODEFAULTLIB:wxbase29u",
		"/NODEFAULTLIB:wxbase29",
		"/NODEFAULTLIB:wxbase29ud_net",
		"/NODEFAULTLIB:wxbase29d_net",
		"/NODEFAULTLIB:wxbase29u_net",
		"/NODEFAULTLIB:wxbase29_net",
		"/NODEFAULTLIB:wxbase29ud_xml",
		"/NODEFAULTLIB:wxbase29d_xml",
		"/NODEFAULTLIB:wxbase29u_xml",
		"/NODEFAULTLIB:wxbase29_xml",
		"/NODEFAULTLIB:wxmsw29ud_core",
		"/NODEFAULTLIB:wxmsw29d_core",
		"/NODEFAULTLIB:wxmsw29u_core",
		"/NODEFAULTLIB:wxmsw29_core",
		"/NODEFAULTLIB:wxmsw29ud_adv",
		"/NODEFAULTLIB:wxmsw29d_adv",
		"/NODEFAULTLIB:wxmsw29u_adv",
		"/NODEFAULTLIB:wxmsw29_adv",
		"/NODEFAULTLIB:wxmsw29ud_html",
		"/NODEFAULTLIB:wxmsw29d_html",
		"/NODEFAULTLIB:wxmsw29u_html",
		"/NODEFAULTLIB:wxmsw29_html",
		"/NODEFAULTLIB:wxmsw29ud_qa",
		"/NODEFAULTLIB:wxmsw29d_qa",
		"/NODEFAULTLIB:wxmsw29u_qa",
		"/NODEFAULTLIB:wxmsw29_qa",
		"/NODEFAULTLIB:wxmsw29ud_xrc",
		"/NODEFAULTLIB:wxmsw29d_xrc",
		"/NODEFAULTLIB:wxmsw29u_xrc",
		"/NODEFAULTLIB:wxmsw29_xrc",
		"/NODEFAULTLIB:wxmsw29ud_aui",
		"/NODEFAULTLIB:wxmsw29d_aui",
		"/NODEFAULTLIB:wxmsw29u_aui",
		"/NODEFAULTLIB:wxmsw29_aui",
		"/NODEFAULTLIB:wxmsw29ud_propgrid",
		"/NODEFAULTLIB:wxmsw29d_propgrid",
		"/NODEFAULTLIB:wxmsw29u_propgrid",
		"/NODEFAULTLIB:wxmsw29_propgrid",
		"/NODEFAULTLIB:wxmsw29ud_ribbon",
		"/NODEFAULTLIB:wxmsw29d_ribbon",
		"/NODEFAULTLIB:wxmsw29u_ribbon",
		"/NODEFAULTLIB:wxmsw29_ribbon",
		"/NODEFAULTLIB:wxmsw29ud_richtext",
		"/NODEFAULTLIB:wxmsw29d_richtext",
		"/NODEFAULTLIB:wxmsw29u_richtext",
		"/NODEFAULTLIB:wxmsw29_richtext",
		"/NODEFAULTLIB:wxmsw29ud_media",
		"/NODEFAULTLIB:wxmsw29d_media",
		"/NODEFAULTLIB:wxmsw29u_media",
		"/NODEFAULTLIB:wxmsw29_media",
		"/NODEFAULTLIB:wxmsw29ud_stc",
		"/NODEFAULTLIB:wxmsw29d_stc",
		"/NODEFAULTLIB:wxmsw29u_stc",
		"/NODEFAULTLIB:wxmsw29_stc",
		"/NODEFAULTLIB:wxmsw29ud_gl",
		"/NODEFAULTLIB:wxmsw29d_gl",
		"/NODEFAULTLIB:wxmsw29u_gl",
		"/NODEFAULTLIB:wxmsw29_gl",
	}
	includedirs
	{
		os.getenv( "DXSDK_DIR" ) .. "Include"
	}
	
configuration "no-unicode"
	defines
	{
		"wxUSE_UNICODE=0",
	}
	
configuration "not no-unicode"
	defines
	{
		"wxUSE_UNICODE=1",
	}

configuration { "windows", "x32" }
	libdirs
	{
		os.getenv( "DXSDK_DIR" ) .. "Lib/x86",
	}

	if haveGranny then
		libdirs
		{
			"Integrations/Granny/granny_sdk/lib/win32",
		}
	end


configuration { "windows", "x64" }
	libdirs
	{
		os.getenv( "DXSDK_DIR" ) .. "Lib/x64",
	}

	if haveGranny then
		libdirs
		{
			"Integrations/Granny/granny_sdk/lib/win64",
		}
	end

configuration { "windows", "x32", "Debug" }
	libdirs
	{
		"Dependencies/tbb/build/windows_ia32_cl_vc9_debug",
	}

configuration { "windows", "x32", "not Debug" }
	libdirs
	{
		"Dependencies/tbb/build/windows_ia32_cl_vc9_release",
	}

configuration { "windows", "x64", "Debug" }
	libdirs
	{
		"Dependencies/tbb/build/windows_intel64_cl_vc9_debug",
	}

configuration { "windows", "x64", "not Debug" }
	libdirs
	{
		"Dependencies/tbb/build/windows_intel64_cl_vc9_release",
	}

project "Platform"
	uuid "E4A1F8FC-A93A-46E2-9CA8-40C2CE1B163E"
	language "C++"
	defines
	{
		"PLATFORM_EXPORTS",
		"HELIUM_MODULE_HEAP_FUNCTION=GetPlatformDefaultHeap",
	}
	files
	{
		"Platform/*",
		"Platform/Gcc/*",
		"Platform/Math/*",
		"Platform/Msc/*",
		"Platform/X86/*",
	}

	configuration "windows"
		files
		{
			"Platform/Math/Simd/*",
			"Platform/Math/Simd/Sse/*",
			"Platform/Windows/*",
		}
	configuration "macosx"
		files
		{
			"Platform/Math/Simd/*",
			"Platform/Math/Simd/Sse/*",
			"Platform/POSIX/*",
		}
	configuration "linux"
		files
		{
			"Platform/Math/Simd/*",
			"Platform/Math/Simd/Sse/*",
			"Platform/POSIX/*",
		}

	configuration "Debug"
		kind "SharedLib"

	configuration "not Debug"
		kind "StaticLib"

	configuration { "windows", "SharedLib" }
		links
		{
			"ws2_32",
		}

project "Foundation"
	uuid "9708463D-9698-4BB6-A911-37354AF0E21E"
	language "C++"
	defines
	{
		"FOUNDATION_EXPORTS",
		"HELIUM_MODULE_HEAP_FUNCTION=GetFoundationDefaultHeap",
	}
	files
	{
		"Foundation/**",
		"Foundation/**",
	}

	configuration "Debug"
		kind "SharedLib"

	configuration "not Debug"
		kind "StaticLib"

	configuration "SharedLib"
		links
		{
			"Platform",
			"Expat",
			"zlib",
		}

	configuration { "windows", "SharedLib" }
		links
		{
			"ws2_32",
		}

project "Pipeline"
	uuid "50F5AA7E-22D9-4D33-B48A-357CD3082BC1"
	language "C++"
	defines
	{
		"PIPELINE_EXPORTS",
		"HELIUM_MODULE_HEAP_FUNCTION=GetPipelineDefaultHeap",
	}
	includedirs
	{
		"Dependencies/nvtt",
		"Dependencies/nvtt/src",
		"Dependencies/nvtt/src/nvtt/squish",
		"Dependencies/tiff",
		"Dependencies/tiff/libtiff",
	}
	files
	{
		"Pipeline/**.h",
		"Pipeline/**.cpp",
	}

	configuration "windows"
		includedirs
		{
			"Dependencies/nvtt/project/vc8",
		}

	configuration "Debug"
		kind "SharedLib"

	configuration "not Debug"
		kind "StaticLib"

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"nvtt",
			"tiff",
			"d3d9",
			"d3dx9",
		}

project "Editor"
	uuid "A5CAC2F6-62BC-4EF3-A752-887F89C64812"
	kind "ConsoleApp"
	language "C++"
	defines
	{
		"HELIUM_MODULE_HEAP_FUNCTION=GetEditorDefaultHeap",
	}
	files
	{
		"Editor/**.h",
		"Editor/**.c",
		"Editor/**.cpp",
		"Editor/Editor.rc",
	}
--	pchheader( "Precompile.h" )
--	pchsource( "Precompile.cpp" )
	includedirs
	{
		"Editor",
		"Dependencies/wxWidgets/include",
		"Dependencies/LiteSQL/include",
		"Dependencies/p4api/include",
		"Dependencies/lua/src",
	}
	links
	{
		"Platform",
		"Foundation",
		"Pipeline",
		"Expat",
		"zlib",
		"LiteSQL",
		"libclient",
		"librpc",
		"libsupp",
		"lua",
		"d3d9",
		"d3dx9",
		"ws2_32",
	}
	flags
	{
		"WinMain"
	}

	configuration "windows"
		includedirs
		{
			"Dependencies/wxWidgets/include/msvc",
		}
		
	-- per architecture
	configuration { "windows", "x32" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_dll",
		}
	configuration { "windows", "x64" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_amd64_dll",
		}
		
	-- per configuration
	configuration { "windows", "Debug", "no-unicode" }
		links
		{
			"wxmsw29d"
		}
	configuration { "windows", "not Debug", "no-unicode" }
		links
		{
			"wxmsw29"
		}
	configuration { "windows", "Debug", "not no-unicode" }
		links
		{
			"wxmsw29ud"
		}
	configuration { "windows", "not Debug", "not no-unicode" }
		links
		{
			"wxmsw29u"
		}
		
	-- per architecture, per configuration
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/Win32/Debug",
		}
	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/Win32/Release",
		}
	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/x64/Debug",
		}
	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/x64/Release",
		}

project "Engine"
	uuid "CDD089F1-EC6E-469B-BF06-8DF56C5B1489"


	Helium.DoLunarModuleProjectSettings( "LUNAR", "Engine", "ENGINE" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
		}

project "EngineJobs"
	uuid "65CFFE89-3111-4D58-95DC-5DB6D3F28935"


	Helium.DoLunarModuleProjectSettings( "LUNAR", "EngineJobs", "ENGINE_JOBS" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
		}

project "Windowing"
	uuid "B68268DF-3942-432F-89B1-DBC82C21218E"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "Windowing", "WINDOWING" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
		}

project "Rendering"
	uuid "3F1BD209-272C-4833-AF8E-35C317F21452"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "Rendering", "RENDERING" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
		}

project "GraphicsTypes"
	uuid "4A13A4F6-6860-4F52-A217-B0C3943E7025"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "GraphicsTypes", "GRAPHICS_TYPES" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Rendering",
		}

project "GraphicsJobs"
	uuid "4D83346D-DCB2-40E6-AAF1-508341728E57"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "GraphicsJobs", "GRAPHICS_JOBS" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Rendering",
			"GraphicsTypes",
		}

project "Graphics"
	uuid "3342921C-F6C7-4A81-A6FF-1C93373AF285"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "Graphics", "GRAPHICS" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Rendering",
			"GraphicsTypes",
			"GraphicsJobs",
		}

project "Framework"
	uuid "6DB6B383-76E6-4361-8CFE-F08F1CFE24BE"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "Framework", "FRAMEWORK" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Windowing",
			"Rendering",
			"GraphicsTypes",
			"GraphicsJobs",
			"Graphics",
		}

project "WinWindowing"
	uuid "1D7B65F8-6A31-4E8C-AF91-C1D2FA73AD12"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "WinWindowing", "WIN_WINDOWING" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Windowing",
		}

project "D3D9Rendering"
	uuid "4BE28ED4-950D-469B-A6F8-88C09BA479E5"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "D3D9Rendering", "D3D9_RENDERING" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Rendering",
		}

project "PcSupport"
	uuid "2B3B921A-BFF1-4A73-A9DD-3FCACA9D2916"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "PcSupport", "PC_SUPPORT" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Rendering",
		}

project "PreprocessingPc"
	uuid "94E6A151-FC28-41EE-A5F3-D8629F6B8B3B"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "PreprocessingPc", "PREPROCESSING_PC" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Rendering",
			"GraphicsTypes",
			"GraphicsJobs",
			"Graphics",
			"PcSupport",
		}

project "EditorSupport"
	uuid "82F12FF0-CA4E-42E5-84A7-92A5C1A8AE26"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "EditorSupport", "EDITOR_SUPPORT" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Windowing",
			"Rendering",
			"GraphicsTypes",
			"GraphicsJobs",
			"Graphics",
			"Framework",
			"PcSupport",
			"PreprocessingPc",
		}

project "FrameworkWin"
	uuid "8F1B5E58-BDA5-447D-9FD4-36A3B23221B8"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "FrameworkWin", "FRAMEWORK_WIN" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
			"Windowing",
			"Rendering",
			"GraphicsTypes",
			"GraphicsJobs",
			"Graphics",
			"Framework",
			"WinWindowing",
			"D3D9Rendering",
			"PcSupport",
			"PreprocessingPc",
			"EditorSupport",
		}

project "TestJobs"  -- DEPRECATED
	uuid "12106586-0EB1-4D4C-9DFE-E3C63D3E4013"

	Helium.DoLunarModuleProjectSettings( "LUNAR", "TestJobs", "TEST_JOBS" )

	configuration "SharedLib"
		links
		{
			"Platform",
			"Foundation",
			"Engine",
			"EngineJobs",
		}

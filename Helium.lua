require "Base"

configuration {}

defines
{
	"PLATFORM_DLL=1",
	"FOUNDATION_DLL=1",
	"PIPELINE_DLL=1",
	"CORE_DLL=1",
	"XML_STATIC=1",
	"WXUSINGDLL=1",
	"wxUSE_UNICODE=0",
	"wxNO_EXPAT_LIB=1",
	"wxNO_JPEG_LIB=1",
	"wxNO_PNG_LIB=1",
	"wxNO_TIFF_LIB=1",
	"wxNO_ZLIB_LIB=1",
}

includedirs
{
	".",
}

buildoptions
{
	-- Class 'foo<>' needs to have dll-interface to be used by clients of class 'bar'
	--  This is a non-issue so long as debug/release and CRT is not mixed b/t modules
	"/wd4251",
}

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

project "Platform"
	kind "SharedLib"
	language "C++"
	defines
	{
		"PLATFORM_EXPORTS",
	}
	files
	{
		"Platform/*.h",
		"Platform/*.cpp",
	}

	configuration "windows"
		files
		{
			"Platform/Windows/*.h",
			"Platform/Windows/*.cpp",
		}
		links
		{
			"ws2_32",
		}
	configuration "macosx"
		files
		{
			"Platform/POSIX/*.h",
			"Platform/POSIX/*.cpp",
		}
	configuration "linux"
		files
		{
			"Platform/POSIX/*.h",
			"Platform/POSIX/*.cpp",
		}

project "Foundation"
	kind "SharedLib"
	language "C++"
	defines
	{
		"FOUNDATION_EXPORTS",
	}
	includedirs
	{
		"Dependencies/Expat",
		"Dependencies/zlib",
	}
	files
	{
		"Foundation/**.h",
		"Foundation/**.cpp",
	}
	links
	{
		"Platform",
		"ws2_32",
		"Expat",
		"zlib",
	}

project "Pipeline"
	kind "SharedLib"
	language "C++"
	defines
	{
		"PIPELINE_EXPORTS",
	}
	includedirs
	{
		"Dependencies/squish",
		"Dependencies/nvtt",
		"Dependencies/nvtt/src",
		"Dependencies/tiff",
		"Dependencies/tiff/libtiff",
	}
	files
	{
		"Pipeline/**.h",
		"Pipeline/**.cpp",
	}
	links
	{
		"Platform",
		"Foundation",
		"squish",
		"nvtt",
		"tiff",
	}

	configuration "windows"
		includedirs
		{
			"Dependencies/nvtt/project/vc8",
		}

project "Core"
	kind "SharedLib"
	language "C++"
	defines
	{
		"CORE_EXPORTS",
	}
	files
	{
		"Core/**.h",
		"Core/**.cpp",
	}
	links
	{
		"Foundation",
		"Platform",
		"opengl32",
		"glu32",
		"d3d9",
		"d3dx9",
	}	

	configuration "windows"
		includedirs
		{
			os.getenv( "DXSDK_DIR" ) .. "/include",
		}
	configuration { "windows", "x32" }
		libdirs
		{
			os.getenv( "DXSDK_DIR" ) .. "/lib/x86",
		}
	configuration { "windows", "x64" }
		libdirs
		{
			os.getenv( "DXSDK_DIR" ) .. "/lib/x64",
		}

project "Editor"
	kind "ConsoleApp"
	language "C++"
	files
	{
		"Editor/**.h",
		"Editor/**.c",
		"Editor/**.cpp",
	}
	includedirs
	{
		"Editor",
		"Dependencies/wxWidgets/include",
		"Dependencies/litesql/include",
		"Dependencies/p4api/include",
		"Dependencies/lua/src",
	}
	links
	{
		"Platform",
		"Foundation",
		"Pipeline",
		"Core",
		"litesql",
		"libclient",
		"librpc",
		"libsupp",
		"lua",
		"d3d9",
		"d3dx9",
		"ws2_32",
	}

	configuration "windows"
		includedirs
		{
			"Dependencies/wxWidgets/include/msvc",
			os.getenv( "DXSDK_DIR" ) .. "/include",
		}
		postbuildcommands
		{
			"robocopy /MIR /MT \"..\\Editor\\Icons\\Helium\" \"$(OutDir)\\Icons\"",
			"exit /B 0",
		}			
		
	-- per architecture
	configuration { "windows", "x32" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_dll",
			os.getenv( "DXSDK_DIR" ) .. "/lib/x86",
		}
	configuration { "windows", "x64" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_amd64_dll",
			os.getenv( "DXSDK_DIR" ) .. "/lib/x64"
		}
		
	-- per configuration
	configuration { "windows", "Debug" }
		links
		{
			"wxmsw29d"
		}
	configuration { "windows", "Release" }
		links
		{
			"wxmsw29"
		}
	configuration { "windows", "DebugUnicode" }
		links
		{
			"wxmsw29ud"
		}
	configuration { "windows", "ReleaseUnicode" }
		links
		{
			"wxmsw29u"
		}
		
	-- per architecture, per configuration
	configuration { "windows", "x32", "Debug*" }
		libdirs
		{
			"Dependencies/p4api/lib/Win32/Debug",
		}
	configuration { "windows", "x32", "Release*" }
		libdirs
		{
			"Dependencies/p4api/lib/Win32/Release",
		}
	configuration { "windows", "x64", "Debug*" }
		libdirs
		{
			"Dependencies/p4api/lib/x64/Debug",
		}
	configuration { "windows", "x64", "Release*" }
		libdirs
		{
			"Dependencies/p4api/lib/x64/Release",
		}

require "Helium"

defines
{
    "HELIUM_TOOLS=1",
}

prefix = "Tools."

dofile "Shared.lua"

project( prefix .. "Application" )
	uuid "870F919A-B4E8-4b15-93CA-FA211D68B609"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Application/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

		
project( prefix .. "PcSupport" )
	uuid "2B3B921A-BFF1-4A73-A9DD-3FCACA9D2916"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PcSupport", "PC_SUPPORT" )
	Helium.DoTbbProjectSettings()

	files
	{
		"PcSupport/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
		}

project( prefix .. "PreprocessingPc" )
	uuid "94E6A151-FC28-41EE-A5F3-D8629F6B8B3B"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PreprocessingPc", "PREPROCESSING_PC" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"PreprocessingPc/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "PcSupport",
		}

project( prefix .. "EditorSupport" )
	uuid "82F12FF0-CA4E-42E5-84A7-92A5C1A8AE26"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EditorSupport", "EDITOR_SUPPORT" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"EditorSupport/*",
	}

	includedirs
	{
		"Dependencies/nvtt",
		"Dependencies/nvtt/extern/poshlib",
		"Dependencies/nvtt/src",
		"Dependencies/nvtt/src/nvtt/squish",
		"Dependencies/freetype/include",
		"Dependencies/libpng",
	}

	if haveGranny then
		includedirs
		{
			"Integrations/Granny",
			"Integrations/Granny/granny_sdk/include",
		}
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
		}

	if haveGranny then
		configuration { "windows", "x32" }
			libdirs
			{
				"Integrations/Granny/granny_sdk/lib/win32",
			}
		configuration { "windows", "x64" }
			libdirs
			{
				"Integrations/Granny/granny_sdk/lib/win64",
			}
		configuration "x32"
			links
			{
				"granny2",
			}
		configuration "x64"
			links
			{
				"granny2_x64",
			}
	end

project( prefix .. "FrameworkWin" )
	uuid "8F1B5E58-BDA5-447D-9FD4-36A3B23221B8"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "FrameworkWin", "FRAMEWORK_WIN" )

	files
	{
		"FrameworkWin/*",
	}

	includedirs
	{
		"Dependencies/freetype/include",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "WindowingWin",
			prefix .. "RenderingD3D9",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
			prefix .. "EditorSupport",
		}
		
project( prefix .. "TestJobs" )-- DEPRECATED
	uuid "12106586-0EB1-4D4C-9DFE-E3C63D3E4013"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "TestJobs", "TEST_JOBS" )
	Helium.DoTbbProjectSettings()
	
	files
	{
		"TestJobs/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
		}

project( prefix .. "Inspect" )
	uuid "D4D7F216-5EE6-4252-BF25-0698C1BD30CD"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Inspect", "INSPECT" )

	files
	{
		"Inspect/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Application",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
		}

project( prefix .. "SceneGraph" )
	uuid "4EAB668E-2382-40d9-AA6B-7FB0BB163E7F"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "SceneGraph", "SCENE_GRAPH" )

	files
	{
		"SceneGraph/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Application",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Inspect",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "WindowingWin",
			prefix .. "RenderingD3D9",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
			prefix .. "EditorSupport",
		}

project( prefix .. "Editor" )
	uuid "A5CAC2F6-62BC-4EF3-A752-887F89C64812"
	kind "ConsoleApp"

	Helium.DoTbbProjectSettings()

	files
	{
		"Editor/**.h",
		"Editor/**.c",
		"Editor/**.cpp",
		"Editor/Editor.rc",
	}

	flags
	{
		"WinMain"
	}

	defines
	{
		"HELIUM_MODULE=Editor",
		"WXUSINGDLL=1",
		"wxNO_EXPAT_LIB=1",
		"wxNO_JPEG_LIB=1",
		"wxNO_PNG_LIB=1",
		"wxNO_TIFF_LIB=1",
		"wxNO_ZLIB_LIB=1",
	}
	
	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/p4api/include",
		"Dependencies/wxWidgets/include",
	}

    pchheader( "EditorPch.h" )
    pchsource( "Editor/EditorPch.cpp" )

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()
	Helium.DoFbxProjectSettings()

	links
	{
		prefix .. "Platform",
		prefix .. "Foundation",
		prefix .. "Application",
		prefix .. "Reflect",
		prefix .. "Persist",
		prefix .. "Math",
		prefix .. "MathSimd",
		prefix .. "Inspect",
		prefix .. "Engine",
		prefix .. "EngineJobs",
		prefix .. "Windowing",
		prefix .. "Rendering",
		prefix .. "GraphicsTypes",
		prefix .. "GraphicsJobs",
		prefix .. "Graphics",
		prefix .. "Framework",
		prefix .. "WindowingWin",
		prefix .. "RenderingD3D9",
		prefix .. "PcSupport",
		prefix .. "PreprocessingPc",
		prefix .. "EditorSupport",
		prefix .. "SceneGraph",
		prefix .. "Components",
		"zlib",
		"libclient",
		"libp4sslstub",
		"librpc",
		"libsupp",
	}

	-- We build monolithic wx, so ignore all the legacy non-monolithic #pragma comment directives (on windows only)

	configuration "windows"
		includedirs
		{
			"Dependencies/wxWidgets/include/msvc",
		}
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

	-- per architecture
	configuration { "windows", "x32" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_dll",
		}
	configuration { "windows", "x64" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_x64_dll",
		}
		
	-- per configuration
	configuration { "windows", "Debug" }
		links
		{
			"wxmsw29ud"
		}
	configuration { "windows", "not Debug" }
		links
		{
			"wxmsw29u"
		}
		
	-- per architecture, per configuration
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/Win32/Debug",
		}
	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/Win32/Release",
		}
	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/x64/Debug",
		}
	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/x64/Release",
		}

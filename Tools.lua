require "Helium"

solution "Tools"

Helium.DoDefaultSolutionSettings()

defines
{
	"HELIUM_TOOLS=1",
}

dofile "Shared.lua"

local prefix = solution().name .. '.'

project( prefix .. "Image" )
	uuid "50F5AA7E-22D9-4D33-B48A-357CD3082BC1"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Image", "IMAGE" )

	files
	{
		"Image/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Math",
			prefix .. "Foundation",
			"nvtt",
			"tiff",
			"d3d9",
			"d3dx9",
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
			prefix .. "Math",
			prefix .. "Foundation",
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
			"d3d9",
			"d3dx9",
		}

project( prefix .. "Editor" )
	uuid "A5CAC2F6-62BC-4EF3-A752-887F89C64812"
	kind "ConsoleApp"

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

    pchheader( "EditorPch.h" )
    pchsource( "Editor/EditorPch.cpp" )

	Helium.DoDefaultProjectSettings()

	links
	{
		prefix .. "Platform",
		prefix .. "Math",
		prefix .. "Foundation",
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
		prefix .. "Image",
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

require "Helium"

solution "Runtime"

Helium.DoDefaultSolutionSettings()

defines
{
	"HELIUM_EDITOR=0",
}

dofile "Shared.lua"

local prefix = solution().name .. '.'

project( prefix .. "TestJobs" )-- DEPRECATED
	uuid "12106586-0EB1-4D4C-9DFE-E3C63D3E4013"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "TestJobs", "TEST_JOBS" )
	
	files
	{
		"TestJobs/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Engine",
			prefix .. "EngineJobs",
		}

project( prefix .. "TestApp" )-- DEPRECATED
	uuid "CB5427DC-CE08-4FA6-B060-F35A902806BA"

	kind "WindowedApp"

	files
	{
		"TestApp/**.cpp",
		"TestApp/**.h",
		"TestApp/**.ico",
		"TestApp/**.rc"
	}

	flags
	{
		"WinMain",
	}

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
		prefix .. "FrameworkWin",
		prefix .. "TestJobs",
	}

	pchheader( "TestAppPch.h" )
	pchsource( "TestApp/TestAppPch.cpp" )

	Helium.DoDefaultProjectSettings()

	configuration "windows"
		links
		{
			"d3d9",
			"d3dx9",
			"d3d11",
			"dxguid",
			"d3dcompiler",
			"wininet",
			"ws2_32",
			"dbghelp",
		}
	configuration { "windows", "x32" }
		links
		{
			"fbxsdk_20113_1",
		}
	configuration { "windows", "x64" }
		links
		{
			"fbxsdk_20113_1_amd64",
		}
	if haveGranny then
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

project( prefix .. "ExampleGame" )
	uuid "ABB15BB2-467A-4D1A-A6DC-193DEF359AE4"

	Helium.DoModuleProjectSettings( "Example", "EXAMPLE", "ExampleGame", "EXAMPLE_GAME" )

	files
	{
		"Example/ExampleGame/*",
	}

	includedirs
	{
		"Example",
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
		}

project( prefix .. "ExampleMain" )
	uuid "2FF096F2-B7D3-4009-A409-3C2C6B57B56E"

	kind "WindowedApp"

	files
	{
		"Example/ExampleMain/*",
	}

	flags
	{
		"WinMain",
	}

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
		prefix .. "FrameworkWin",
		prefix .. "ExampleGame",
	}

	includedirs
	{
		"Example",
	}

	pchheader( "ExampleMainPch.h" )
	pchsource( "Example/ExampleMain/ExampleMainPch.cpp" )

	Helium.DoDefaultProjectSettings()

	configuration "windows"
		links
		{
			"d3d9",
			"d3dx9",
			"d3d11",
			"dxguid",
			"d3dcompiler",
			"wininet",
			"ws2_32",
			"dbghelp",
		}
	configuration { "windows", "x32" }
		links
		{
			"fbxsdk_20113_1",
		}
	configuration { "windows", "x64" }
		links
		{
			"fbxsdk_20113_1_amd64",
		}
	if haveGranny then
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

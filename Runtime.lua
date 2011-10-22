require "Helium"

solution "Runtime"

Helium.DoDefaultSolutionSettings()

print( "Temporarily defining HELIUM_TOOLS in Runtime" )
defines
{
	"HELIUM_TOOLS=1",
}

dofile "Shared.lua"

local prefix = solution().name .. '.'

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
		
	configuration { "windows", "Debug" }
		links
		{
			"fbxsdk-2012.2d",
		}
	configuration { "windows", "not Debug" }
		links
		{
			"fbxsdk-2012.2",
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

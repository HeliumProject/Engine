require "Dependencies/Helium"

-- Common settings for projects linking with libraries.
Helium.DoBasicProjectSettings = function()

	configuration {}

	language "C++"

	flags
	{
		"FloatFast",
		"FatalWarnings",
	}

	includedirs
	{
		".",
		"Dependencies",
	}

	if _ACTION == "vs2008" then	
		includedirs
		{
			"Platform/vs2008"
		}
		defines
		{
			"__STDC_LIMIT_MACROS=1"
		}
	end

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"dbghelp",
			"ws2_32",
			"wininet",
		}

	configuration {}

end

Helium.DoGraphicsProjectSettings = function()

	configuration {}

	configuration "windows"
		if _ACTION == "vs2012" or _ACTION == "vs2010" or _ACTION == "vs2008" then
			includedirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Include"
			}
		end
	configuration { "windows", "x32" }
		if _ACTION == "vs2012" or _ACTION == "vs2010" or _ACTION == "vs2008" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x86",
			}
		end
	configuration { "windows", "x64" }
		if _ACTION == "vs2012" or _ACTION == "vs2010" or _ACTION == "vs2008" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x64",
			}
		end

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"dinput8",
			"d3d9",
			"d3dx9",
			"d3d11",
			"d3dcompiler",
			"dxguid",
		}

	configuration {}

end

-- Common settings for modules.
Helium.DoModuleProjectSettings = function( baseDirectory, tokenPrefix, moduleName, moduleNameUpper )

	configuration {}

	defines
	{
		"HELIUM_MODULE=" .. moduleName
	}

	if os.get() == "windows" then

		local header = moduleName .. "Pch.h"
		if os.get() == "macosx" then
			header = path.join( moduleName, header )
			header = path.join( baseDirectory, header )
			header = path.join( "..", header )
			header = path.join( "..", header )
		end
		pchheader( header )

		local source = moduleName .. "Pch.cpp"
		source = path.join( moduleName, source )
		source = path.join( baseDirectory, source )
		pchsource( source )
		
	end

	Helium.DoBasicProjectSettings()

	if string.len(tokenPrefix) > 0 then
		tokenPrefix = tokenPrefix .. "_"
	end
		
	configuration { "windows", "Debug" }
		kind "SharedLib"
		defines
		{
			tokenPrefix .. moduleNameUpper .. "_EXPORTS",
		}

	configuration { "windows", "not Debug" }
		kind "StaticLib"

	configuration "not windows"
		kind "StaticLib"
		
	configuration {}

end

Helium.DoExampleMainProjectSettings = function(demoName)
	uuid "2FF096F2-CDD3-4009-A409-3C2C6B57B56E"

	kind "WindowedApp"

	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Example/ExampleMain_" .. demoName .. "/*",
	}

	flags
	{
		"WinMain",
	}

	defines
	{
		"HELIUM_MODULE=ExampleMain",
	}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Example",
	}

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
		prefix .. "FrameworkWin",
		prefix .. "TestJobs",
		prefix .. "Components",
		prefix .. "Bullet",
		prefix .. "Ois",
		prefix .. "ExampleGame",
	}

	pchheader( "ExampleMainPch.h" )
	pchsource( "Example/ExampleMain_" .. demoName .. "/ExampleMainPch.cpp" )

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()

	-- ExampleMain is a bit odd because it includes custom game objects and a main().
	-- So we need the dll export #defines. But calling DoModuleProjectSettings(...) above
	-- seems to blow away the libs we try to import when we call DoBasicProjectSettings()
	configuration { "windows", "Debug" }
		defines
		{
			"HELIUM_EXAMPLE_MAIN_EXPORTS",
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
end
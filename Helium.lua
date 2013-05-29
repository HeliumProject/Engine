require "Dependencies/Helium"

-- Common settings for projects linking with libraries.
Helium.DoBasicProjectSettings = function()

	configuration {}

	language "C++"

	flags
	{
		"FloatFast",
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

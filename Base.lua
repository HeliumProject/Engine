Helium = {}

Helium.Sleep = function( seconds )
	if os.get() == "windows" then
		os.execute("ping 127.0.0.1 -n " .. seconds + 1 .. " -w 1000 >:nul 2>&1")
	else
		os.execute("sleep " .. seconds)
	end
end

Helium.Publish = function( files )
	for i,v in pairs(files) do
		-- mkpath the target folder
		os.mkdir( v.target )
		
		local path = v.source .. "/" .. v.file			
		local exists = os.isfile( path )
		local destination = v.target .. "/" .. v.file
		
		-- cull existing files
		if os.isfile( destination ) then
			local delCommand = "del /q \"" .. string.gsub( destination, "/", "\\" ) .. "\""
			os.execute( delCommand )
		end

		-- do the file copy
		local linkCommand = "mklink /h \"" .. destination .. "\" \"" .. path .. "\""
		local result = os.execute( linkCommand )

		-- the files were copied, complete this entry
		if result == 0 then
			files[ i ] = nil
		else
			os.exit( 1 )
		end						
	end
end

-- Pre-build script execution.
Helium.Prebuild = function()

	local commands =
	{
		"python Build/JobDefParser.py JobDefinitions . .",
		"python Build/TypeParser.py . .",
	}

	local result = 0

	for i, commandString in ipairs( commands ) do
		result = os.execute( commandString )
		if result ~= 0 then
			break
		end
	end

	if result ~= 0 then
		error( "An error occurred processing the pre-build scripts." )
	end

end

Helium.DoDefaultSolutionSettings = function()

	location "Premake"

	platforms
	{
		"x32",
		"x64",
	}

	configurations
	{
		"Debug",
		"Intermediate",
		"Profile",
		"Release",
	}
	
	defines
	{
		"XML_STATIC=1",
		"KFBX_DLLINFO=1",
	}

	flags
	{
		"EnableSSE2",
		"NoMinimalRebuild",
	}

	configuration "x64"
		defines
		{
			-- Explicitly define "__SSE__" and "__SSE2__" on x86-64 platforms, as Visual C++ does not define them automatically.
			"__SSE__",
			"__SSE2__",
		}

	configuration "no-unicode"
		defines
		{
			"HELIUM_UNICODE=0",
		}
	
	configuration "not no-unicode"
		defines
		{
			"HELIUM_UNICODE=1",
			"UNICODE=1",
			"LITESQL_UNICODE=1",
			"XML_UNICODE_WCHAR_T=1",
		}
		flags
		{
			"Unicode",
		}
	
	for i, platform in ipairs( platforms() ) do
		for j, config in ipairs( configurations() ) do
			configuration( { config, platform } )
				objdir( "Intermediate" ) -- premake seems to automatically add the platform and config name
			configuration( { config, platform } )
				targetdir( "Bin/" .. platform .. "/" .. config )
		end
	end

	configuration "windows"
		defines
		{
			"_WIN32",
			"WIN32",
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NON_CONFORMING_SWPRINTFS",
		}

	configuration "Debug"
		defines
		{
			"HELIUM_DEBUG=1",
			"_DEBUG",
			"HELIUM_SHARED=1",
			"TBB_USE_DEBUG=1",
		}
		flags
		{
			"Symbols",
		}
		
	configuration "Intermediate"
		defines
		{
			"HELIUM_INTERMEDIATE=1",
			"HELIUM_STATIC=1",
		}
		flags
		{
			"OptimizeSpeed",
			"NoEditAndContinue",
			"Symbols",
		}
		
	configuration "Profile"
		defines
		{
			"HELIUM_PROFILE=1",
			"NDEBUG",
			"HELIUM_STATIC=1",
		}
		flags
		{
			"NoFramePointer",
			"OptimizeSpeed",
			"NoEditAndContinue",
		}

	configuration "Release"
		defines
		{
			"HELIUM_RELEASE=1",
			"NDEBUG",
			"HELIUM_STATIC=1",
		}
		flags
		{
			"NoFramePointer",
			"OptimizeSpeed",
			"NoEditAndContinue",
		}

	configuration { "windows", "Debug" }
		buildoptions
		{
			"/Ob0",
		}

	configuration { "windows", "not Debug" }
		buildoptions
		{
			"/Ob2",
			"/Oi",
		}

end

-- Common settings for projects linking with Lunar libraries.
Helium.DoDefaultLunarProjectSettings = function()

	language "C++"

	flags
	{
		"ExtraWarnings",
		"FatalWarnings",
		"FloatFast",  -- Should be used in all configurations to ensure data consistency.
		"NoRTTI",
	}

	links
	{
		"Expat",
		"nvtt",
		"png",
		"zlib",
	}

	configuration "no-unicode"
		defines
		{
			"L_UNICODE=0",
		}

	configuration "not no-unicode"
		defines
		{
			"L_UNICODE=1",
		}

	configuration "Debug"
		defines
		{
			"L_DEBUG=1",
			"L_EDITOR=1",
			"L_SHARED=1",
		}

	configuration "Intermediate"
		defines
		{
			"L_INTERMEDIATE=1",
			"L_EDITOR=1",
			"L_STATIC=1",
		}

	configuration "Profile"
		defines
		{
			"L_PROFILE=1",
			"L_STATIC=1",
		}

	configuration "Release"
		defines
		{
			"L_RELEASE=1",
			"L_STATIC=1",
		}

	configuration "windows"
		links
		{
			"d3d9",
			"d3dx9",
			"d3d11",
			"dxguid",
			"d3dcompiler",
			"wininet",
		}

	configuration { "windows", "Debug" }
		links
		{
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

end

-- Common settings for Lunar modules.
Helium.DoLunarModuleProjectSettings = function( tokenPrefix, moduleName, moduleNameUpper )

	defines
	{
		"HELIUM_MODULE_HEAP_FUNCTION=Get" .. moduleName .. "DefaultHeap"
	}

	files
	{
		moduleName .. "/*",
	}

	pchheader( moduleName .. "Pch.h" )
	pchsource( moduleName .. "/" .. moduleName .. "Pch.cpp" )

	Helium.DoDefaultLunarProjectSettings()

	configuration "Debug"
		kind "SharedLib"
		defines
		{
			tokenPrefix .. "_" .. moduleNameUpper .. "_EXPORTS",
		}

	configuration "not Debug"
		kind "StaticLib"

end

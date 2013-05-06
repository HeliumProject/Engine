Helium = {}

Helium.RequiredPremakeVersion = '4.4-beta1'
Helium.RequiredCLVersion = 150030729

os.capture = function( cmd, raw )
    local f = assert( io.popen( cmd, 'r' ) )
    local s = assert( f:read( '*a' ) )
    f:close()
    if raw then
        return s
    end
    s = string.gsub(s, '^%s+', '')
    s = string.gsub(s, '%s+$', '')
    s = string.gsub(s, '[\n\r]+', ' ')
    return s
end

Helium.GetSystemVersion = function()
    local version = 'Unknown'
	if os.get() == "windows" then
        version = os.capture( "cmd /c ver" )
	else
        version = os.capture( "uname -r" )
	end
	
	return version
end

Helium.Build32Bit = function()
	if ( _OPTIONS[ "universal" ] ) then
		return true
	else
    	return not os.is64bit()
    end
end

Helium.Build64Bit = function()
	if ( _OPTIONS[ "universal" ] ) then
		return true
	else
	    return os.is64bit()
	end
end

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

		-- do the hard link
		local linkCommand = ''
		if ( os.get() == "windows" ) then
			-- delete target
			if os.isfile( destination ) then
				local delCommand = "del /q \"" .. string.gsub( destination, "/", "\\" ) .. "\""

				-- if deleting the target failed, bail
				if os.execute( delCommand ) ~= 0 then
					print( "Deleting destination file: " .. destination .. " failed." )
					os.exit( 1 )
				end
			end

			-- check system version, do appropriate command line
            local versionString = Helium.GetSystemVersion()
            if ( string.find( versionString, "6\.%d+\.%d+" ) ) then -- vista/windows 7
                linkCommand = "mklink /H \"" .. destination .. "\" \"" .. path .. "\""
            else
                linkCommand = "fsutil hardlink create \"" .. destination .. "\" \"" .. path .. "\""
            end
   		else
   			-- hooray simplicity in *nix
            linkCommand = "ln -f \"" .. path .. "\" \"" .. destination .. "\""
		end

		-- if creating a hardlink failed, bail
		if os.execute( linkCommand ) ~= 0 then
			print( "Creating hardlink: FROM '" .. path .. "'   TO   '" .. destination .. "' failed." )
			os.exit( 1 )
		end

		-- the files were copied, complete this entry
		files[ i ] = nil
	end
end

newoption
{
   trigger = "universal",
   description = "Build for both 32-bit and 64-bit target machines"
}

newoption
{
	trigger = "wchar_t",
	description = "Build using wchar_t instead of UTF-8 strings"
}

Helium.DoBasicSolutionSettings = function()

	location "Premake"

    if _OPTIONS[ "universal" ] then
        platforms
        {
            "x32",
       		"x64",
        } 
    elseif Helium.Build64Bit() then
        platforms
        {
       		"x64",
       	}
	elseif Helium.Build32Bit() then
        platforms
        {
            "x32",
       	}
    end

	configurations
	{
		"Debug",
		"Intermediate",
		"Profile",
		"Release",
	}
	
	defines
	{
		"UNICODE=1",
		"FBXSDK_SHARED=1",
	}

	if _OPTIONS[ "wchar_t" ] then
		defines
		{
			"HELIUM_WCHAR_T=1",
		}
	end

	flags
	{
		"Unicode",
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

	for i, platform in ipairs( platforms() ) do
		for j, config in ipairs( configurations() ) do
			configuration( { config, platform } )
				objdir( "Build" ) -- premake seems to automatically add the platform and config name
			configuration( { config, platform } )
				targetdir( "Bin/" .. platform .. "/" .. config )
		end
	end

	configuration "windows"
		defines
		{
			"WIN32",
			"_WIN32",
			"_CRT_SECURE_NO_DEPRECATE",
			"_CRT_NON_CONFORMING_SWPRINTFS",
		}

	configuration "Debug"
		defines
		{
			"_DEBUG",
			"TBB_USE_DEBUG=1",
			"HELIUM_DEBUG=1",
			"HELIUM_SHARED=1",
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
			"Symbols",
			"OptimizeSpeed",
			"NoEditAndContinue",
		}
		
	configuration "Profile"
		defines
		{
			"NDEBUG",
			"HELIUM_PROFILE=1",
			"HELIUM_STATIC=1",
		}
		flags
		{
			"Symbols",
			"NoFramePointer",
			"OptimizeSpeed",
			"NoEditAndContinue",
		}

	configuration "Release"
		defines
		{
			"NDEBUG",
			"HELIUM_RELEASE=1",
			"HELIUM_STATIC=1",
		}
		flags
		{
			"Symbols",
			"NoFramePointer",
			"OptimizeSpeed",
			"NoEditAndContinue",
		}

	configuration "windows"
		buildoptions
		{
			"/MP",
			"/Zm256"
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

    -- vars to set in the project file for llvm + c++11 (for type traits)
	-- CLANG_CXX_LANGUAGE_STANDARD = "c++0x";
	-- CLANG_CXX_LIBRARY = "libc++";
	-- GCC_VERSION = com.apple.compilers.llvm.clang.1_0;

	configuration {}

end

-- Common settings for projects linking with libraries.
Helium.DoBasicProjectSettings = function()

	configuration {}

	language "C++"
	location( "Premake/" .. solution().name )
	targetname( "Helium." .. project().name )

	flags
	{
		"FloatFast",
	}

	includedirs
	{
		".",
		"Dependencies",
	}

	configuration "SharedLib or *App"
		links
		{
			"bullet",
			"freetype",
			"libpng",
			"nvtt",
			"ois",
			"zlib",
		}

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
		if _ACTION == "vs2010" or _ACTION == "vs2008" then
			includedirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Include"
			}
		end
	configuration { "windows", "x32" }
		if _ACTION == "vs2010" or _ACTION == "vs2008" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x86",
			}
		end
	configuration { "windows", "x64" }
		if _ACTION == "vs2010" or _ACTION == "vs2008" then
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

	configuration "not windows"
		kind "StaticLib"

	configuration { "windows", "Debug" }
		kind "SharedLib"
		defines
		{
			tokenPrefix .. "_" .. moduleNameUpper .. "_EXPORTS",
		}

	configuration { "windows", "not Debug" }
		kind "StaticLib"

	configuration {}

end

Helium = {}

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
        version = os.capture( "uname" )
	end
	
	return version
end

Helium.Build64Bit = function()
    if os.get() == "windows" then
        return string.find( os.getenv("PATH"), "x64" )
    else
		print("Implement support for " .. os.get() .. " to Helium.Build64Bit()")
		os.exit(1)
    end
end

Helium.Sleep = function( seconds )
	if os.get() == "windows" then
		os.execute("ping 127.0.0.1 -n " .. seconds + 1 .. " -w 1000 >:nul 2>&1")
	else
		os.execute("sleep " .. seconds)
	end
end

Helium.CheckEnvironment = function()

    print("\nChecking Environment...\n")

    if os.get() == "windows" then
    
        local failed = 0
        
        if os.getenv( "VCINSTALLDIR" ) == nil then
            print( " -> You must be running in a Visual Studio Command Prompt.")
            failed = 1
        end
        
        if not failed then
            if os.pathsearch( 'cl.exe', os.getenv( 'PATH' ) ) == nil then
                print( " -> cl.exe was not found in your path.  Make sure you are using a Visual Studio 2008 SP1 Command Prompt." )
                failed = 1
            else
                compilerPath = "cl.exe"
            end

            local compilerVersion = ''
            local compilerVersionOutput = os.capture( "\"cl.exe\" 2>&1" )
            for major, minor, build in string.gmatch( compilerVersionOutput, "Version (%d+)\.(%d+)\.(%d+)" ) do
                compilerVersion = major .. minor .. build
            end
            
            if tonumber( compilerVersion ) < Helium.RequiredCLVersion then
                print( " -> You must have Visual Studio 2008 with SP1 applied to compile Helium.  Please update your compiler and tools." )
                failed = 1
            end
        end
        
        if os.getenv( "DXSDK_DIR" ) == nil then
            print( " -> You must have the DirectX SDK installed (DXSDK_DIR is not defined in your environment)." )
            failed = 1
        end
        
        if failed == 1 then
            print( "\nCannot proceed until your environment is valid." )
            os.exit( 1 )
        end
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
			local delCommand = ''
			if ( os.get() == "windows" ) then
                delCommand = "del /q \"" .. string.gsub( destination, "/", "\\" ) .. "\""
            else
                delCommand = "rm \"" .. destination .. "\""
            end
			os.execute( delCommand )
		end

		-- do the file copy
		local linkCommand = ''
		if ( os.get() == "windows" ) then
            linkCommand = "fsutil hardlink create \"" .. destination .. "\" \"" .. path .. "\""
		else
            linkCommand = "ln -s \"" .. destination .. "\" \"" .. path .. "\""
		end
		local result = os.execute( linkCommand )
    
		-- the files were copied, complete this entry
		if result == 0 then
			files[ i ] = nil
		else
			os.exit( 1 )
		end						
	end
end

Helium.PublishIcons = function( bin )

    if os.get() == "windows" then
        os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Debug\\Icons\" *.png")
        os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Intermediate\\Icons\" *.png")
        os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Profile\\Icons\" *.png")
        os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Release\\Icons\" *.png")
        if Helium.Build64Bit() then
            os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Debug\\Icons\" *.png")
            os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Intermediate\\Icons\" *.png")
            os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Profile\\Icons\" *.png")
            os.execute("Utilities\\Win32\\robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Release\\Icons\" *.png")
        end
    else
		print("Implement support for " .. os.get() .. " to Helium.PublishIcons()")
		os.exit(1)
    end

end

-- Pre-build script execution.
Helium.Prebuild = function()

	local commands =
	{
		"python Build/JobDefParser.py JobDefinitions . .",
		"python Build/TypeParser.py . .",
	}
    
    local pythonPath = os.pathsearch( 'python', os.getenv( 'PATH' ) )
    if pythonPath == nil then
        pythonPath = os.pathsearch( 'python.exe', os.getenv( 'PATH' ) )
        if pythonPath == nil then
            error( "\n\nYou must have Python 3.x installed and in your PATH to continue." )
        end
    end
        
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

	if Helium.Build64Bit() then
        platforms
        {
            "x32",
       		"x64",
       	}
    else
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

	--configuration "SharedLib or *App"
	configuration "Debug"
		links
		{
			"Expat",
			"freetype",
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

	--configuration { "windows", "SharedLib or *App" }
	configuration { "windows", "Debug" }
		links
		{
			"d3d9",
			"d3dx9",
			"d3d11",
			"dxguid",
			"d3dcompiler",
			"wininet",
		}

	--configuration { "windows", "Debug", "SharedLib or *App" }
	configuration { "windows", "Debug" }
		links
		{
			"dbghelp",
		}

	--configuration { "windows", "x32", "SharedLib or *App" }
	configuration { "windows", "x32", "Debug" }
		links
		{
			"fbxsdk_20113_1",
		}

	--configuration { "windows", "x64", "SharedLib or *App" }
	configuration { "windows", "x64", "Debug" }
		links
		{
			"fbxsdk_20113_1_amd64",
		}

	if haveGranny then
		--configuration { "x32", "SharedLib or *App" }
		configuration { "x32", "Debug" }
			links
			{
				"granny2",
			}

		--configuration { "x64", "SharedLib or *App" }
		configuration { "x64", "Debug" }
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

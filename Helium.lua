Helium = {}

Helium.RequiredPremakeVersion = '4.4-beta1'
Helium.RequiredCLVersion = 150030729
Helium.RequiredFBXVersion = '2012.2'

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

Helium.Build64Bit = function()
    return os.is64bit()
end

Helium.GetFbxSdkLocation = function()
    local fbxLocation = os.getenv( 'FBX_SDK' )
    if not fbxLocation then
        if os.get() == "windows" then
            fbxLocation = "C:\\Program Files\\Autodesk\\FBX\\FbxSdk\\" .. Helium.RequiredFBXVersion
        elseif os.get() == "macosx" then
	       	fbxLocation = "/Applications/Autodesk/FBXSDK" .. Helium.RequiredFBXVersion
        else
            print("Implement support for " .. os.get() .. " to Helium.GetFbxSdkLocation()")
            os.exit(1)
        end
		if not os.isdir( fbxLocation ) then
			fbxLocation = nil
		end
    end
    
    return fbxLocation
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
    
    if _PREMAKE_VERSION < Helium.RequiredPremakeVersion then
		print( "You must be running at least Premake " .. Helium.RequiredPremakeVersion .. "." )
		os.exit( 1 )
	end

    if os.get() == "windows" then
    
        local failed = 0
        
        if os.pathsearch( 'Python.exe', os.getenv( 'PATH' ) ) == nil then
            print( " -> Python was not found in your path.  Python is required for the 'prebuild' phase." )
            print( " -> Make sure to download python (http://www.python.org/download/) and add it to your path." )
            print( " -> eg: Add c:\\Python\\Python31 to your path." )
            failed = 1
		end

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
        
        local fbxDir = Helium.GetFbxSdkLocation()
        if not fbxDir or not os.isdir( fbxDir ) then
            print( " -> You must have the FBX SDK installed and the FBX_SDK environment variable set." )
            print( " -> Make sure to point the FBX_SDK environment variable at the FBX install location, eg: C:\\Program Files\\Autodesk\\FBX\\FbxSdk\\" .. Helium.RequiredFBXVersion )
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

print( path )

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
			print( "Creating hardlink: " .. destination .. " failed." )
			os.exit( 1 )
		end

		-- the files were copied, complete this entry
		files[ i ] = nil
	end
end

Helium.PublishIcons = function( bin )

    if os.get() == "windows" then
        os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Debug\\Icons\" *.png")
        os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Intermediate\\Icons\" *.png")
        os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Profile\\Icons\" *.png")
        os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x32\\Release\\Icons\" *.png")
        if Helium.Build64Bit() then
            os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Debug\\Icons\" *.png")
            os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Intermediate\\Icons\" *.png")
            os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Profile\\Icons\" *.png")
            os.execute("robocopy /MIR \"Editor\\Icons\\Helium\" \"" .. bin .. "\\x64\\Release\\Icons\" *.png")
        end
    else
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Debug/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Intermediate/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Profile/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x32/Release/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        if Helium.Build64Bit() then
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Debug/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Intermediate/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Profile/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
            os.execute("rsync -a --delete Editor/Icons/Helium/ " .. bin .. "/x64/Release/Icons/ --filter='+ */' --filter '+ *.png' --filter='- *'")
        end
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
		"UNICODE=1",
		"FBXSDK_SHARED=1",
		"LITESQL_UNICODE=1",
		"XML_STATIC=1",
		"XML_UNICODE_WCHAR_T=1",
		"HELIUM_UNICODE=1",
	}

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

	configuration {}

end

-- Common settings for projects linking with libraries.
Helium.DoDefaultProjectSettings = function()

	language "C++"
	
	location( "Premake/" .. solution().name )

	targetname( "Helium." .. project().name )

	configuration {}

	flags
	{
		--"FatalWarnings",
		--"ExtraWarnings",
		"FloatFast",  -- Should be used in all configurations to ensure data consistency.
	}

	configuration "SharedLib or *App"
		links
		{
			"Expat",
			"freetype",
			"nvtt",
			"png",
			"zlib",
		}

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"ws2_32",
			"d3d9",
			"d3dx9",
			"d3d11",
			"dxguid",
			"d3dcompiler",
			"wininet",
		}

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"dbghelp",
		}

	configuration { "windows", "Debug", "SharedLib or *App" }
		links
		{
			"fbxsdk-2012.2d",
		}
	configuration { "windows", "not Debug", "SharedLib or *App" }
		links
		{
			"fbxsdk-2012.2",
		}

	if haveGranny then
		configuration { "x32", "SharedLib or *App" }
			links
			{
				"granny2",
			}

		configuration { "x64", "SharedLib or *App" }
			links
			{
				"granny2_x64",
			}
	end

	configuration {}
end

-- Common settings for modules.
Helium.DoModuleProjectSettings = function( baseDirectory, tokenPrefix, moduleName, moduleNameUpper )

	configuration {}

	defines
	{
		"HELIUM_MODULE_HEAP_FUNCTION=Get" .. moduleName .. "DefaultHeap"
	}

	pchheader( moduleName .. "Pch.h" )
	pchsource( baseDirectory .. "/" .. moduleName .. "/" .. moduleName .. "Pch.cpp" )

	Helium.DoDefaultProjectSettings()

--[[--This is off until we get rid of a couple dynamic_cast<>'s -Geoff
	configuration "not Debug"
		flags
		{
			"NoRTTI",
		}
--]]

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

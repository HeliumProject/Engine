Helium = {}

Helium.RequiredPremakeVersion = '4.4-beta1'
Helium.RequiredCLVersion = 150030729

Helium.CheckEnvironment = function ()

    print("\nChecking Environment...\n")
    
    if _PREMAKE_VERSION < Helium.RequiredPremakeVersion then
        print( "You must be running at least Premake " .. Helium.RequiredPremakeVersion .. "." )
        os.exit( 1 )
    end

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

    elseif os.get() == "macosx" then

        local major = 10
        local minor = 8
        local revision = 5
        local check = major * 10000 + minor * 100 + revision

        local ver = os.getversion()
        local number = ver.majorversion * 10000 + ver.minorversion * 100 + ver.revision
        if number < check then
            local str = string.format("%d.%d.%d", major, minor, revision)
            print( " -> Please update to OS X " .. str )
            failed = 1
        end

        if failed == 1 then
            print( "\nCannot proceed until your environment is valid." )
            os.exit( 1 )
        end

    end
end

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

Helium.GetProcessorCount = function()
	if os.get() == "windows" then
		return os.getenv("NUMBER_OF_PROCESSORS")
	elseif os.get() == "macosx" then
		return os.capture("/usr/sbin/system_profiler -detailLevel full SPHardwareDataType | awk '/Number .f Cores/ {print $5};'")
	elseif os.get() == "linux" then
		return os.capture( "nproc" )
	end
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

		local name = v.name
		if not name then
			name = v.file
		end

		local path = v.source .. "/" .. v.file
		local exists = os.isfile( path )
		local destination = v.target .. "/" .. name

		print( path .. "\n\t-> " .. destination )
		os.copyfile( path, destination ) -- linux returns non-zero if the target file is identical (!?)
		
		-- the files were copied, complete this entry
		files[ i ] = nil
	end
end

newoption
{
	trigger = "universal",
	description = "Build for both 32-bit and 64-bit target machines"
}

Helium.DoBasicSolutionSettings = function()

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

	flags
	{
		"Unicode",
		"NoMinimalRebuild",
	}

	if _ACTION ~= "vs2012" then
		flags
		{
			"EnableSSE2",
		}
	end

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
			configuration( { config, platform } )
				libdirs
				{
					 "Dependencies/Bin/" .. platform .. "/" .. config,
				}
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

	configuration { "macosx" }
		buildoptions
		{
			"-stdlib=libc++",
		}

	configuration {}

end

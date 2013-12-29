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

Helium.GetPremakeVersion = function()
	if _PREMAKE_VERSION == "HEAD" then
		return 5
	else
		return tonumber( string.sub( _PREMAKE_VERSION, 1, 1 ) )
	end
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

Helium.PublishSharedLibs = function( depsBin, heliumBin )
	-- Identify appropriate search pattern for shared libs by platform
	local libPattern = ""
	if os.get() == "windows" then
		libPattern = "dll"
	elseif os.get() == "macosx" then
		libPattern = "dylib"
	elseif os.get() == "linux" then
		libPattern = "so"
	end

	-- Collect a list of all shared libs under depsBin
	local libs = os.matchfiles( depsBin .. "/**." .. libPattern )
	
	-- Assuming identical directory structure under depsBin and heliumBin,
	-- create source and target paths for each shared lib.  Publish.
	local publishLibs = {}
	for i,v in pairs(libs) do
		local name = string.match( v, "^.*/(.-\." .. libPattern .. ")" )
		local sourceDir = string.gsub( v, "/" .. name, "" )
		local targetDir = heliumBin .. string.gsub( sourceDir, depsBin, "" )
		table.insert( publishLibs, { file=name, source=sourceDir, target=targetDir } )
	end
	Helium.Publish( publishLibs )
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
			"/Zm256",
			"/d2Zi+", -- http://randomascii.wordpress.com/2013/09/11/debugging-optimized-codenew-in-visual-studio-2012/
		}
		linkoptions
		{
			"/ignore:4221", -- disable warning about linking .obj files with not symbols defined (conditionally compiled away)
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

	configuration { "linux" }
		linkoptions
		{
			"-lboost_regex"
		}

	configuration {}

end

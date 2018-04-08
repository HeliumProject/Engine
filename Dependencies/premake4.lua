local thisFileLocation = ...
if thisFileLocation == nil then
	thisFileLocation = '.'
end
thisFileLocation = path.getdirectory( thisFileLocation )

require( thisFileLocation .. '/Helium' )
require( thisFileLocation .. '/fbx' )
require( thisFileLocation .. '/wxWidgets' )

function CheckEnvironment()

	print("\nChecking Environment...\n")
	
	if _PREMAKE_VERSION < Helium.RequiredPremakeVersion then
		print( "You must be running at least Premake " .. Helium.RequiredPremakeVersion .. "." )
		os.exit( 1 )
	end

	if os.host() == "windows" then
	
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
			local compilerVersionOutput = Helium.ExecuteAndCapture( "\"cl.exe\" 2>&1" )
			for major, minor, build in string.gmatch( compilerVersionOutput, "Version (%d+).(%d+).(%d+)" ) do
				compilerVersion = major .. minor .. build
			end
			
			if tonumber( compilerVersion ) < Helium.RequiredClVersion then
				print( " -> You must have Visual Studio 2008 with SP1 applied to compile Helium.  Please update your compiler and tools." )
				failed = 1
			end
		end

		if os.getenv( "DXSDK_DIR" ) == nil then
			print( " -> You must have the DirectX SDK installed (DXSDK_DIR is not defined in your environment)." )
			failed = 1
		end

		local fbxDir = Helium.GetFbxSdkLocation()
		if not os.isdir( fbxDir ) then
			print( " -> You must have the FBX SDK installed and the FBX_SDK environment variable set." )
			print( " -> Make sure to point the FBX_SDK environment variable at the FBX install location, eg: C:\\Program Files\\Autodesk\\FBX\\FbxSdk\\" .. Helium.RequiredFbxVersion )
			failed = 1
		end

		if failed == 1 then
			print( "\nCannot proceed until your environment is valid." )
			os.exit( 1 )
		end

	elseif os.host() == "macosx" then

		local checkMajor = 10
		local checkMinor = 8
		local checkNumber = checkMajor * 10000 + checkMinor

		local osMajor = 0
		local osMinor = 0
		local swVersOutput = Helium.ExecuteAndCapture( "sw_vers" )
		for major, minor in string.gmatch( swVersOutput, "ProductVersion:	(%d+).(%d+)" ) do
			osMajor = tonumber(major)
			osMinor = tonumber(minor)
		end

		local osNumber = osMajor * 10000 + osMinor * 100
		if osNumber < checkNumber then
			local str = string.format("%d.%d", checkMajor, checkMinor)
			print( " -> Please update to OS X " .. str )
			failed = 1
		end

		if failed == 1 then
			print( "\nCannot proceed until your environment is valid." )
			os.exit( 1 )
		end

	end
end

newoption {
	trigger	= "core",
	description	= "Core components only",
}

newoption {
	trigger	= "wx-config",
	value	= "configuration",
	description	= "Choose which configs of wxWidgets to build",
	allowed	= {
		{ "debug", "Debug" },
		{ "release", "Release" }
	}
}

wx_debug = true
wx_release = true
if _OPTIONS[ "wx-config" ] == "debug" then
	wx_release = false
end
if _OPTIONS[ "wx-config" ] == "release" then
	wx_debug = false
end

-- Do nothing if there is no action (--help, etc...)
if _ACTION then

	-- Check prereqs
	CheckEnvironment()

	if _ACTION == "xcode3" then
		print("XCode 3 is not supported")
		os.exit(1)
	end

	if _ACTION == "vs2002" then
		print("Visual Studio 2002 is not supported")
		os.exit(1)
	end

	if _ACTION == "vs2003" then
		print("Visual Studio 2003 is not supported")
		os.exit(1)
	end

	if _ACTION == "vs2005" then
		print("Visual Studio 2005 is not supported")
		os.exit(1)
	end

	if _ACTION == "vs2010" then
		print("Visual Studio 2010 is not supported")
		os.exit(1)
	end

	if _ACTION == "vs2012" then
		print("Visual Studio 2012 is not supported")
		os.exit(1)
	end

	if _ACTION == "vs2013" then
		print("Visual Studio 2013 is not supported")
		os.exit(1)
	end

	if not _OPTIONS[ "core" ] then
		if _ACTION ~= "clean" then
			local bin = "../Bin/"
			Helium.BuildWxWidgets( wx_debug, wx_release )
			Helium.PublishWxWidgets( bin, wx_debug, wx_release )
			Helium.PublishFbx( bin )
		else
			Helium.CleanWxWidgets()
		end
	end

	workspace "Dependencies"
	Helium.DoBasicWorkspaceSettings()

	configuration "Debug"
		targetdir( "../Bin/Debug/" .. Helium.GetBundleExecutablePath() )

	configuration "Intermediate"
		targetdir( "../Bin/Intermediate/" .. Helium.GetBundleExecutablePath() )

	configuration "Profile"
		targetdir( "../Bin/Profile/" .. Helium.GetBundleExecutablePath() )

	configuration "Release"
		targetdir( "../Bin/Release/" .. Helium.GetBundleExecutablePath() )

	dofile "Dependencies.lua"

end
--]]
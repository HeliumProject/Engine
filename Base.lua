Helium = {}

Helium.RequiredCLVersion = 150030729
Helium.RequiredFBXVersion = '2011.3.1'

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
    elseif os.get() == "macosx" then
    	return true;
    else
		print("Implement support for " .. os.get() .. " to Helium.Build64Bit()")
		os.exit(1)
    end
end

Helium.GetFbxSdkLocation = function()
    local fbxLocation = os.getenv( 'FBX_SDK' )
    if not fbxLocation then
        if os.get() == "windows" then
            fbxLocation = "C:\\Program Files\\Autodesk\\FBX\\FbxSdk\\" .. Helium.RequiredFBXVersion
        elseif os.get() == "macosx" then
        	if Helium.RequiredFBXVersion == '2011.3.1' then
 		       	fbxLocation = "/Applications/Autodesk/FBXSDK20113_1"
 		   	end
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
        
        if os.getenv( 'CL' ) == nil or not string.match( os.getenv( 'CL' ), '\/MP' ) then
            print( "\n\n  SUGGESTION: You should consider setting the 'CL' environment variable to contain '/MP' to take advantage of multiple cores/hyperthreading to help improve Helium build times.\n\n\n" )
        end
        
        if os.getenv( "DXSDK_DIR" ) == nil then
            print( " -> You must have the DirectX SDK installed (DXSDK_DIR is not defined in your environment)." )
            failed = 1
        end
        
        local fbxDir = Helium.GetFbxSdkLocation()
        if not fbxDir or not os.isdir( fbxDir ) then
            print( " -> You must have the FBX SDK installed and the FBX_SDK environment variable set." )
            print( " -> Make sure to point the FBX_SDK environment variable at the FBX install location, eg: C:\\Program Files\\Autodesk\\FBX\\FbxSdk\\2011.3.1" )
            failed = 1
        end
        
        if fbxDir and not string.match( fbxDir, '2011\.3\.1$' ) then
            print( " -> Currently, Helium only supports version 2011.3.1 of the FBX SDK.  Please download that specific version and make sure your FBX_SDK environment variable points to the proper install location." )
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
            local versionString = Helium.GetSystemVersion()
            if ( string.find( versionString, "6\.%d+\.%d+" ) ) then -- vista/windows 7
                linkCommand = "mklink /H \"" .. destination .. "\" \"" .. path .. "\""
            else
                linkCommand = "fsutil hardlink create \"" .. destination .. "\" \"" .. path .. "\""
            end
   		else
            linkCommand = "ln \"" .. path .. "\" \"" .. destination .. "\""
		end
		local result = os.execute( linkCommand )

		-- If creating a hardlink failed, attempt a normal copy
		if result ~= 0 then
			local copyCommand = ''
			if ( os.get() == "windows" ) then
				copyCommand = "xcopy \"" .. path .. "\" \"" .. v.target .. "\" /d /f /r /y"
			else
				copyCommand = "cp " .. path .. " " .. destination
			end
			os.execute( copyCommand )
			if result ~= 0 then
				os.exit( 1 )
			end
		end

		-- the files were copied, complete this entry
		files[ i ] = nil
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

-- Pre-build script execution.
Helium.Prebuild = function()

	local commands =
	{
		"python Build/JobDefParser.py JobDefinitions . .",
		"python Build/TypeParser.py D3D9Rendering EditorSupport Engine EngineJobs Framework FrameworkWin Graphics GraphicsJobs GraphicsTypes PcSupport PreprocessingPc Rendering TestJobs WinWindowing Windowing",
		"python Build/TypeParser.py -i Example -s Example -p EXAMPLE_ ExampleGame ExampleMain",
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
			"L_DEBUG=1",
			"L_EDITOR=1",
			"L_SHARED=1",
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
			"L_INTERMEDIATE=1",
			"L_EDITOR=1",
			"L_STATIC=1",
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
			"L_PROFILE=1",
			"L_STATIC=1",
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
			"L_RELEASE=1",
			"L_STATIC=1",
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
Helium.DoLunarModuleProjectSettings = function( baseDirectory, tokenPrefix, moduleName, moduleNameUpper )

	defines
	{
		"HELIUM_MODULE_HEAP_FUNCTION=Get" .. moduleName .. "DefaultHeap"
	}

	files
	{
		baseDirectory .. "/" .. moduleName .. "/*",
	}

	pchheader( moduleName .. "Pch.h" )
	pchsource( baseDirectory .. "/" .. moduleName .. "/" .. moduleName .. "Pch.cpp" )

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

require "Dependencies/Helium"

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

-- Common settings for projects linking with libraries.
Helium.DoBasicProjectSettings = function()

	configuration {}

	language "C++"

	flags
	{
		"FloatFast",
		"FatalWarnings",
	}

	if _OPTIONS[ "direct3d" ] then
		defines
		{
			"HELIUM_DIRECT3D=1",
		}
	end

	if _OPTIONS[ "opengl" ] then
		defines
		{
			"HELIUM_OPENGL=1",
		}
	end

	if string.find( project().name, "Helium%-Tools%-" ) ~= nil then
		defines
		{
			"HELIUM_TOOLS=1",
			"HELIUM_RTTI=1",
		}
	else
		flags
		{
			"NoRTTI",
		}
		defines
		{
			"HELIUM_RTTI=0",
			"TBB_USE_EXCEPTIONS=0",
		}
	end

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

	configuration { "macosx" }
		buildoptions
		{
			"-std=c++11",
		}

	configuration { "macosx", "SharedLib or *App" }
		linkoptions
		{
			"-stdlib=libc++",
			"-framework CoreFoundation",
			"-framework CoreGraphics",
			"-framework Carbon",
			"-framework IOKit",
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
		kind "SharedLib"
		
	configuration {}

end

Helium.DoExampleMainProjectSettings = function(demoName)

	kind "WindowedApp"

	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Example/ExampleMain_" .. demoName .. "/*.cpp",
		"Example/ExampleMain_" .. demoName .. "/*.h",
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
		prefix .. "Engine",
		prefix .. "EngineJobs",
		prefix .. "Windowing",
		prefix .. "Rendering",
		prefix .. "GraphicsTypes",
		prefix .. "GraphicsJobs",
		prefix .. "Graphics",
		prefix .. "Framework",
		prefix .. "FrameworkImpl",
		prefix .. "Components",
		prefix .. "Bullet",
		prefix .. "Ois",
		prefix .. "ExampleGame",

		-- core
		prefix .. "Platform",
		prefix .. "Foundation",
		prefix .. "Reflect",
		prefix .. "Persist",
		prefix .. "Math",
		prefix .. "MathSimd",

		"bullet",
		"mongo-c",
		"ois",
	}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, "Helium%-Tools%-" ) then
		links
		{
			"Helium-Tools-PcSupport",
			"Helium-Tools-PreprocessingPc",
			"Helium-Tools-EditorSupport",
		}
	end

if os.get() == "windows" then
	pchheader( "ExampleMainPch.h" )
	pchsource( "Example/ExampleMain_" .. demoName .. "/ExampleMainPch.cpp" )
end

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()

	configuration "windows"
		files
		{
			"Example/ExampleMain_" .. demoName .. "/*.rc",
		}

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
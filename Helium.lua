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
			local compilerVersionOutput = Helium.ExecuteAndCapture( "\"cl.exe\" 2>&1" )
			for major, minor, build in string.gmatch( compilerVersionOutput, "Version (%d+).(%d+).(%d+)" ) do
				compilerVersion = major .. minor .. build
			end

			if tonumber( compilerVersion ) < Helium.RequiredClVersion then
				print( " -> You must have Visual Studio 2008 with SP1 applied to compile Helium.  Please update your compiler and tools." )
				failed = 1
			end
		end

		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			if os.getenv( "DXSDK_DIR" ) == nil then
				print( " -> You must have the DirectX SDK installed (DXSDK_DIR is not defined in your environment)." )
				failed = 1
			end
		end

		local fbxDir = Helium.GetFbxSdkLocation()
		if not fbxDir then
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

	defines
	{
		"HELIUM_HEAP=1",
	}

	if _OPTIONS[ "gfxapi" ] == "direct3d" then
		defines
		{
			"HELIUM_DIRECT3D=1",
		}
	elseif _OPTIONS[ "gfxapi" ] == "opengl" then
		defines
		{
			"HELIUM_OPENGL=1",
		}
		includedirs
		{
			"Dependencies/glew/include",
			"Dependencies/glfw/include",
		}
	end

	if tools then
		defines
		{
			"HELIUM_SHARED=1",
			"HELIUM_TOOLS=1",
			"HELIUM_RTTI=1",
		}
	else
		rtti "Off"
		defines
		{
			"HELIUM_SHARED=0",
			"HELIUM_RTTI=0",
		}
	end

	includedirs
	{
		".",
		"Dependencies",
	}

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"dbghelp",
			"ws2_32",
			"wininet",
		}

	configuration "linux"
		links
		{
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

	configuration "macosx or linux"
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

	configuration "linux"
		buildoptions
		{
			"-pthread",
		}

	configuration {}

end

Helium.DoTestsProjectSettings = function()

	kind "ConsoleApp"

	Helium.DoBasicProjectSettings()

	defines
	{
		"HELIUM_MODULE=Tests"
	}

	includedirs
	{
		".",
		"Dependencies/googletest/googletest/include"
	}

	links
	{
		"googletest"
	}

	postbuildcommands
	{
		"\"%{cfg.linktarget.abspath}\""
	}

end

Helium.DoGraphicsProjectSettings = function()

	configuration {}

	configuration "windows"
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			includedirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Include"
			}
		end

	configuration { "windows", "x32" }
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x86",
			}
		end

	configuration { "windows", "x64" }
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "Lib/x64",
			}
		end

	configuration { "windows", "SharedLib or *App" }
		links
		{
			"dxguid",
			"dinput8",
		}

	configuration { "SharedLib or *App" }
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			links
			{
				"d3d9",
				"d3dx9",
				"d3d11",
				"d3dcompiler",
			}
		elseif _OPTIONS[ "gfxapi" ] == "opengl" then
			links
			{
				"glew",
				"glfw",
			}
			configuration { "linux" }
				links
				{
					"GL",
				}
			configuration { "macosx" }
				linkoptions
				{
					"-framework OpenGL",
				}
			configuration { "windows" }
				links
				{
					"opengl32",
				}
		end	

	configuration {}

end

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

	if tools then
		kind "SharedLib"
	else
		kind "StaticLib"
	end

	if string.len(tokenPrefix) > 0 then
		tokenPrefix = tokenPrefix .. "_"
	end

	if os.get() == "windows" then
		configuration "SharedLib"
			defines
			{
				tokenPrefix .. moduleNameUpper .. "_EXPORTS",
			}
	end

	if os.get() == "macosx" then
		configuration "SharedLib"
			linkoptions
			{
				"-Wl,-install_name,@executable_path/lib" .. project().name .. ".dylib",
			}
	end

	configuration {}

end

Helium.DoGameProjectSettings = function( name )

	configuration {}

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	defines
	{
		"HELIUM_MODULE=Game",
	}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Projects",
	}

	links
	{
		prefix .. "GameLibrary",
		prefix .. "Ois",
		prefix .. "Bullet",
		prefix .. "Components",
		prefix .. "FrameworkImpl",
	}

	if _OPTIONS[ "gfxapi" ] == "direct3d" then
		links
		{
			prefix .. "RenderingD3D9",
		}
	elseif _OPTIONS[ "gfxapi" ] == "opengl" then
		links
		{
			prefix .. "RenderingGL",
		}
	end

	if tools then
		links
		{
			"Helium-Tools-PreprocessingPc",
			"Helium-Tools-PcSupport",
			"Helium-Tools-EditorSupport",
		}
	end

	links
	{
		prefix .. "Framework",
		prefix .. "Graphics",
		prefix .. "GraphicsJobs",
		prefix .. "GraphicsTypes",
		prefix .. "Rendering",
		prefix .. "Windowing",
		prefix .. "EngineJobs",
		prefix .. "Engine",

		-- core
		prefix .. "MathSimd",
		prefix .. "Math",
		prefix .. "Persist",
		prefix .. "Reflect",
		prefix .. "Foundation",
		prefix .. "Platform",

		-- dependencies
		"bullet",
		"mongo-c",
		"ois",
	}

	if _OPTIONS[ "gfxapi" ] == "opengl" then
		links
		{
			"glew",
			"glfw",
		}
	end

	configuration "linux"
		links
		{
			"GL",
			"X11",
			"Xrandr",
			"Xi",
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

	configuration {}

end

Helium.DoGameModuleProjectSettings = function( name )

	configuration {}

	project( name .. "Module" )

	objdir( "Projects/" .. name .. "/Build" )

	configuration "Debug"
		targetdir( "Projects/" .. name .. "/Bin/Debug/" )
		libdirs { "Bin/Debug/" .. Helium.GetBundleExecutablePath() }

	configuration "Intermediate"
		targetdir( "Projects/" .. name .. "/Bin/Intermediate/" )
		libdirs { "Bin/Debug/" .. Helium.GetBundleExecutablePath() }

	configuration "Profile"
		targetdir( "Projects/" .. name .. "/Bin/Profile/" )
		libdirs { "Bin/Debug/" .. Helium.GetBundleExecutablePath() }

	configuration "Release"
		targetdir( "Projects/" .. name .. "/Bin/Release/" )
		libdirs { "Bin/Debug/" .. Helium.GetBundleExecutablePath() }

	configuration {}

	kind "SharedLib"

	Helium.DoGameProjectSettings()

	defines
	{
		"GAME_MODULE_EXPORTS",
	}

	includedirs
	{
		"Projects",
	}

	configuration "windows"
		pchheader( "GamePch.h" )
		pchsource( "Projects/" .. name .. "/Source/Module/GamePch.cpp" )
	configuration {}

	files
	{
		"Projects/" .. name .. "/Source/Module/**",
	}

	configuration {}

end

Helium.DoGameMainProjectSettings = function( name )

	configuration {}

	if tools then
		project( name .. "Tool" )
	else
		project( name )

		objdir( "Projects/" .. name .. "/Build" )

		configuration "Debug"
			targetdir( "Projects/" .. name .. "/Bin/Debug/" )

		configuration "Intermediate"
			targetdir( "Projects/" .. name .. "/Bin/Intermediate/" )

		configuration "Profile"
			targetdir( "Projects/" .. name .. "/Bin/Profile/" )

		configuration "Release"
			targetdir( "Projects/" .. name .. "/Bin/Release/" )

		configuration {}
	end

	kind "WindowedApp"

	Helium.DoGameProjectSettings()

	flags
	{
		"WinMain",
	}

	files
	{
		"Projects/" .. name .. "/Source/Module/*.cpp",
		"Projects/" .. name .. "/Source/Module/*.h",
		"Projects/" .. name .. "/Source/Main/*.cpp",
		"Projects/" .. name .. "/Source/Main/*.h",
	}

	configuration "windows"
		files
		{
			"Projects/" .. name .. "/Source/Main/*.rc",
		}
		pchheader( "GamePch.h" )
		pchsource( "Projects/" .. name .. "/Source/Module/GamePch.cpp" )

	configuration "not windows"
		includedirs { "Projects/" .. name .. "/Source/Module" }

	configuration {}

end

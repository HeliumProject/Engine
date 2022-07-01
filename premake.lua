require "Dependencies/premake"

Helium.CheckEnvironment = function ()

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

	elseif os.host() == "macosx" then

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

	filter {}

	language "C++"

	floatingpoint "Fast"

	flags
	{
		"FatalWarnings",
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
		"Core/Source",
		"Core/Dependencies/rapidjson/include",
		"Core/Dependencies/mongo-c/src",
		"Source/Engine",
		"Source/Tools",
		"Dependencies/ois/includes",
	}

	filter { "system:windows", "kind:SharedLib or *App" }
		links
		{
			"dbghelp",
			"ws2_32",
			"wininet",
		}

	filter "system:macosx or linux"
		buildoptions
		{
			"-std=c++11",
		}

	filter { "system:macosx", "kind:SharedLib or *App" }
		linkoptions
		{
			"-stdlib=libc++",
			"-framework CoreFoundation",
			"-framework CoreGraphics",
			"-framework Carbon",
			"-framework IOKit",
		}

	filter "system:linux"
		buildoptions
		{
			"-pthread",
		}

	filter {}

end

Helium.DoTestsProjectSettings = function()

	filter {}

	kind "ConsoleApp"

	Helium.DoBasicProjectSettings()

	includedirs
	{
		".",
		"Core/Dependencies/googletest/googletest/include"
	}

	links
	{
		"googletest"
	}

	postbuildcommands
	{
		"\"%{cfg.linktarget.abspath}\""
	}

	filter "system:linux"
		links
		{
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

	filter {}

end

Helium.DoGraphicsProjectSettings = function()

	filter {}

	filter "system:windows"
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			includedirs
			{
				"Dependencies/D3DX/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/include"
			}
		end

	filter { "system:windows", "architecture:x86" }
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			libdirs
			{
				"Dependencies/D3DX/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/lib/x86"
			}
		end

	filter { "system:windows", "architecture:x86_64" }
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			libdirs
			{
				"Dependencies/D3DX/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/lib/x64"
			}
		end

	filter { "system:windows", "kind:SharedLib or *App" }
		links
		{
			"dxguid",
			"dinput8",
		}

	filter { "kind:SharedLib or *App" }
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
			filter { "system:linux" }
				links
				{
					"GL",
				}
			filter { "system:macosx" }
				linkoptions
				{
					"-framework OpenGL",
				}
			filter { "system:windows" }
				links
				{
					"opengl32",
				}
		end

	filter {}

end

Helium.DoModuleProjectSettings = function( baseDirectory, tokenPrefix, moduleName, moduleNameUpper )

	filter {}

	defines
	{
		"HELIUM_HEAP=1",
		"HELIUM_MODULE=" .. moduleName
	}

	if _OPTIONS["pch"] then
		pchheader( "Precompile.h" )

		local source = "Precompile.cpp"
		source = path.join( moduleName, source )
		source = path.join( baseDirectory, source )
		pchsource( source )

		local include = ""
		source = path.join( moduleName, include )
		source = path.join( baseDirectory, include )
		includedirs
		{
			include,
		}
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

	if os.host() == "windows" then
		filter "kind:SharedLib"
			defines
			{
				tokenPrefix .. moduleNameUpper .. "_EXPORTS",
			}
	end

	if os.host() == "macosx" then
		filter "kind:SharedLib"
			linkoptions
			{
				"-Wl,-install_name,@executable_path/lib" .. project().name .. ".dylib",
			}
	end

	filter {}

end

Helium.DoGameProjectSettings = function( name )

	filter {}

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	defines
	{
		"HELIUM_HEAP=1",
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
		prefix .. "MathSimd",

		-- core
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

	filter "system:linux"
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

	filter {}

end

Helium.DoGameModuleProjectSettings = function( name )

	filter {}

	project( name .. "Module" )

	objdir( "Projects/" .. name .. "/Build" )

	filter "configurations:Debug"
		targetdir( "Projects/" .. name .. "/Bin/Debug/" )
		libdirs { "Bin/Debug/" .. Helium.GetBundleExecutablePath() }

	filter "configurations:Intermediate"
		targetdir( "Projects/" .. name .. "/Bin/Intermediate/" )
		libdirs { "Bin/Intermediate/" .. Helium.GetBundleExecutablePath() }

	filter "configurations:Profile"
		targetdir( "Projects/" .. name .. "/Bin/Profile/" )
		libdirs { "Bin/Profile/" .. Helium.GetBundleExecutablePath() }

	filter "configurations:Release"
		targetdir( "Projects/" .. name .. "/Bin/Release/" )
		libdirs { "Bin/Release/" .. Helium.GetBundleExecutablePath() }

	filter {}

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

	if _OPTIONS["pch"] then
		pchheader( "Precompile.h" )
		pchsource( "Projects/" .. name .. "/Source/Module/Precompile.cpp" )
		includedirs
		{
			"Projects/" .. name .. "/Source/Module",
		}
	end

	files
	{
		"Projects/" .. name .. "/Source/Module/**",
	}

	filter {}

end

Helium.DoGameMainProjectSettings = function( name )

	filter {}

	if tools then
		project( name .. "Tool" )
	else
		project( name )

		objdir( "Projects/" .. name .. "/Build" )

		filter "configurations:Debug"
			targetdir( "Projects/" .. name .. "/Bin/Debug/" )

		filter "configurations:Intermediate"
			targetdir( "Projects/" .. name .. "/Bin/Intermediate/" )

		filter "configurations:Profile"
			targetdir( "Projects/" .. name .. "/Bin/Profile/" )

		filter "configurations:Release"
			targetdir( "Projects/" .. name .. "/Bin/Release/" )

		filter {}
	end

	kind "WindowedApp"

	Helium.DoGameProjectSettings()

	files
	{
		"Projects/" .. name .. "/Source/Module/*.cpp",
		"Projects/" .. name .. "/Source/Module/*.h",
		"Projects/" .. name .. "/Source/Main/*.cpp",
		"Projects/" .. name .. "/Source/Main/*.h",
	}

	includedirs
	{
		"Projects/" .. name .. "/Source/Module"
	}

	if _OPTIONS["pch"] then
		pchheader( "Precompile.h" )
		pchsource( "Projects/" .. name .. "/Source/Module/Precompile.cpp" )
	end

	filter "system:windows"
		entrypoint "WinMainCRTStartup"
		files
		{
			"Projects/" .. name .. "/Source/Main/*.rc",
		}

end

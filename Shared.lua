require "Dependencies/Helium"
require "Dependencies/tbb"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "Engine" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Engine", "ENGINE" )
	Helium.DoTbbProjectSettings()

	files
	{
		"Engine/*",
	}

	configuration "SharedLib"
		links
		{
			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "EngineJobs" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EngineJobs", "ENGINE_JOBS" )
	Helium.DoTbbProjectSettings()

	files
	{
		"EngineJobs/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "Windowing" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Windowing", "WINDOWING" )

	files
	{
		"Windowing/*",
	}

	configuration "windows"
		excludes
		{
			"Windowing/*Mac.*",
			"Windowing/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"Windowing/*Win.*",
			"Windowing/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Windowing/*Win.*",
			"Windowing/*Mac.*",
		}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "Rendering" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Rendering", "RENDERING" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Rendering/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

if _OPTIONS[ "direct3d" ] then

project( prefix .. "RenderingD3D9" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "RenderingD3D9", "RENDERING_D3D9" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"RenderingD3D9/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

end

project( prefix .. "GraphicsTypes" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "GraphicsTypes", "GRAPHICS_TYPES" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"GraphicsTypes/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "GraphicsJobs" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "GraphicsJobs", "GRAPHICS_JOBS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"GraphicsJobs/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "Graphics" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Graphics", "GRAPHICS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Graphics/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "Framework",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "Components" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Components", "COMPONENTS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Components/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Ois",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "Bullet" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Bullet", "BULLET" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Bullet/*",
	}

	includedirs
	{
		"Dependencies/bullet/src",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Components",
			prefix .. "Rendering", -- (for debug drawing)
			prefix .. "GraphicsTypes", -- (for debug drawing)
			prefix .. "Graphics", -- (for debug drawing)

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "MathSimd",

			"bullet",
		}
		
				
project( prefix .. "Ois" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Ois", "OIS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Ois/*",
	}

	includedirs
	{
		"Dependencies/ois/includes",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "MathSimd",

			"ois",
		}

project( prefix .. "Framework" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Framework", "FRAMEWORK" )
	Helium.DoTbbProjectSettings()

	files
	{
		"Framework/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "FrameworkImpl" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "FrameworkImpl", "FRAMEWORK_IMPL" )

	files
	{
		"FrameworkImpl/*",
	}

	includedirs
	{
		"Dependencies/freetype/include",
	}

	configuration "windows"
		excludes
		{
			"FrameworkImpl/*Mac.*",
			"FrameworkImpl/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"FrameworkImpl/*Win.*",
			"FrameworkImpl/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"FrameworkImpl/*Win.*",
			"FrameworkImpl/*Mac.*",
		}

	configuration {}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, "Helium%-Tools%-" ) then
		links
		{
			"Helium-Tools-PreprocessingPc",
			"Helium-Tools-PcSupport",
			"Helium-Tools-EditorSupport",
		}
	end

	configuration "SharedLib"
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

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "TestJobs" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "TestJobs", "TEST_JOBS" )
	Helium.DoTbbProjectSettings()
	
	files
	{
		"TestJobs/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "TestApp" )

	kind "WindowedApp"

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"TestApp/**.cpp",
		"TestApp/**.h",
		"TestApp/**.ico",
	}

	flags
	{
		"WinMain",
	}

	defines
	{
		"HELIUM_MODULE=TestApp",
	}

	-- TestApp is a bit odd because it includes custom game objects and a main().
	-- So we need the dll export #defines. But calling DoModuleProjectSettings(...) above
	-- seems to blow away the libs we try to import when we call DoBasicProjectSettings()
	configuration { "windows", "Debug" }
		defines
		{
			"HELIUM_TEST_APP_EXPORTS",
		}

	configuration {}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
	}

	configuration "windows"
		files
		{
			"TestApp/**.rc"
		}
		pchheader( "TestAppPch.h" )
		pchsource( "TestApp/TestAppPch.cpp" )

	configuration {}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	links
	{
		prefix .. "Ois",
		prefix .. "Bullet",
		prefix .. "Components",
		prefix .. "TestJobs",
		prefix .. "FrameworkImpl",
	}

	if string.find( project().name, "Helium%-Tools%-" ) then
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

		"bullet",
		"mongo-c",
		"ois",
	}

	configuration { "linux", "SharedLib or *App" }
		links
		{
			"GL",
			"X11",
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

project( prefix .. "ExampleGame" )

	Helium.DoModuleProjectSettings( "Example", "", "ExampleGame", "EXAMPLE_GAME" )
	Helium.DoFbxProjectSettings()

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Example",
	}

	files
	{
		"Example/ExampleGame/**",
	}

	configuration "windows"
		pchheader( "ExampleGamePch.h" )
		pchsource( "Example/ExampleGame/ExampleGamePch.cpp" )
	
	configuration "not windows"
		includedirs
		{
			"Example/ExampleGame",
		}

	configuration {}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, "Helium%-Tools%-" ) then
		links
		{
			"Helium-Tools-PreprocessingPc",
			"Helium-Tools-PcSupport",
			"Helium-Tools-EditorSupport",
		}
	end

	configuration "SharedLib"
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

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",

			"ois",
			"mongo-c",
		}

project( prefix .. "ExampleMain_PhysicsDemo" )

	Helium.DoExampleMainProjectSettings( "PhysicsDemo" )
		
project( prefix .. "ExampleMain_ShapeShooter" )

	Helium.DoExampleMainProjectSettings( "ShapeShooter" )

project( prefix .. "EmptyGame" )

	Helium.DoModuleProjectSettings( "Empty", "", "EmptyGame", "EMPTY_GAME" )
	Helium.DoFbxProjectSettings()

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Example",
	}

	files
	{
		"Example/EmptyGame/**",
	}

	configuration "windows"
		pchheader( "EmptyGamePch.h" )
		pchsource( "Example/EmptyGame/EmptyGamePch.cpp" )

	configuration {}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, "Helium%-Tools%-" ) then
		links
		{
			"Helium-Tools-PreprocessingPc",
			"Helium-Tools-PcSupport",
			"Helium-Tools-EditorSupport",
		}
	end

	configuration "SharedLib"
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

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",

			"ois",
			"mongo-c",
		}

project( prefix .. "EmptyMain" )

	kind "WindowedApp"

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"Example/EmptyMain/*.cpp",
		"Example/EmptyMain/*.h",
	}

	flags
	{
		"WinMain",
	}

	defines
	{
		"HELIUM_MODULE=EmptyMain",
	}

	-- EmptyMain is a bit odd because it includes custom game objects and a main().
	-- So we need the dll export #defines. But calling DoModuleProjectSettings(...) above
	-- seems to blow away the libs we try to import when we call DoBasicProjectSettings()
	configuration { "windows", "Debug" }
		defines
		{
			"HELIUM_EMPTY_MAIN_EXPORTS",
		}

	configuration {}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Example",
	}

	configuration "windows"
		files
		{
			"Example/EmptyMain/*.rc",
		}
		pchheader( "EmptyMainPch.h" )
		pchsource( "Example/EmptyMain/EmptyMainPch.cpp" )
	
	configuration {}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	links
	{
		prefix .. "EmptyGame",
		prefix .. "Ois",
		prefix .. "Bullet",
		prefix .. "Components",
		prefix .. "FrameworkImpl",
	}

	if string.find( project().name, "Helium%-Tools%-" ) then
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

		"bullet",
		"mongo-c",
		"ois",
	}

	configuration { "linux", "SharedLib or *App" }
		links
		{
			"GL",
			"X11",
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

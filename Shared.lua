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
			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
		}

end

project( prefix .. "GraphicsTypes" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "GraphicsTypes", "GRAPHICS_TYPES" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"GraphicsTypes/*",
	}

	if haveGranny then
		includedirs
		{
			"Integrations/Granny",
			"Integrations/Granny/granny_sdk/include",
		}
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

project( prefix .. "GraphicsJobs" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "GraphicsJobs", "GRAPHICS_JOBS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"GraphicsJobs/*",
	}

	if haveGranny then
		includedirs
		{
			"Integrations/Granny",
			"Integrations/Granny/granny_sdk/include",
		}
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

project( prefix .. "Graphics" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Graphics", "GRAPHICS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Graphics/*",
	}

	if haveGranny then
		includedirs
		{
			"Integrations/Granny",
			"Integrations/Granny/granny_sdk/include",
		}
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "Framework",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

project( prefix .. "Components" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Components", "COMPONENTS" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Components/*",
	}

	if haveGranny then
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Math",
			core .. "MathSimd",
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

	if haveGranny then
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Math",
			core .. "MathSimd",

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

	if haveGranny then
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Math",
			core .. "MathSimd",

			"ois",
		}

project( prefix .. "Framework" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Framework", "FRAMEWORK" )
	Helium.DoTbbProjectSettings()

	files
	{
		"Framework/*",
	}

	if haveGranny then
		includedirs
		{
			"Integrations/Granny",
			"Integrations/Granny/granny_sdk/include",
		}
		defines
		{
			"HELIUM_HAVE_GRANNY=1",
		}
	else
		defines
		{
			"HELIUM_HAVE_GRANNY=0",
		}
	end

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
		}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, 'Tools.' ) then
		links
		{
			"Tools.PcSupport",
			"Tools.PreprocessingPc",
			"Tools.EditorSupport",
		}
	end

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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",
		}

project( prefix .. "TestApp" )

	kind "WindowedApp"

	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

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

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
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
		prefix .. "TestJobs",
		prefix .. "Components",
		prefix .. "Bullet",
		prefix .. "Ois",

		core .. "Platform",
		core .. "Foundation",
		core .. "Reflect",
		core .. "Persist",
		core .. "Math",
		core .. "MathSimd",

		"ois",
		"mongo-c",
	}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, 'Tools.' ) then
		links
		{
			"Tools.PcSupport",
			"Tools.PreprocessingPc",
			"Tools.EditorSupport",
		}
	end

if os.get() == "windows" then
	pchheader( "TestAppPch.h" )
	pchsource( "TestApp/TestAppPch.cpp" )
end

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()

	configuration "windows"
		files
		{
			"TestApp/**.rc"
		}

	-- TestApp is a bit odd because it includes custom game objects and a main().
	-- So we need the dll export #defines. But calling DoModuleProjectSettings(...) above
	-- seems to blow away the libs we try to import when we call DoBasicProjectSettings()
	configuration { "windows", "Debug" }
		defines
		{
			"HELIUM_TEST_APP_EXPORTS",
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

project( prefix .. "ExampleGame" )

	Helium.DoModuleProjectSettings( "Example", "", "ExampleGame", "EXAMPLE_GAME" )

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()
	
	files
	{
		"Example/ExampleGame/**",
	}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Example",
	}

if os.get() == "windows" then
	pchheader( "ExampleGamePch.h" )
	pchsource( "Example/ExampleGame/ExampleGamePch.cpp" )
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",

			"ois",
			"mongo-c",
		}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, 'Tools.' ) then
		links
		{
			"Tools.PcSupport",
			"Tools.PreprocessingPc",
			"Tools.EditorSupport",
		}
	end

project( prefix .. "ExampleMain_PhysicsDemo" )

	Helium.DoExampleMainProjectSettings( "PhysicsDemo" )
		
project( prefix .. "ExampleMain_ShapeShooter" )

	Helium.DoExampleMainProjectSettings( "ShapeShooter" )

project( prefix .. "EmptyGame" )

	Helium.DoModuleProjectSettings( "Empty", "", "EmptyGame", "EMPTY_GAME" )

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()
	
	files
	{
		"Example/EmptyGame/**",
	}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Example",
	}

if os.get() == "windows" then
	pchheader( "EmptyGamePch.h" )
	pchsource( "Example/EmptyGame/EmptyGamePch.cpp" )
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

			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
			core .. "MathSimd",

			"ois",
			"mongo-c",
		}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, 'Tools.' ) then
		links
		{
			"Tools.PcSupport",
			"Tools.PreprocessingPc",
			"Tools.EditorSupport",
		}
	end

project( prefix .. "EmptyMain" )

	kind "WindowedApp"

	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"Example/EmptyMain/*",
	}

	flags
	{
		"WinMain",
	}

	defines
	{
		"HELIUM_MODULE=EmptyMain",
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
		prefix .. "EmptyGame",

		core .. "Platform",
		core .. "Foundation",
		core .. "Reflect",
		core .. "Persist",
		core .. "Math",
		core .. "MathSimd",

		"ois",
		"mongo-c",
	}

	if _OPTIONS[ "direct3d" ] then
		links
		{
			prefix .. "RenderingD3D9",
		}
	end

	if string.find( project().name, 'Tools.' ) then
		links
		{
			"Tools.PcSupport",
			"Tools.PreprocessingPc",
			"Tools.EditorSupport",
		}
	end

if os.get() == "windows" then
	pchheader( "EmptyMainPch.h" )
	pchsource( "Example/EmptyMain/EmptyMainPch.cpp" )
end

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()

	-- EmptyMain is a bit odd because it includes custom game objects and a main().
	-- So we need the dll export #defines. But calling DoModuleProjectSettings(...) above
	-- seems to blow away the libs we try to import when we call DoBasicProjectSettings()
	configuration { "windows", "Debug" }
		defines
		{
			"HELIUM_EMPTY_MAIN_EXPORTS",
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

require "Dependencies/Helium"
require "Dependencies/tbb"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "Platform" )
	uuid "E4A1F8FC-A93A-46E2-9CA8-40C2CE1B163E"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Platform", "PLATFORM" )

	files
	{
		"Platform/*.cpp",
		"Platform/*.h",
		"Platform/*.inl",
	}

	configuration "windows"
		excludes
		{
			"Platform/*Posix.*",
			"Platform/*Mac.*",
			"Platform/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"Platform/*Win.*",
			"Platform/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Platform/*Win.*",
			"Platform/*Mac.*",
		}

project( prefix .. "Foundation" )
	uuid "9708463D-9698-4BB6-A911-37354AF0E21E"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Foundation", "FOUNDATION" )

	files
	{
		"Foundation/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
		}

project( prefix .. "Reflect" )
	uuid "6488751F-220A-4E88-BA5B-A1BE5E3124EC"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Reflect/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

project( prefix .. "Persist" )
	uuid "B853C313-79CF-4eec-A295-B0076A070FD6"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Persist", "PERSIST" )

	files
	{
		"Persist/**",
	}

	includedirs
	{
		"Dependencies/rapidjson/include",
		"Dependencies/zlib",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			"zlib",
		}

project( prefix .. "Math" )
	uuid "8F42DBD6-75E3-4A16-A3B6-77381600009D"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Math", "MATH" )

	files
	{
		"Math/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
		}

project( prefix .. "MathSimd" )
	uuid "B9ED7386-82FE-4a13-8EC6-59931FADB7C5"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "MathSimd", "MATH_SIMD" )

	files
	{
		"MathSimd/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
		}

project( prefix .. "Engine" )
	uuid "CDD089F1-EC6E-469B-BF06-8DF56C5B1489"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Engine", "ENGINE" )
	Helium.DoTbbProjectSettings()

	files
	{
		"Engine/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "EngineJobs" )
	uuid "65CFFE89-3111-4D58-95DC-5DB6D3F28935"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EngineJobs", "ENGINE_JOBS" )
	Helium.DoTbbProjectSettings()

	files
	{
		"EngineJobs/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
		}

project( prefix .. "Windowing" )
	uuid "B68268DF-3942-432F-89B1-DBC82C21218E"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Windowing", "WINDOWING" )

	files
	{
		"Windowing/*",
	}

	configuration "windows"
		excludes
		{
			"Platform/*Mac.*",
			"Platform/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"Platform/*Win.*",
			"Platform/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Platform/*Win.*",
			"Platform/*Mac.*",
		}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
		}

project( prefix .. "Rendering" )
	uuid "3F1BD209-272C-4833-AF8E-35C317F21452"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Rendering", "RENDERING" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Rendering/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
		}

project( prefix .. "GraphicsTypes" )
	uuid "4A13A4F6-6860-4F52-A217-B0C3943E7025"

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
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
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
	uuid "4D83346D-DCB2-40E6-AAF1-508341728E57"

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
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
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
	uuid "3342921C-F6C7-4A81-A6FF-1C93373AF285"

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
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "Framework",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
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
	uuid "DE0F5117-C0F1-4AEA-95FD-00E206243296"

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
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Ois"
		}
	
		
project( prefix .. "Bullet" )
	uuid "DE0F5117-C0F1-4AEA-95FD-00E206243297"

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
			-- dependencies
			"bullet",

			-- projects
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Components",
			prefix .. "Rendering", -- (for debug drawing)
			prefix .. "GraphicsTypes", -- (for debug drawing)
			prefix .. "Graphics", -- (for debug drawing)
			
		}
		
				
project( prefix .. "Ois" )
	uuid "DE0F5117-C0F1-4AEA-95FD-00E205243297"

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
			-- dependencies
			"ois",

			-- projects
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
		}

project( prefix .. "Framework" )
	uuid "6DB6B383-76E6-4361-8CFE-F08F1CFE24BE"

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
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs"--,
			--prefix .. "Windowing"--,
			--prefix .. "Rendering",
			--prefix .. "GraphicsTypes",
			--prefix .. "GraphicsJobs",
			--prefix .. "Graphics",
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

project( prefix .. "RenderingD3D9" )
	uuid "4BE28ED4-950D-469B-A6F8-88C09BA479E5"

	Helium.DoModuleProjectSettings( ".", "HELIUM", "RenderingD3D9", "RENDERING_D3D9" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"RenderingD3D9/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
		}

project( prefix .. "TestApp" )
	uuid "CB5427DC-CE08-4FA6-B060-F35A902806BA"

	kind "WindowedApp"

	Helium.DoGraphicsProjectSettings()
	Helium.DoTbbProjectSettings()

	files
	{
		"TestApp/**.cpp",
		"TestApp/**.h",
		"TestApp/**.ico",
		"TestApp/**.rc"
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
		prefix .. "Platform",
		prefix .. "Foundation",
		prefix .. "Reflect",
		prefix .. "Persist",
		prefix .. "Math",
		prefix .. "MathSimd",
		prefix .. "Engine",
		prefix .. "EngineJobs",
		prefix .. "Windowing",
		prefix .. "Rendering",
		prefix .. "GraphicsTypes",
		prefix .. "GraphicsJobs",
		prefix .. "Graphics",
		prefix .. "Framework",
		prefix .. "RenderingD3D9",
		prefix .. "PcSupport",
		prefix .. "PreprocessingPc",
		prefix .. "EditorSupport",
		prefix .. "FrameworkWin",
		prefix .. "TestJobs",
		prefix .. "Components",
		prefix .. "Bullet",
		prefix .. "Ois",
	}

	pchheader( "TestAppPch.h" )
	pchsource( "TestApp/TestAppPch.cpp" )

	Helium.DoBasicProjectSettings()
	Helium.DoFbxProjectSettings()

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
	uuid "ABB15BB2-467A-4D1A-A6DC-193DEF359AE4"

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
	
	pchheader( "ExampleGamePch.h" )
	pchsource( "Example/ExampleGame/ExampleGamePch.cpp" )

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "RenderingD3D9",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
			prefix .. "EditorSupport",
			prefix .. "FrameworkWin",
			prefix .. "TestJobs",
			prefix .. "Components",
			prefix .. "Bullet",
			prefix .. "Ois",
		}

project( prefix .. "ExampleMain_PhysicsDemo" )
	uuid "2FF096F2-CDD3-4009-A409-3C2C6B57B5DA"
	Helium.DoExampleMainProjectSettings( "PhysicsDemo" )
		
project( prefix .. "ExampleMain_ShapeShooter" )
	uuid "2FF096F2-CDD3-4009-A409-3C2C6B57B5DB"
	Helium.DoExampleMainProjectSettings( "ShapeShooter" )

project( prefix .. "EmptyGame" )
	uuid "ABB15BB2-46CD-4D1A-A6DC-193DEF359AE4"

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
	
	pchheader( "EmptyGamePch.h" )
	pchsource( "Example/EmptyGame/EmptyGamePch.cpp" )

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "RenderingD3D9",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
			prefix .. "EditorSupport",
			prefix .. "FrameworkWin",
			prefix .. "TestJobs",
			prefix .. "Components",
			prefix .. "Bullet",
			prefix .. "Ois",
		}
		

project( prefix .. "EmptyMain" )
	uuid "2FF096F2-B7D3-4009-A409-3C2C6B57B56E"

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
		prefix .. "Platform",
		prefix .. "Foundation",
		prefix .. "Reflect",
		prefix .. "Persist",
		prefix .. "Math",
		prefix .. "MathSimd",
		prefix .. "Engine",
		prefix .. "EngineJobs",
		prefix .. "Windowing",
		prefix .. "Rendering",
		prefix .. "GraphicsTypes",
		prefix .. "GraphicsJobs",
		prefix .. "Graphics",
		prefix .. "Framework",
		prefix .. "RenderingD3D9",
		prefix .. "PcSupport",
		prefix .. "PreprocessingPc",
		prefix .. "EditorSupport",
		prefix .. "FrameworkWin",
		prefix .. "TestJobs",
		prefix .. "Components",
		prefix .. "Bullet",
		prefix .. "Ois",
		prefix .. "EmptyGame",
	}

	pchheader( "EmptyMainPch.h" )
	pchsource( "Example/EmptyMain/EmptyMainPch.cpp" )

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


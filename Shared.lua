require "Dependencies/Helium"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "Engine" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Engine", "ENGINE" )

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
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Windowing/*",
	}

	configuration "windows"
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			excludes
			{
				"Windowing/*GLFW.*",
			}
		else
			excludes
			{
				"Windowing/*Win.*",
			}
		end

	configuration "macosx"
		excludes
		{
			"Windowing/*Win.*",
		}

	configuration "linux"
		excludes
		{
			"Windowing/*Win.*",
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

if _OPTIONS[ "gfxapi" ] == "direct3d" then

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

elseif _OPTIONS[ "gfxapi" ] == "opengl" then

project( prefix .. "RenderingGL" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "RenderingGL", "RENDERING_GL" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"RenderingGL/*",
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
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			excludes
			{
				"FrameworkImpl/*GLFW.*",
			}
		else
			excludes
			{
				"FrameworkImpl/WindowManagerInitializationImplWin.*",
			}
		end


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

	configuration "SharedLib"

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

project( prefix .. "GameLibrary" )

	Helium.DoModuleProjectSettings( "Game", "", "GameLibrary", "GAME_LIBRARY" )
	Helium.DoFbxProjectSettings()

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Projects",
	}

	files
	{
		"Projects/GameLibrary/**",
	}

	configuration "windows"
		pchheader( "GameLibraryPch.h" )
		pchsource( "Projects/GameLibrary/GameLibraryPch.cpp" )
	
	configuration "not windows"
		includedirs
		{
			"Projects/GameLibrary",
		}

	configuration {}

	configuration "SharedLib"

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

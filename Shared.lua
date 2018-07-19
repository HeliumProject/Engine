require "Dependencies/Helium"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "MathSimd" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "MathSimd", "MATH_SIMD" )

	files
	{
		"Source/Engine/MathSimd/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
		}

	configuration {}

project( prefix .. "Engine" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Engine", "ENGINE" )

	files
	{
		"Source/Engine/Engine/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "EngineJobs", "ENGINE_JOBS" )

	files
	{
		"Source/Engine/EngineJobs/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Windowing", "WINDOWING" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Windowing/*",
	}

	configuration "windows"
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			excludes
			{
				"Source/Engine/Windowing/*GLFW.*",
			}
		else
			excludes
			{
				"Source/Engine/Windowing/*Win.*",
			}
		end

	configuration "macosx"
		excludes
		{
			"Source/Engine/Windowing/*Win.*",
		}

	configuration "linux"
		excludes
		{
			"Source/Engine/Windowing/*Win.*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Rendering", "RENDERING" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Rendering/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "RenderingD3D9", "RENDERING_D3D9" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/RenderingD3D9/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "RenderingGL", "RENDERING_GL" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/RenderingGL/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "GraphicsTypes", "GRAPHICS_TYPES" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/GraphicsTypes/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "GraphicsJobs", "GRAPHICS_JOBS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/GraphicsJobs/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Graphics", "GRAPHICS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Graphics/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Components", "COMPONENTS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Components/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Bullet", "BULLET" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Bullet/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Ois", "OIS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Ois/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Framework", "FRAMEWORK" )

	files
	{
		"Source/Engine/Framework/*",
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

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "FrameworkImpl", "FRAMEWORK_IMPL" )

	files
	{
		"Source/Engine/FrameworkImpl/*",
	}

	includedirs
	{
		"Dependencies/freetype/include",
	}

	configuration "windows"
		excludes
		{
			"Source/Engine/FrameworkImpl/*Mac.*",
			"Source/Engine/FrameworkImpl/*Lin.*",
		}
		if _OPTIONS[ "gfxapi" ] == "direct3d" then
			excludes
			{
				"Source/Engine/FrameworkImpl/*GLFW.*",
			}
		else
			excludes
			{
				"Source/Engine/FrameworkImpl/WindowManagerInitializationImplWin.*",
			}
		end


	configuration "macosx"
		excludes
		{
			"Source/Engine/FrameworkImpl/*Win.*",
			"Source/Engine/FrameworkImpl/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Source/Engine/FrameworkImpl/*Win.*",
			"Source/Engine/FrameworkImpl/*Mac.*",
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

	if _OPTIONS["pch"] then
		pchheader( "Precompile.h" )
		pchsource( "Projects/GameLibrary/Precompile.cpp" )
		includedirs
		{
			"Projects/GameLibrary",
		}
	end
	
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

Helium.DoGameMainProjectSettings( "PhysicsDemo" )
Helium.DoGameMainProjectSettings( "ShapeShooter" )
Helium.DoGameMainProjectSettings( "SideScroller" )
Helium.DoGameMainProjectSettings( "TestBull" )

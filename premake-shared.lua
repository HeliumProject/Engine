require "Dependencies/premake"
require "Dependencies/premake-fbx"

require "premake"

project( prefix .. "MathSimd" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "MathSimd", "MATH_SIMD" )

	files
	{
		"Source/Engine/MathSimd/**",
	}

	filter "kind:SharedLib"
		links
		{
			-- core
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "Engine" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Engine", "ENGINE" )

	files
	{
		"Source/Engine/Engine/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "EngineJobs" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "EngineJobs", "ENGINE_JOBS" )

	files
	{
		"Source/Engine/EngineJobs/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}
	
	filter {}

project( prefix .. "Windowing" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Windowing", "WINDOWING" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Windowing/*",
	}

	filter "system:windows"
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

	filter "system:macosx"
		excludes
		{
			"Source/Engine/Windowing/*Win.*",
		}

	filter "system:linux"
		excludes
		{
			"Source/Engine/Windowing/*Win.*",
		}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "Rendering" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Rendering", "RENDERING" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Rendering/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}
	
	filter {}

if _OPTIONS[ "gfxapi" ] == "direct3d" then

project( prefix .. "RenderingD3D9" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "RenderingD3D9", "RENDERING_D3D9" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/RenderingD3D9/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

elseif _OPTIONS[ "gfxapi" ] == "opengl" then

project( prefix .. "RenderingGL" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "RenderingGL", "RENDERING_GL" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/RenderingGL/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

end

project( prefix .. "GraphicsTypes" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "GraphicsTypes", "GRAPHICS_TYPES" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/GraphicsTypes/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "GraphicsJobs" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "GraphicsJobs", "GRAPHICS_JOBS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/GraphicsJobs/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "Graphics" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Graphics", "GRAPHICS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Graphics/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "Framework",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "Components" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Components", "COMPONENTS" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"Source/Engine/Components/*",
	}

	filter "kind:SharedLib"
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
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

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

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Components",
			prefix .. "Rendering", -- (for debug drawing)
			prefix .. "GraphicsTypes", -- (for debug drawing)
			prefix .. "Graphics", -- (for debug drawing)
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",

			"bullet",
		}

	filter {}
		
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

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Framework",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",

			"ois",
		}

	filter {}

project( prefix .. "Framework" )

	Helium.DoModuleProjectSettings( "Source/Engine", "HELIUM", "Framework", "FRAMEWORK" )

	files
	{
		"Source/Engine/Framework/*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

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

	filter "system:windows"
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

	filter "system:macosx"
		excludes
		{
			"Source/Engine/FrameworkImpl/*Win.*",
			"Source/Engine/FrameworkImpl/*Lin.*",
		}

	filter "system:linux"
		excludes
		{
			"Source/Engine/FrameworkImpl/*Win.*",
			"Source/Engine/FrameworkImpl/*Mac.*",
		}

	filter "kind:SharedLib"
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
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "GameLibrary" )

	Helium.DoModuleProjectSettings( "Game", "", "GameLibrary", "GAME_LIBRARY" )
	Helium.DoFbxProjectSettings()

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/bullet/src",
		"Projects",
		"Projects/GameLibrary",
	}

	files
	{
		"Projects/GameLibrary/**",
	}

	if _OPTIONS["pch"] then
		pchheader( "Precompile.h" )
		pchsource( "Projects/GameLibrary/Precompile.cpp" )
	end
	
	filter "kind:SharedLib"
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
			prefix .. "MathSimd",

			-- core
			prefix .. "Math",
			prefix .. "Persist",
			prefix .. "Reflect",
			prefix .. "Foundation",
			prefix .. "Platform",

			"ois",
			"mongo-c",
		}

	filter {}

Helium.DoGameMainProjectSettings( "PhysicsDemo" )
Helium.DoGameMainProjectSettings( "ShapeShooter" )
Helium.DoGameMainProjectSettings( "SideScroller" )
Helium.DoGameMainProjectSettings( "TestBull" )

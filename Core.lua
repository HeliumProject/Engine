require "Dependencies/Helium"
require "Dependencies/tbb"
require "Dependencies/fbx"

require "Helium"

project( "Core.Platform" )

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

project( "Core.Foundation" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Foundation", "FOUNDATION" )

	files
	{
		"Foundation/**",
	}

	configuration "SharedLib"
		links
		{
			"Core.Platform",
		}

project( "Core.Application" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Application/**",
	}

	configuration "SharedLib"
		links
		{
			"Core.Platform",
			"Core.Foundation",
		}

project( "Core.Reflect" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Reflect/**",
	}

	configuration "SharedLib"
		links
		{
			"Core.Platform",
			"Core.Foundation",
		}

project( "Core.Persist" )

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
			"Core.Platform",
			"Core.Foundation",
			"Core.Reflect",
			"zlib",
		}

project( "Core.Inspect" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Inspect", "INSPECT" )

	files
	{
		"Inspect/**",
	}

	includedirs
	{
		"Inspect",
	}

	configuration "SharedLib"
		links
		{
			"Core.Platform",
			"Core.Foundation",
			"Core.Application",
			"Core.Reflect",
			"Core.Persist",
			"Core.Math",
		}

project( "Core.Math" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Math", "MATH" )

	files
	{
		"Math/**",
	}

	configuration "SharedLib"
		links
		{
			"Core.Platform",
			"Core.Foundation",
			"Core.Reflect",
			"Core.Persist",
		}

project( "Core.MathSimd" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "MathSimd", "MATH_SIMD" )

	files
	{
		"MathSimd/**",
	}

	configuration "SharedLib"
		links
		{
			"Core.Platform",
			"Core.Foundation",
			"Core.Reflect",
			"Core.Persist",
		}

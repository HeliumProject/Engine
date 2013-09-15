require "Dependencies/Helium"
require "Dependencies/tbb"
require "Dependencies/fbx"

require "Helium"

core = "Helium-Core-"
group "Core"

project( core .. "Platform" )

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

project( core .. "Foundation" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Foundation", "FOUNDATION" )

	files
	{
		"Foundation/**",
	}

	configuration "SharedLib"
		links
		{
			core .. "Platform",
		}

project( core .. "Application" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Application/**",
	}

	configuration "SharedLib"
		links
		{
			core .. "Platform",
			core .. "Foundation",
		}

project( core .. "Reflect" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Reflect/**",
	}

	configuration "SharedLib"
		links
		{
			core .. "Platform",
			core .. "Foundation",
		}

project( core .. "Persist" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Persist", "PERSIST" )

	files
	{
		"Persist/**",
	}

	configuration "SharedLib"
		links
		{
			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			"mongo-c",
		}

project( core .. "Inspect" )

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
			core .. "Platform",
			core .. "Foundation",
			core .. "Application",
			core .. "Reflect",
			core .. "Persist",
			core .. "Math",
		}

project( core .. "Math" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Math", "MATH" )

	files
	{
		"Math/**",
	}

	configuration "SharedLib"
		links
		{
			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
		}

project( core .. "MathSimd" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "MathSimd", "MATH_SIMD" )

	files
	{
		"MathSimd/**",
	}

	configuration "SharedLib"
		links
		{
			core .. "Platform",
			core .. "Foundation",
			core .. "Reflect",
			core .. "Persist",
		}

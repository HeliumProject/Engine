require "Dependencies/Helium"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "Platform" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Platform", "PLATFORM" )

	files
	{
		"Platform/*.cpp",
		"Platform/*.h",
		"Platform/*.inl",
	}

	excludes
	{
		"Platform/*Tests.*",
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

project( prefix .. "PlatformTests" )

	kind "ConsoleApp"

	Helium.DoBasicProjectSettings()

	files
	{
		"Platform/*Tests.*",
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

	configuration "linux"
		links
		{
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

project( prefix .. "Foundation" )

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

project( prefix .. "Application" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Application/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

project( prefix .. "Reflect" )

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

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Persist", "PERSIST" )

	files
	{
		"Persist/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			"mongo-c",
		}

project( prefix .. "Mongo" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "Mongo", "MONGO" )

	files
	{
		"Mongo/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			"mongo-c",
		}

project( prefix .. "Inspect" )

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
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Application",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "Math" )

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

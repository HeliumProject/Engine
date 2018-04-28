require "Dependencies/Helium"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "Platform" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Platform", "PLATFORM" )

	files
	{
		"Source/Core/Platform/*.cpp",
		"Source/Core/Platform/*.h",
		"Source/Core/Platform/*.inl",
	}

	excludes
	{
		"Source/Core/Platform/*Tests.*",
	}

	configuration "windows"
		excludes
		{
			"Source/Core/Platform/*Posix.*",
			"Source/Core/Platform/*Mac.*",
			"Source/Core/Platform/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"Source/Core/Platform/*Win.*",
			"Source/Core/Platform/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Source/Core/Platform/*Win.*",
			"Source/Core/Platform/*Mac.*",
		}

	configuration { "SharedLib", "linux" }
		links
		{
			"pthread",
			"dl",
		}

	configuration {}

project( prefix .. "PlatformTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Source/Core/Platform/*Tests.*",
	}

	links
	{
		prefix .. "Platform"
	}

project( prefix .. "Foundation" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Foundation", "FOUNDATION" )

	files
	{
		"Source/Core/Foundation/**",
	}

	excludes
	{
		"Source/Core/Foundation/*Tests.*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
		}

	configuration {}

project( prefix .. "FoundationTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Source/Core/Foundation/*Tests.*",
	}

	links
	{
		prefix .. "Platform",
		prefix .. "Foundation"
	}

project( prefix .. "Application" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Source/Core/Application/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

	configuration {}

project( prefix .. "Reflect" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Source/Core/Reflect/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

	configuration {}

project( prefix .. "Persist" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Persist", "PERSIST" )

	files
	{
		"Source/Core/Persist/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			"mongo-c",
		}

	configuration {}

project( prefix .. "Mongo" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Mongo", "MONGO" )

	files
	{
		"Source/Core/Mongo/**",
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

	configuration {}

project( prefix .. "Inspect" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Inspect", "INSPECT" )

	files
	{
		"Source/Core/Inspect/**",
	}

	includedirs
	{
		"Source/Core/Inspect",
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

	configuration {}

project( prefix .. "Math" )

	Helium.DoModuleProjectSettings( "Source/Core", "HELIUM", "Math", "MATH" )

	files
	{
		"Source/Core/Math/**",
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

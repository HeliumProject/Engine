require "Dependencies/Helium"
require "Dependencies/fbx"

require "Helium"

project( prefix .. "Platform" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Platform", "PLATFORM" )

	files
	{
		"Core/Source/Platform/*.cpp",
		"Core/Source/Platform/*.h",
		"Core/Source/Platform/*.inl",
	}

	excludes
	{
		"Core/Source/Platform/*Tests.*",
	}

	configuration "windows"
		excludes
		{
			"Core/Source/Platform/*Posix.*",
			"Core/Source/Platform/*Mac.*",
			"Core/Source/Platform/*Lin.*",
		}

	configuration "macosx"
		excludes
		{
			"Core/Source/Platform/*Win.*",
			"Core/Source/Platform/*Lin.*",
		}

	configuration "linux"
		excludes
		{
			"Core/Source/Platform/*Win.*",
			"Core/Source/Platform/*Mac.*",
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
		"Core/Source/Platform/*Tests.*",
	}

	links
	{
		prefix .. "Platform"
	}

project( prefix .. "Foundation" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Foundation", "FOUNDATION" )

	files
	{
		"Core/Source/Foundation/**",
	}

	excludes
	{
		"Core/Source/Foundation/*Tests.*",
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
		"Core/Source/Foundation/*Tests.*",
	}

	links
	{
		prefix .. "Platform",
		prefix .. "Foundation"
	}

project( prefix .. "Application" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Core/Source/Application/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

	configuration {}

project( prefix .. "Reflect" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Core/Source/Reflect/**",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
		}

	configuration {}

project( prefix .. "Persist" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Persist", "PERSIST" )

	files
	{
		"Core/Source/Persist/**",
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

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Mongo", "MONGO" )

	files
	{
		"Core/Source/Mongo/**",
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

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Inspect", "INSPECT" )

	files
	{
		"Core/Source/Inspect/**",
	}

	includedirs
	{
		"Core/Source/Inspect",
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
		}

	configuration {}

project( prefix .. "Math" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Math", "MATH" )

	files
	{
		"Core/Source/Math/**",
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

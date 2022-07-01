require "Dependencies/premake"
require "Dependencies/premake-fbx"

require "premake"

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

	filter "system:windows"
		excludes
		{
			"Core/Source/Platform/*Posix.*",
			"Core/Source/Platform/*Mac.*",
			"Core/Source/Platform/*Lin.*",
		}

	filter "system:macosx"
		excludes
		{
			"Core/Source/Platform/*Win.*",
			"Core/Source/Platform/*Lin.*",
		}

	filter "system:linux"
		excludes
		{
			"Core/Source/Platform/*Win.*",
			"Core/Source/Platform/*Mac.*",
		}

	filter { "system:linux", "kind:SharedLib" }
		links
		{
			"pthread",
			"dl",
		}

	filter {}

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

	filter "kind:SharedLib"
		links
		{
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "FoundationTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Foundation/*Tests.*",
	}

	links
	{
		prefix .. "Foundation",
		prefix .. "Platform",
	}

project( prefix .. "Application" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Application", "APPLICATION" )

	files
	{
		"Core/Source/Application/**",
	}

	excludes
	{
		"Core/Source/Application/*Tests.*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "ApplicationTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Application/*Tests.*",
	}

	links
	{
		prefix .. "Application",
		prefix .. "Foundation",
		prefix .. "Platform",
	}

project( prefix .. "Reflect" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Reflect", "REFLECT" )

	files
	{
		"Core/Source/Reflect/**",
	}

	excludes
	{
		"Core/Source/Reflect/*Tests.*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Foundation",
			prefix .. "Platform",
		}

	filter {}

project( prefix .. "ReflectTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Reflect/*Tests.*",
	}

	links
	{
		prefix .. "Reflect",
		prefix .. "Foundation",
		prefix .. "Platform"
	}

project( prefix .. "Persist" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Persist", "PERSIST" )

	files
	{
		"Core/Source/Persist/**",
	}

	excludes
	{
		"Core/Source/Persist/*Tests.*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			"mongo-c",
		}

	filter {}

project( prefix .. "PersistTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Persist/*Tests.*",
	}

	links
	{
		prefix .. "Persist",
		prefix .. "Reflect",
		prefix .. "Foundation",
		prefix .. "Platform"
	}

project( prefix .. "Mongo" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Mongo", "MONGO" )

	files
	{
		"Core/Source/Mongo/**",
	}

	excludes
	{
		"Core/Source/Mongo/*Tests.*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			"mongo-c",
		}

	filter {}

project( prefix .. "MongoTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Mongo/*Tests.*",
	}

	links
	{
		prefix .. "Mongo",
		prefix .. "Persist",
		prefix .. "Reflect",
		prefix .. "Foundation",
		prefix .. "Platform",
		"mongo-c",
	}

project( prefix .. "Inspect" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Inspect", "INSPECT" )

	files
	{
		"Core/Source/Inspect/**",
	}

	excludes
	{
		"Core/Source/Inspect/*Tests.*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Application",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
		}

	filter {}

project( prefix .. "InspectTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Inspect/*Tests.*",
	}

	links
	{
		prefix .. "Inspect",
		prefix .. "Reflect",
		prefix .. "Foundation",
		prefix .. "Platform"
	}

project( prefix .. "Math" )

	Helium.DoModuleProjectSettings( "Core/Source", "HELIUM", "Math", "MATH" )

	files
	{
		"Core/Source/Math/**",
	}

	excludes
	{
		"Core/Source/Math/*Tests.*",
	}

	filter "kind:SharedLib"
		links
		{
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
		}

	filter {}

project( prefix .. "MathTests" )

	Helium.DoTestsProjectSettings()

	files
	{
		"Core/Source/Math/*Tests.*",
	}

	links
	{
		prefix .. "Reflect",
		prefix .. "Foundation",
		prefix .. "Platform"
	}

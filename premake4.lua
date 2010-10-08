--[[

Notes:
* keep in mind that solution-wide settings should be defined
  before ever mentioning configuration-specific settings because
  the indentation in this file is for humans (not premake), and
  premake might infer conditionals when walking the graph of 
  solutions/configurations/projects -Geoff

--]]

solution "Helium"

	location "Premake"
	
    platforms
    {
		"native"
	}

	configurations
	{
		"Debug",
		"Debug Unicode",
		"Release",
		"Release Unicode"
	}

	flags
	{
		"NoMinimalRebuild"
	}
	
	includedirs
	{
		"."
	}
	
	configuration "Debug"
		targetdir "Bin/Debug"

	configuration "Debug Unicode"
		targetdir "Bin/Debug Unicode"

	configuration "Release"
		targetdir "Bin/Release"

	configuration "Release Unicode"
		targetdir "Bin/Release Unicode"
		
	configuration "windows"
		defines
		{
			"WIN32=1",
		}
    
	project "Platform"
		kind "SharedLib"
		language "C++"
		files { "Platform/*.h", "Platform/*.cpp" }
				
		configuration "windows"
			files{ "Platform/Windows/*.h", "Platform/Windows/*.cpp" }
			links { "ws2_32" }

		configuration "macosx"
			files{ "Platform/POSIX/*.h", "Platform/POSIX/*.cpp" }
			
		configuration "linux"
			files{ "Platform/POSIX/*.h", "Platform/POSIX/*.cpp" }
			
		configuration "bsd"
			files{ "Platform/POSIX/*.h", "Platform/POSIX/*.cpp" }

	project "Foundation"
		kind "SharedLib"
		language "C++"
		files { "Foundation/**.h", "Foundation/**.cpp" }
		links { "Platform" }

	project "Core"
		kind "SharedLib"
		language "C++"
		files { "Core/**.h", "Core/**.cpp" }
		links { "Foundation", "Platform" }

	project "Pipeline"
		kind "SharedLib"
		language "C++"
		files { "Pipeline/**.h", "Pipeline/**.cpp" }
		links { "Foundation", "Platform" }

	project "Editor"
		kind "ConsoleApp"
		language "C++"
		files { "Editor/**.h", "Editor/**.cpp" }
		links { "Pipeline", "Core", "Foundation", "Platform" }

	configuration "Debug*"
		defines { "_DEBUG", "DEBUG" }
		flags { "Symbols" }

	configuration "Release*"
		defines { "NDEBUG" }
		flags { "Optimize" }

	configuration "*Unicode"
		defines { "UNICODE=1" }

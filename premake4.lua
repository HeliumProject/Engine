function DoDefaultSolutionSetup()

	location "Premake"
	
    platforms
    {
		"x32",
		"x64",
	}

	configurations
	{
		"Debug",
		"Debug Unicode",
		"Release",
		"Release Unicode",
	}

--[[
	Iterate through our platforms and configurations and set them up with appropriate
	target directories (that have both the configuration and platform built into them)
--]]
	for i, platform in ipairs( platforms() ) do
		for j, config in ipairs( configurations() ) do
			configuration( { config, platform } )
				targetdir( "Bin/" .. config .. "/" .. platform )
		end
	end

--[[
	Keep in mind that solution-wide settings should be defined
	before ever mentioning configuration-specific settings because
	the indentation in this file is for humans (not premake), and
	premake might infer conditionals when walking the graph of 
	solutions/configurations/projects -Geoff
--]]

	configuration "windows"
		defines
		{
			"_WIN32",
			"WIN32",
			"_CRT_SECURE_NO_DEPRECATE",
		}
		flags
		{
			"NoMinimalRebuild",
		}

	configuration "Debug*"
		defines
		{
			"_DEBUG",
			"DEBUG",
		}
		flags
		{
			"Symbols",
		}

	configuration "Release*"
		defines
		{
			"NDEBUG",
		}
		flags
		{
			"Optimize",
		}

	configuration "*Unicode"
		defines
		{
			"UNICODE=1",
			"LITESQL_UNICODE=1",
			"XML_UNICODE_WCHAR_T=1",
		}
		flags
		{
			"Unicode",
		}

end

solution "Dependencies"

	DoDefaultSolutionSetup()
	
	project "Expat"
		kind "StaticLib"
		language "C++"
		defines
		{
			"COMPILED_FROM_DSP",
		}
		files
		{
			"Dependencies/Expat/*.h",
			"Dependencies/Expat/*.c",
		}

	project "LiteSQL"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"Dependencies/Expat",
			"Dependencies/LiteSQL/include",
			"Dependencies/LiteSQL/src/library",
		}
		files
		{
			"Dependencies/LiteSQL/include/**.h",
			"Dependencies/LiteSQL/include/**.hpp",
			"Dependencies/LiteSQL/src/library/**.cpp",
			"Dependencies/LiteSQL/src/library/**.h",
			"Dependencies/LiteSQL/src/library/**.hpp",
			"Dependencies/LiteSQL/src/generator/generator.cpp",
			"Dependencies/LiteSQL/src/generator/generator.hpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-cpp.cpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-cpp.hpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-graphviz.cpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-graphviz.hpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-ruby-activerecord.cpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-ruby-activerecord.hpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-xml.cpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen-xml.hpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen.cpp",
			"Dependencies/LiteSQL/src/generator/litesql-gen.hpp",
			"Dependencies/LiteSQL/src/generator/logger.cpp",
			"Dependencies/LiteSQL/src/generator/logger.hpp",
			"Dependencies/LiteSQL/src/generator/md5.cpp",
			"Dependencies/LiteSQL/src/generator/md5.hpp",
			"Dependencies/LiteSQL/src/generator/objectmodel.cpp",
			"Dependencies/LiteSQL/src/generator/objectmodel.hpp",
			"Dependencies/LiteSQL/src/generator/xmlobjects.cpp",
			"Dependencies/LiteSQL/src/generator/xmlobjects.hpp",
			"Dependencies/LiteSQL/src/generator/xmlparser.cpp",
			"Dependencies/LiteSQL/src/generator/xmlparser.hpp",
		}

solution "Helium"

	defines
	{
		"PLATFORM_DLL=1",
		"FOUNDATION_DLL=1",
		"PIPELINE_DLL=1",
		"CORE_DLL=1",
	}

	includedirs
	{
		".",
	}

	DoDefaultSolutionSetup()

	project "Platform"
		kind "SharedLib"
		language "C++"
		files
		{
			"Platform/*.h",
			"Platform/*.cpp",
		}

		configuration "windows"
			files
			{
				"Platform/Windows/*.h",
				"Platform/Windows/*.cpp",
			}
			links
			{
				"ws2_32",
			}

		configuration "macosx"
			files
			{
				"Platform/POSIX/*.h",
				"Platform/POSIX/*.cpp",
			}
			
		configuration "linux"
			files
			{
				"Platform/POSIX/*.h",
				"Platform/POSIX/*.cpp",
			}
			
		configuration "bsd"
			files
			{
				"Platform/POSIX/*.h",
				"Platform/POSIX/*.cpp",
			}

	project "Foundation"
		kind "SharedLib"
		language "C++"
		files
		{
			"Foundation/**.h",
			"Foundation/**.cpp",
		}
		links
		{
			"Platform",
		}

	project "Pipeline"
		kind "SharedLib"
		language "C++"
		files
		{
			"Pipeline/**.h",
			"Pipeline/**.cpp",
		}
		links
		{
			"Platform",
			"Foundation",
		}

	project "Core"
		kind "SharedLib"
		language "C++"
		files
		{
			"Core/**.h",
			"Core/**.cpp",
		}
		links
		{
			"Foundation",
			"Platform",
		}

	project "Editor"
		kind "ConsoleApp"
		language "C++"
		files
		{
			"Editor/**.h",
			"Editor/**.cpp",
		}
		links
		{
			"Platform",
			"Foundation",
			"Pipeline",
			"Core",
		}

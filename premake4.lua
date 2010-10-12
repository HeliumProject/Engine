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

function BuildWxWidgets()

	local cwd = os.getcwd()

	if os.get() == "windows" then

		os.chdir( "Dependencies/wxWidgets/build/msw" );

		local make
		local base = "nmake.exe -f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		make = "cmd.exe /c " .. cwd .. "\\vc32.bat " .. base	
		os.execute( make .. " BUILD=debug UNICODE=0" )
		os.execute( make .. " BUILD=debug UNICODE=1" )
		os.execute( make .. " BUILD=release UNICODE=0" )
		os.execute( make .. " BUILD=release UNICODE=1" )
		
		make = "cmd.exe /c " .. cwd .. "\\vc64.bat " .. base
		os.execute( make .. " BUILD=debug TARGET_CPU=AMD64 UNICODE=0" )
		os.execute( make .. " BUILD=debug TARGET_CPU=AMD64 UNICODE=1" )
		os.execute( make .. " BUILD=release TARGET_CPU=AMD64 UNICODE=0" )
		os.execute( make .. " BUILD=release TARGET_CPU=AMD64 UNICODE=1" )

	else
		print("Add support for " .. os.get() .. " to BuildWxWidgets()")
		os.exit(1)
	end

	os.chdir( cwd );

end

solution "Dependencies"

	BuildWxWidgets()

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

	project "Lua"
		kind "StaticLib"
		language "C++"
		files
		{
			"Dependencies/lua/src/*.h",
			"Dependencies/lua/src/*.c",
		}
		excludes
		{
			"Dependencies/lua/src/luac.c",
		}
		
	project "nvtt"
		kind "StaticLib"
		language "C++"
		defines
		{
			"__SSE2__",
			"__SSE__",
			"__MMX__",
		}
		includedirs
		{
			"Dependencies/nvtt/src",
		}
		files
		{
			"Dependencies/nvtt/src/nvmath/*.h",
			"Dependencies/nvtt/src/nvmath/*.cpp",
			"Dependencies/nvtt/src/nvcore/*.h",
			"Dependencies/nvtt/src/nvcore/*.cpp",
			"Dependencies/nvtt/src/nvimage/*.h",
			"Dependencies/nvtt/src/nvimage/*.cpp",
		}
		excludes
		{
			"Dependencies/nvtt/src/nvcore/Tokenizer.h",
			"Dependencies/nvtt/src/nvcore/Tokenizer.cpp",
			"Dependencies/nvtt/src/nvimage/ConeMap.h",
			"Dependencies/nvtt/src/nvimage/ConeMap.cpp",
		}
		
		configuration "windows"
			includedirs
			{
				"Dependencies/nvtt/project/vc8",
			}

	project "squish"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"Dependencies/squish",
		}
		files
		{
			"Dependencies/squish/*.h",
			"Dependencies/squish/*.inl",
			"Dependencies/squish/*.cpp",
		}

	project "tiff"
		kind "StaticLib"
		language "C++"
		includedirs
		{
			"Dependencies/tiff",
			"Dependencies/tiff/libtiff",
		}
		files
		{
			"Dependencies/tiff/libtiff/*.h",
			"Dependencies/tiff/libtiff/*.c",
		}
		
		configuration "windows"
			excludes
			{
				"Dependencies/tiff/libtiff/tif_acorn.c",
				"Dependencies/tiff/libtiff/tif_atari.c",
				"Dependencies/tiff/libtiff/tif_apple.c",
				"Dependencies/tiff/libtiff/tif_next.c",
				"Dependencies/tiff/libtiff/tif_msdos.c",
				"Dependencies/tiff/libtiff/tif_unix.c",
				"Dependencies/tiff/libtiff/tif_win3.c",
			}

		configuration "macosx"
			excludes
			{
				"Dependencies/tiff/libtiff/tif_acorn.c",
				"Dependencies/tiff/libtiff/tif_atari.c",
				"Dependencies/tiff/libtiff/tif_next.c",
				"Dependencies/tiff/libtiff/tif_msdos.c",
				"Dependencies/tiff/libtiff/tif_unix.c",
				"Dependencies/tiff/libtiff/tif_win3.c",
				"Dependencies/tiff/libtiff/tif_win32.c",
			}

		configuration "linux"
			excludes
			{
				"Dependencies/tiff/libtiff/tif_acorn.c",
				"Dependencies/tiff/libtiff/tif_atari.c",
				"Dependencies/tiff/libtiff/tif_apple.c",
				"Dependencies/tiff/libtiff/tif_next.c",
				"Dependencies/tiff/libtiff/tif_msdos.c",
				"Dependencies/tiff/libtiff/tif_win3.c",
				"Dependencies/tiff/libtiff/tif_win32.c",
			}

	project "zlib"
		kind "StaticLib"
		language "C++"
		files
		{
			"Dependencies/zlib/*.h",
			"Dependencies/zlib/*.c",
		}
		excludes
		{
			"Dependencies/zlib/gz*.h",
			"Dependencies/zlib/gz*.c",
			"Dependencies/zlib/minigzip.c",
		}

solution "Helium"

	defines
	{
		"PLATFORM_DLL=1",
		"FOUNDATION_DLL=1",
		"PIPELINE_DLL=1",
		"CORE_DLL=1",
		"XML_STATIC=1",
		"WXUSINGDLL=1",
		"wxUSE_UNICODE=0",
		"wxNO_EXPAT_LIB=1",
		"wxNO_JPEG_LIB=1",
		"wxNO_PNG_LIB=1",
		"wxNO_TIFF_LIB=1",
		"wxNO_ZLIB_LIB=1",
	}

	includedirs
	{
		".",
	}
	
	DoDefaultSolutionSetup()

--[[
	We build monolithic wx, so ignore all the legacy non-monolithic
	#pragma comment directives (on windows only)
--]]
	configuration "windows"
		buildoptions
		{
			"/NODEFAULTLIB wxbase29ud;wxbase29d;wxbase29u;wxbase29;wxbase29ud_net;wxbase29d_net;wxbase29u_net;wxbase29_net;wxbase29ud_xml;wxbase29d_xml;wxbase29u_xml;wxbase29_xml;wxmsw29ud_core;wxmsw29d_core;wxmsw29u_core;wxmsw29_core;wxmsw29ud_adv;wxmsw29d_adv;wxmsw29u_adv;wxmsw29_adv;wxmsw29ud_html;wxmsw29d_html;wxmsw29u_html;wxmsw29_html;wxmsw29ud_qa;wxmsw29d_qa;wxmsw29u_qa;wxmsw29_qa;wxmsw29ud_xrc;wxmsw29d_xrc;wxmsw29u_xrc;wxmsw29_xrc;wxmsw29ud_aui;wxmsw29d_aui;wxmsw29u_aui;wxmsw29_aui;wxmsw29ud_propgrid;wxmsw29d_propgrid;wxmsw29u_propgrid;wxmsw29_propgrid;wxmsw29ud_ribbon;wxmsw29d_ribbon;wxmsw29u_ribbon;wxmsw29_ribbon;wxmsw29ud_richtext;wxmsw29d_richtext;wxmsw29u_richtext;wxmsw29_richtext;wxmsw29ud_media;wxmsw29d_media;wxmsw29u_media;wxmsw29_media;wxmsw29ud_stc;wxmsw29d_stc;wxmsw29u_stc;wxmsw29_stc",
		}

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

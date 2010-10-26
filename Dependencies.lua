require "Base"

configuration {}

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
		"Dependencies/LiteSQL/include/**.cpp",
		"Dependencies/LiteSQL/include/**.h",
		"Dependencies/LiteSQL/include/**.hpp",
		"Dependencies/LiteSQL/src/library/**.c",
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
			"Dependencies/tiff/libtiff/tif_msdos.c",
			"Dependencies/tiff/libtiff/tif_unix.c",
			"Dependencies/tiff/libtiff/tif_win3.c",
		}
	configuration "macosx"
		excludes
		{
			"Dependencies/tiff/libtiff/tif_acorn.c",
			"Dependencies/tiff/libtiff/tif_atari.c",
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
	
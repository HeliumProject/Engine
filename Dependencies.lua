require "Helium"

solution "Dependencies"

Helium.DoDefaultSolutionSettings()

configuration {}

project "Expat"
	uuid "224FF97E-122E-4515-AB71-CBE11D3EC210"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
	defines
	{
		"COMPILED_FROM_DSP",
	}
	files
	{
		"Dependencies/Expat/*.h",
		"Dependencies/Expat/*.c",
	}

project "freetype"
	uuid "53C96BED-38E8-4A1f-81E0-45D09AFD33EB"
	kind "StaticLib"
	language "C"
	location( "Premake/" .. solution().name )
	defines
	{
		"_LIB",
		"FT2_BUILD_LIBRARY",
	}
	includedirs
	{
		"Dependencies/freetype/include",
	}
	files
	{
		"Dependencies/freetype/src/autofit/autofit.c",
		"Dependencies/freetype/src/bdf/bdf.c",
		"Dependencies/freetype/src/cff/cff.c",
		"Dependencies/freetype/src/base/ftbase.c",
		"Dependencies/freetype/src/base/ftbitmap.c",
		"Dependencies/freetype/src/cache/ftcache.c",
		"Dependencies/freetype/src/base/ftfstype.c",
		"Dependencies/freetype/src/base/ftgasp.c",
		"Dependencies/freetype/src/base/ftglyph.c",
		"Dependencies/freetype/src/gzip/ftgzip.c",
		"Dependencies/freetype/src/base/ftinit.c",
		"Dependencies/freetype/src/lzw/ftlzw.c",
		"Dependencies/freetype/src/base/ftstroke.c",
		"Dependencies/freetype/src/base/ftsystem.c",
		"Dependencies/freetype/src/smooth/smooth.c",
		"Dependencies/freetype/src/base/ftbbox.c",
		"Dependencies/freetype/src/base/ftmm.c",
		"Dependencies/freetype/src/base/ftpfr.c",
		"Dependencies/freetype/src/base/ftsynth.c",
		"Dependencies/freetype/src/base/fttype1.c",
		"Dependencies/freetype/src/base/ftwinfnt.c",
		"Dependencies/freetype/src/base/ftxf86.c",
		"Dependencies/freetype/src/base/ftlcdfil.c",
		"Dependencies/freetype/src/base/ftgxval.c",
		"Dependencies/freetype/src/base/ftotval.c",
		"Dependencies/freetype/src/base/ftpatent.c",
		"Dependencies/freetype/src/pcf/pcf.c",
		"Dependencies/freetype/src/pfr/pfr.c",
		"Dependencies/freetype/src/psaux/psaux.c",
		"Dependencies/freetype/src/pshinter/pshinter.c",
		"Dependencies/freetype/src/psnames/psmodule.c",
		"Dependencies/freetype/src/raster/raster.c",
		"Dependencies/freetype/src/sfnt/sfnt.c",
		"Dependencies/freetype/src/truetype/truetype.c",
		"Dependencies/freetype/src/type1/type1.c",
		"Dependencies/freetype/src/cid/type1cid.c",
		"Dependencies/freetype/src/type42/type42.c",
		"Dependencies/freetype/src/winfonts/winfnt.c",
		"Dependencies/freetype/include/ft2build.h",
		"Dependencies/freetype/include/freetype/config/ftconfig.h",
		"Dependencies/freetype/include/freetype/config/ftheader.h",
		"Dependencies/freetype/include/freetype/config/ftmodule.h",
		"Dependencies/freetype/include/freetype/config/ftoption.h",
		"Dependencies/freetype/include/freetype/config/ftstdlib.h",
	}
	configuration "windows"
		files
		{
			"Dependencies/freetype/builds/win32/ftdebug.c",
		}

project "LiteSQL"
	uuid "6D880828-B2BC-4FB5-A8E2-75FBE177349A"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
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

project "lua"
	uuid "A6339978-DA6B-4DEE-9997-0D60052CFFC3"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
	files
	{
		"Dependencies/lua/src/*.h",
		"Dependencies/lua/src/*.c",
	}
	excludes
	{
		"Dependencies/lua/src/lua.c",
		"Dependencies/lua/src/luac.c",
	}
	
project "nvtt"
	uuid "6753B918-F16E-4C13-8DA7-4F9A6DB58B77"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
	includedirs
	{
		"Dependencies/nvtt/src",
		"Dependencies/nvtt/src/nvtt/squish",
	}
	files
	{
		"Dependencies/nvtt/src/nvmath/*.h",
		"Dependencies/nvtt/src/nvmath/*.cpp",
		"Dependencies/nvtt/src/nvcore/*.h",
		"Dependencies/nvtt/src/nvcore/*.cpp",
		"Dependencies/nvtt/src/nvimage/*.h",
		"Dependencies/nvtt/src/nvimage/*.cpp",
		"Dependencies/nvtt/src/nvtt/*.h",
		"Dependencies/nvtt/src/nvtt/*.cpp",
		"Dependencies/nvtt/src/nvtt/cuda/*.h",
		"Dependencies/nvtt/src/nvtt/cuda/*.cpp",
		"Dependencies/nvtt/src/nvtt/squish/*.h",
		"Dependencies/nvtt/src/nvtt/squish/*.cpp",
	}
	excludes
	{
		"Dependencies/nvtt/src/nvcore/Tokenizer.h",
		"Dependencies/nvtt/src/nvcore/Tokenizer.cpp",
		"Dependencies/nvtt/src/nvimage/ConeMap.h",
		"Dependencies/nvtt/src/nvimage/ConeMap.cpp",
		"Dependencies/nvtt/src/nvtt/squish/singlechannelfit.*",
	}
	
	configuration "windows"
		includedirs
		{
			"Dependencies/nvtt/project/vc8",
		}

	-- Override inline function expansion and intrinsic function usage settings for Debug builds.
	configuration { "windows", "Debug" }
		buildoptions
		{
			"/Ob2",
			"/Oi",
		}
		flags
		{
			"NoEditAndContinue",  -- Edit and continue support is not compatible with /Ob2
		}

project "png"
	uuid "46BA228E-C636-4468-9CBD-7CD4F12FBB33"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
	includedirs
	{
		"Dependencies/zlib"
	}
	files
	{
		"Dependencies/png/*.h",
		"Dependencies/png/*.c",
	}

project "tiff"
	uuid "FAB06C82-5A66-419A-ACD1-6F771984E35C"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
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
	uuid "23112391-0616-46AF-B0C2-5325E8530FBA"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
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
	
require "Helium"

solution "Dependencies"

Helium.DoDefaultSolutionSettings()

configuration {}

project "expat"
	uuid "224FF97E-122E-4515-AB71-CBE11D3EC210"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )

	files
	{
		"Dependencies/expat/lib/*.h",
		"Dependencies/expat/lib/*.c",
	}

	configuration "windows"
		defines
		{
			"COMPILED_FROM_DSP",
		}

	configuration "macosx"
		defines
		{
			"HAVE_MEMMOVE",
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
	
project "nvtt"
	uuid "6753B918-F16E-4C13-8DA7-4F9A6DB58B77"
	kind "StaticLib"
	language "C++"
	location( "Premake/" .. solution().name )
	includedirs
	{
		"Dependencies/nvtt/extern/poshlib",
		"Dependencies/nvtt/src",
		"Dependencies/nvtt/src/nvtt/squish",
	}
	files
	{
		"Dependencies/nvtt/extern/poshlib/*.h",
		"Dependencies/nvtt/extern/poshlib/*.cpp",
		"Dependencies/nvtt/src/nvmath/*.h",
		"Dependencies/nvtt/src/nvmath/*.cpp",
		"Dependencies/nvtt/src/nvcore/*.h",
		"Dependencies/nvtt/src/nvcore/*.cpp",
		"Dependencies/nvtt/src/nvthread/*.h",
		"Dependencies/nvtt/src/nvthread/*.cpp",
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
		"Dependencies/nvtt/src/nvcore/Tokenizer.*",
		"Dependencies/nvtt/src/nvimage/ConeMap.*",
		"Dependencies/nvtt/src/nvimage/KtxFile.*",
		"Dependencies/nvtt/src/nvtt/squish/alpha.*",
		"Dependencies/nvtt/src/nvtt/squish/clusterfit.*",
		"Dependencies/nvtt/src/nvtt/squish/rangefit.*",
		"Dependencies/nvtt/src/nvtt/squish/singlecolourfit.*",
		"Dependencies/nvtt/src/nvtt/squish/singlechannelfit.*",
		"Dependencies/nvtt/src/nvtt/squish/squish.*",
		"Dependencies/nvtt/src/nvtt/CompressorDX11.*",
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

	configuration "macosx"
		defines
		{
			"HAVE_STDARG_H",
			"HAVE_MALLOC_H",
		}
		includedirs
		{
			"Dependencies/nvtt/project/xcode4",
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
	
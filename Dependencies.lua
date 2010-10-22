require "Base"

Helium.BuildWxWidgets = function( root )

	local cwd = os.getcwd()

	if os.get() == "windows" then
		local make = "nmake.exe -f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( root .. "/Dependencies/wxWidgets/build/msw" );

		local result
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end

		os.chdir( cwd )

		local files = {}
		files[1]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291d_vc_custom.dll",		built="Bin/x32/Debug" }
		files[2]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291d_vc_custom.pdb",		built="Bin/x32/Debug" }
		files[3]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291_vc_custom.dll",		built="Bin/x32/Release" }
		files[4]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291_vc_custom.pdb",		built="Bin/x32/Release" }
		files[5]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291ud_vc_custom.dll",	built="Bin/x32/DebugUnicode" }
		files[6]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291ud_vc_custom.pdb",	built="Bin/x32/DebugUnicode" }
		files[7]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291u_vc_custom.dll",		built="Bin/x32/ReleaseUnicode" }
		files[8]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_dll", 		file="wxmsw291u_vc_custom.pdb",		built="Bin/x32/ReleaseUnicode" }
		files[9]  = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291d_vc_custom.dll",		built="Bin/x64/Debug" }
		files[10] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291d_vc_custom.pdb",		built="Bin/x64/Debug" }
		files[11] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291_vc_custom.dll",		built="Bin/x64/Release" }
		files[12] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291_vc_custom.pdb",		built="Bin/x64/Release" }
		files[13] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291ud_vc_custom.dll",	built="Bin/x64/DebugUnicode" }
		files[14] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291ud_vc_custom.pdb",	built="Bin/x64/DebugUnicode" }
		files[15] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291u_vc_custom.dll",		built="Bin/x64/ReleaseUnicode" }
		files[16] = { dir=root .. "/Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291u_vc_custom.pdb",		built="Bin/x64/ReleaseUnicode" }
		Helium.Publish( files )

	else
		print("Implement support for " .. os.get() .. " to BuildWxWidgets()")
		os.exit(1)
	end

end

Helium.CleanWxWidgets = function( root )

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then
		local make = "nmake.exe -f makefile.vc clean SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		if not os.getenv("VCINSTALLDIR") then
			print("VCINSTALLDIR is not detected in your environment")
			os.exit(1)
		end
				
		os.chdir( root .. "/Dependencies/wxWidgets/build/msw" );

		local result
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. make .. " BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
		result = os.execute( "cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. make .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
		if result ~= 0 then os.exit( 1 ) end
	else
		print("Implement support for " .. os.get() .. " to CleanWxWidgets()")
		os.exit(1)
	end

end

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
		"Dependencies/lua/src/lua.c",
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
	
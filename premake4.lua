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
		"DebugUnicode",
		"Release",
		"ReleaseUnicode",
	}

--[[
	Iterate through our platforms and configurations and set them up with appropriate
	target directories (that have both the configuration and platform built into them)
--]]
	for i, platform in ipairs( platforms() ) do
		for j, config in ipairs( configurations() ) do
			configuration( { config, platform } )
				targetdir( "Bin/" .. platform .. "/" .. config )
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

function Sleep( seconds )
	if os.get() == "windows" then
		os.execute("ping 127.0.0.1 -n " .. seconds + 1 .. " -w 1000 >:nul 2>&1")
	else
		os.execute("sleep " .. seconds)
	end
end

function WaitForResults( files )

	print( "Waiting for build results..." )
	local quit = false
	while quit == false do
		local found = 0
		for i,v in pairs(files) do
			if v then
				-- we still have files to process
				found = found + 1

				-- mkpath the target folder
				os.mkdir( v.built )
				
				local path = v.dir .. "/" .. v.file			
				local exists = os.isfile( path )
				
				if exists then
					local destination = v.built .. "/" .. v.file

					-- cull existing files
					if os.isfile( destination ) then
						os.execute( "del /q \"" .. string.gsub( destination, "/", "\\" ) .. "\"" )
					end

					-- do the file copy
					local result = os.execute( "mklink \"" .. destination .. "\" \"" .. path .. "\"" )

					-- the files were copied, complete this entry
					if result == 0 then
						files[ i ] = nil
					end						
				end
			end
		end
		quit = found == 0
		Sleep(1)
	end
	print( "Build results published..." )
	
end

function BuildWxWidgets()

	local cwd = os.getcwd()

	local files = {}
	
	if os.get() == "windows" then
		local make
		local base = "nmake.exe -f makefile.vc SHARED=1 MONOLITHIC=1 DEBUG_INFO=1"

		os.chdir( "Dependencies/wxWidgets/build/msw" );
		os.execute( "start \"Debug ASCII 32-bit    \" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. base .. " BUILD=debug UNICODE=0\"" )
		os.execute( "start \"Release ASCII 32-bit  \" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. base .. " BUILD=release UNICODE=0\"" )
		os.execute( "start \"Debug ASCII 64-bit    \" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. base .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=0\"" )
		os.execute( "start \"Release ASCII 64-bit  \" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. base .. " TARGET_CPU=AMD64 BUILD=release UNICODE=0\"" )
		files[0] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291d_vc_custom.dll",		built="Bin/Debug/x32" }
		files[1] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291d_vc_custom.pdb",		built="Bin/Debug/x32" }
		files[2] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291_vc_custom.dll",		built="Bin/Release/x32" }
		files[3] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291_vc_custom.pdb",		built="Bin/Release/x32" }
		files[4] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291d_vc_custom.dll",		built="Bin/Debug/x64" }
		files[5] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291d_vc_custom.pdb",		built="Bin/Debug/x64" }
		files[6] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291_vc_custom.dll",		built="Bin/Release/x64" }
		files[7] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291_vc_custom.pdb",		built="Bin/Release/x64" }
		os.chdir( cwd )
		WaitForResults( files )

		os.chdir( "Dependencies/wxWidgets/build/msw" );
		os.execute( "start \"Debug UNICODE 32-bit  \" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. base .. " BUILD=debug UNICODE=1\"" )
		os.execute( "start \"Release UNICODE 32-bit\" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat x86 && " .. base .. " BUILD=release UNICODE=1\"" )
		os.execute( "start \"Debug UNICODE 64-bit  \" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. base .. " TARGET_CPU=AMD64 BUILD=debug UNICODE=1\"" )
		os.execute( "start \"Release UNICODE 64-bit\" cmd.exe /c \"call \"%VCINSTALLDIR%\"\\vcvarsall.bat amd64 && " .. base .. " TARGET_CPU=AMD64 BUILD=release UNICODE=1\"" )
		files[0] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291ud_vc_custom.dll",	built="Bin/DebugUnicode/x32" }
		files[1] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291ud_vc_custom.pdb",	built="Bin/DebugUnicode/x32" }
		files[2] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291u_vc_custom.dll",		built="Bin/ReleaseUnicode/x32" }
		files[3] = { dir="Dependencies/wxWidgets/lib/vc_dll", 			file="wxmsw291u_vc_custom.pdb",		built="Bin/ReleaseUnicode/x32" }
		files[4] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291ud_vc_custom.dll",	built="Bin/DebugUnicode/x64" }
		files[5] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291ud_vc_custom.pdb",	built="Bin/DebugUnicode/x64" }
		files[6] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291u_vc_custom.dll",		built="Bin/ReleaseUnicode/x64" }
		files[7] = { dir="Dependencies/wxWidgets/lib/vc_amd64_dll", 	file="wxmsw291u_vc_custom.pdb",		built="Bin/ReleaseUnicode/x64" }
		os.chdir( cwd )
		WaitForResults( files )

	else
		print("Implement support for " .. os.get() .. " to BuildWxWidgets()")
		os.exit(1)
	end

end

solution "Dependencies"

	if _ACTION ~= "clean" then
		BuildWxWidgets()
	end

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
	
	buildoptions
	{
		-- Class 'foo<>' needs to have dll-interface to be used by clients of class 'bar'
		--  This is a non-issue so long as debug/release and CRT is not mixed b/t modules
		"/wd4251",
	}
	
	DoDefaultSolutionSetup()

--[[
	We build monolithic wx, so ignore all the legacy non-monolithic
	#pragma comment directives (on windows only)
--]]
	configuration "windows"
		linkoptions
		{
			"/NODEFAULTLIB:wxbase29ud",
			"/NODEFAULTLIB:wxbase29d",
			"/NODEFAULTLIB:wxbase29u",
			"/NODEFAULTLIB:wxbase29",
			"/NODEFAULTLIB:wxbase29ud_net",
			"/NODEFAULTLIB:wxbase29d_net",
			"/NODEFAULTLIB:wxbase29u_net",
			"/NODEFAULTLIB:wxbase29_net",
			"/NODEFAULTLIB:wxbase29ud_xml",
			"/NODEFAULTLIB:wxbase29d_xml",
			"/NODEFAULTLIB:wxbase29u_xml",
			"/NODEFAULTLIB:wxbase29_xml",
			"/NODEFAULTLIB:wxmsw29ud_core",
			"/NODEFAULTLIB:wxmsw29d_core",
			"/NODEFAULTLIB:wxmsw29u_core",
			"/NODEFAULTLIB:wxmsw29_core",
			"/NODEFAULTLIB:wxmsw29ud_adv",
			"/NODEFAULTLIB:wxmsw29d_adv",
			"/NODEFAULTLIB:wxmsw29u_adv",
			"/NODEFAULTLIB:wxmsw29_adv",
			"/NODEFAULTLIB:wxmsw29ud_html",
			"/NODEFAULTLIB:wxmsw29d_html",
			"/NODEFAULTLIB:wxmsw29u_html",
			"/NODEFAULTLIB:wxmsw29_html",
			"/NODEFAULTLIB:wxmsw29ud_qa",
			"/NODEFAULTLIB:wxmsw29d_qa",
			"/NODEFAULTLIB:wxmsw29u_qa",
			"/NODEFAULTLIB:wxmsw29_qa",
			"/NODEFAULTLIB:wxmsw29ud_xrc",
			"/NODEFAULTLIB:wxmsw29d_xrc",
			"/NODEFAULTLIB:wxmsw29u_xrc",
			"/NODEFAULTLIB:wxmsw29_xrc",
			"/NODEFAULTLIB:wxmsw29ud_aui",
			"/NODEFAULTLIB:wxmsw29d_aui",
			"/NODEFAULTLIB:wxmsw29u_aui",
			"/NODEFAULTLIB:wxmsw29_aui",
			"/NODEFAULTLIB:wxmsw29ud_propgrid",
			"/NODEFAULTLIB:wxmsw29d_propgrid",
			"/NODEFAULTLIB:wxmsw29u_propgrid",
			"/NODEFAULTLIB:wxmsw29_propgrid",
			"/NODEFAULTLIB:wxmsw29ud_ribbon",
			"/NODEFAULTLIB:wxmsw29d_ribbon",
			"/NODEFAULTLIB:wxmsw29u_ribbon",
			"/NODEFAULTLIB:wxmsw29_ribbon",
			"/NODEFAULTLIB:wxmsw29ud_richtext",
			"/NODEFAULTLIB:wxmsw29d_richtext",
			"/NODEFAULTLIB:wxmsw29u_richtext",
			"/NODEFAULTLIB:wxmsw29_richtext",
			"/NODEFAULTLIB:wxmsw29ud_media",
			"/NODEFAULTLIB:wxmsw29d_media",
			"/NODEFAULTLIB:wxmsw29u_media",
			"/NODEFAULTLIB:wxmsw29_media",
			"/NODEFAULTLIB:wxmsw29ud_stc",
			"/NODEFAULTLIB:wxmsw29d_stc",
			"/NODEFAULTLIB:wxmsw29u_stc",
			"/NODEFAULTLIB:wxmsw29_stc",
		}

	project "Platform"
		kind "SharedLib"
		language "C++"
		defines
		{
			"PLATFORM_EXPORTS",
		}
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
		defines
		{
			"FOUNDATION_EXPORTS",
		}
		includedirs
		{
			"Dependencies/Expat",
			"Dependencies/zlib",
		}
		files
		{
			"Foundation/**.h",
			"Foundation/**.cpp",
		}
		links
		{
			"Platform",
			"ws2_32",
			"Expat",
			"zlib",
		}

	project "Pipeline"
		kind "SharedLib"
		language "C++"
		defines
		{
			"PIPELINE_EXPORTS",
		}
		includedirs
		{
			"Dependencies/squish",
			"Dependencies/nvtt",
			"Dependencies/nvtt/src",
			"Dependencies/tiff",
			"Dependencies/tiff/libtiff",
		}
		files
		{
			"Pipeline/**.h",
			"Pipeline/**.cpp",
		}
		links
		{
			"Platform",
			"Foundation",
			"squish",
			"nvtt",
			"tiff",
		}

		configuration "windows"
			includedirs
			{
				"Dependencies/nvtt/project/vc8",
			}

	project "Core"
		kind "SharedLib"
		language "C++"
		defines
		{
			"CORE_EXPORTS",
		}
		files
		{
			"Core/**.h",
			"Core/**.cpp",
		}
		links
		{
			"Foundation",
			"Platform",
			"opengl32",
			"glu32",
			"d3d9",
			"d3dx9",
		}	

		configuration "windows"
			includedirs
			{
				os.getenv( "DXSDK_DIR" ) .. "/include"
			}
		configuration { "windows", "x32" }
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "/lib/x86"
			}
		configuration { "windows", "x64" }
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "/lib/x64"
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

		configuration "windows"
			includedirs
			{
				os.getenv( "DXSDK_DIR" ) .. "/include"
			}
		configuration { "windows", "x32" }
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "/lib/x86"
			}
		configuration { "windows", "x64" }
			libdirs
			{
				os.getenv( "DXSDK_DIR" ) .. "/lib/x64"
			}

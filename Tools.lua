require "Dependencies/Helium"
require "Dependencies/wxWidgets"
require "Helium"

prefix = "Helium-Tools-"
group "Tools"

dofile "Core.lua"
dofile "Shared.lua"

project( prefix .. "PcSupport" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PcSupport", "PC_SUPPORT" )

	files
	{
		"PcSupport/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "PreprocessingPc" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "PreprocessingPc", "PREPROCESSING_PC" )
	Helium.DoGraphicsProjectSettings()

	files
	{
		"PreprocessingPc/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "PcSupport",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

project( prefix .. "EditorSupport" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EditorSupport", "EDITOR_SUPPORT" )
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"EditorSupport/*",
	}

	includedirs
	{
		"Dependencies/nvtt",
		"Dependencies/nvtt/extern/poshlib",
		"Dependencies/nvtt/src",
		"Dependencies/nvtt/src/nvtt/squish",
		"Dependencies/freetype/include",
		"Dependencies/libpng",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Math",
			prefix .. "MathSimd",

			"freetype",
			"libpng",
			"nvtt",
			"zlib",
			"mongo-c",
		}

project( prefix .. "EditorScene" )

	Helium.DoModuleProjectSettings( ".", "HELIUM", "EditorScene", "EDITOR_SCENE" )

	files
	{
		"EditorScene/*",
	}

	configuration "SharedLib"
		links
		{
			prefix .. "Engine",
			prefix .. "EngineJobs",
			prefix .. "Windowing",
			prefix .. "Rendering",
			prefix .. "GraphicsTypes",
			prefix .. "GraphicsJobs",
			prefix .. "Graphics",
			prefix .. "Framework",
			prefix .. "PcSupport",
			prefix .. "PreprocessingPc",
			prefix .. "EditorSupport",

			-- core
			prefix .. "Platform",
			prefix .. "Foundation",
			prefix .. "Application",
			prefix .. "Reflect",
			prefix .. "Persist",
			prefix .. "Inspect",
			prefix .. "Math",
			prefix .. "MathSimd",
		}

	if _OPTIONS[ "gfxapi" ] == "direct3d" then
		links
		{
			prefix .. "RenderingD3D9",
		}
	elseif _OPTIONS[ "gfxapi" ] == "opengl" then
		links
		{
			prefix .. "RenderingGL",
		}
	end

project( prefix .. "Editor" )

	kind "ConsoleApp"

	configuration { "windows", "Release" }
		kind "WindowedApp"
	configuration {}

	Helium.DoBasicProjectSettings()
	Helium.DoGraphicsProjectSettings()
	Helium.DoFbxProjectSettings()

	files
	{
		"Editor/**.h",
		"Editor/**.c",
		"Editor/**.cpp",
	}

	flags
	{
		"WinMain"
	}

	defines
	{
		"HELIUM_MODULE=Editor",
		"WXUSINGDLL=1",
		"wxNO_EXPAT_LIB=1",
		"wxNO_JPEG_LIB=1",
		"wxNO_PNG_LIB=1",
		"wxNO_TIFF_LIB=1",
		"wxNO_ZLIB_LIB=1",
	}

	includedirs
	{
		"Dependencies/freetype/include",
		"Dependencies/p4api/include",
		"Dependencies/wxWidgets/include",
		"Dependencies/bullet/src",
		"Example",
	}

	if os.get() == "windows" then
		pchheader( "EditorPch.h" )
		pchsource( "Editor/EditorPch.cpp" )
	else
		includedirs
		{
			"Editor",
		}
	end

	if _OPTIONS[ "gfxapi" ] == "direct3d" then
		links
		{
			prefix .. "RenderingD3D9",
		}
	elseif _OPTIONS[ "gfxapi" ] == "opengl" then
		links
		{
			prefix .. "RenderingGL",
		}
	end

	links
	{
		prefix .. "EditorScene",
		prefix .. "Ois",
		prefix .. "Bullet",
		prefix .. "Components",
		prefix .. "FrameworkImpl",
		prefix .. "PreprocessingPc",
		prefix .. "PcSupport",
		prefix .. "EditorSupport",
		prefix .. "Framework",
		prefix .. "Graphics",
		prefix .. "GraphicsJobs",
		prefix .. "GraphicsTypes",
		prefix .. "Rendering",
		prefix .. "Windowing",
		prefix .. "EngineJobs",
		prefix .. "Engine",

		-- core
		prefix .. "MathSimd",
		prefix .. "Math",
		prefix .. "Inspect",
		prefix .. "Persist",
		prefix .. "Reflect",
		prefix .. "Application",
		prefix .. "Foundation",
		prefix .. "Platform",

		"bullet",
		"mongo-c",
		"ois",
	}

	configuration "linux"
		defines
		{
			"_FILE_OFFSET_BITS=64",
			"__WXGTK__",
			"HAVE_TYPE_TRAITS",
		}

	configuration { "linux", "x32", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-32/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-32/lib",
		}

	configuration { "linux", "x32", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-32/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-32/lib",
		}

	configuration { "linux", "x64", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-64/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-debug-unicode-64/lib",
		}

	configuration { "linux", "x64", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-64/lib/wx/include/gtk2-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/linuxbuild-release-unicode-64/lib",
		}

	configuration { "linux", "x32" }
		libdirs
		{
			"Dependencies/p4api/lib/linux/x32",
		}

	configuration { "linux", "x64" }
		libdirs
		{
			"Dependencies/p4api/lib/linux/x64",
		}

	configuration "linux"
		links
		{
			"client",
			"rpc",
			"supp",
			"p4sslstub",
			"wx_gtk2u_gl-" .. wxVersion .. ".dylib",
			"wx_gtk2u-" .. wxVersion .. ".dylib",
			"GL",
			"X11",
			"Xrandr",
			"Xi",
			"pthread",
			"dl",
			"rt",
			"m",
			"stdc++",
		}

	configuration "macosx"
		defines
		{
			"_FILE_OFFSET_BITS=64",
			"__WXMAC__",
			"__WXOSX__",
			"__WXOSX_COCOA__",
			"HAVE_TYPE_TRAITS",
		}

	configuration { "macosx", "x32", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-32/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-32/lib",
		}

	configuration { "macosx", "x32", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-32/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-32/lib",
		}

	configuration { "macosx", "x64", "Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-64/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-debug-unicode-64/lib",
		}

	configuration { "macosx", "x64", "not Debug" }
		includedirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-64/lib/wx/include/osx_cocoa-unicode-" .. wxVersion,
		}
		libdirs
		{
			"Dependencies/wxWidgets/macbuild-release-unicode-64/lib",
		}

	configuration { "macosx", "x32" }
		libdirs
		{
			"Dependencies/p4api/lib/macosx/x32",
		}

	configuration { "macosx", "x64" }
		libdirs
		{
			"Dependencies/p4api/lib/macosx/x64",
		}

	configuration "macosx"
		links
		{
			"client",
			"rpc",
			"supp",
			"p4sslstub",
			"wx_osx_cocoau_gl-" .. wxVersion .. ".dylib",
			"wx_osx_cocoau-" .. wxVersion .. ".dylib",
		}
		linkoptions
		{
			"-framework Foundation",
			"-framework IOKit",
			"-framework Carbon",
			"-framework Cocoa",
			"-framework AudioToolbox",
			"-framework System",
			"-framework OpenGL",
			"-framework AGL",
		}

	configuration "windows"
		files
		{
			"Editor/Editor.rc",
		}
		includedirs
		{
			"Dependencies/wxWidgets/include/msvc",
		}
		linkoptions -- We build monolithic wx, so ignore all the legacy non-monolithic #pragma comment directives (on windows only)
		{
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "ud",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "d",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "u",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort,
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "ud_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "d_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "u_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "_net",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "ud_xml",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "d_xml",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "u_xml",
			"/NODEFAULTLIB:wxbase" .. wxVersionShort .. "_xml",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_core",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_adv",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_html",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_qa",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_xrc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_aui",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_propgrid",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_ribbon",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_richtext",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_media",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_stc",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_gl",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "ud_webview",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "d_webview",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "u_webview",
			"/NODEFAULTLIB:wxmsw" .. wxVersionShort .. "_webview",
		}

	-- per architecture
	configuration { "windows", "x32" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_dll",
		}
	configuration { "windows", "x64" }
		libdirs
		{
			"Dependencies/wxWidgets/lib/vc_x64_dll",
		}

	-- per configuration
	configuration { "windows", "Debug" }
		links
		{
			"wxmsw" .. wxVersionShort .. "ud"
		}
	configuration { "windows", "not Debug" }
		links
		{
			"wxmsw" .. wxVersionShort .. "u"
		}

	-- per architecture, per configuration
	configuration { "windows", "x32", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/Win32/Debug",
		}
	configuration { "windows", "x32", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/Win32/Release",
		}
	configuration { "windows", "x64", "Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/x64/Debug",
		}
	configuration { "windows", "x64", "not Debug" }
		libdirs
		{
			"Dependencies/p4api/lib/" .. _ACTION .. "/x64/Release",
		}

	configuration "windows"
		links
		{
			"libclient",
			"librpc",
			"libsupp",
			"libp4sslstub",
		}

Helium.DoGameModuleProjectSettings( "PhysicsDemo" )
Helium.DoGameModuleProjectSettings( "ShapeShooter" )
Helium.DoGameModuleProjectSettings( "SideScroller" )
